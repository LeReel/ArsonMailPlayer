#include "SceneComponent.h"
#include <stdio.h>

SceneComponent::SceneComponent()
{
    setSize(getWidth(), getHeight());

    songsList.SetOwner(this);
    // Init songsList from songs_infos.json
    songsList.InitTableList(Utils::LoadSongListFromJson());
    addAndMakeVisible(&songsList);

    favoritesList.SetOwner(this);
    // Creates a var object that will holds JSON's favorites
    juce::var _favorites;
    // Writes JSON "favorites" values into var and stores it as an Array
    juce::Array<juce::var>* _favoritesPathsArray = Utils::GetJsonPropertyArray(_favorites,
                                                                               "favorites");
    favoritesList.InitTableList(GetFavoritesArrayFromJson(_favoritesPathsArray));
    // Marks corresponding songsList elements as favorite
    for (SongTableElement* _song : songsList.GetDataList())
    {
        _song->SetIsFavorite(true);
    }
    addAndMakeVisible(&favoritesList);

    // Adds lists to tabComponent
    tabComponent.addTab("Song list", juce::Colours::grey, &songsList, false);
    tabComponent.addTab("Favorites", juce::Colours::grey, &favoritesList, false);
    // Subscribes to tabButtons.onClick() event
    tabComponent.getTabbedButtonBar().getTabButton(0)->addListener(this);
    tabComponent.getTabbedButtonBar().getTabButton(1)->addListener(this);
    addAndMakeVisible(&tabComponent);

    currentPlaying.SetOwner(this);
    addAndMakeVisible(&currentPlaying);

    Utils::InitButton(this,
                      importButton,
                      "Import",
                      [this] { importButtonClicked(); },
                      juce::Colours::darkslategrey,
                      true);
}

SceneComponent::~SceneComponent()
{
    // Uncomment if inheriting Listener
    // songsList.removeListener(this);
}

void SceneComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)); // clear the background

    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1); // draw an outline around the component
}

void SceneComponent::resized()
{
    const int _width = getWidth(), _height = getHeight();

    const int _heightBy4 = _height / 4,
              _widthBy8 = _width / 8;

    const int _heightMinusBy4 = _height - _heightBy4;

    tabComponent.setBounds(0, 0, _width, _heightMinusBy4);

    currentPlaying.setBounds(0, _heightMinusBy4, _width, _heightBy4);

    importButton.setBounds(0, _heightMinusBy4, _widthBy8, _height / 8);
}

juce::Array<juce::File> SceneComponent::GetFavoritesArrayFromJson(juce::Array<juce::var>* _jsonPathsArray)
{
    juce::Array<juce::File> _favoritesList;
    for (int i = 0; i < _jsonPathsArray->size(); ++i)
    {
        juce::String _favoritePathAsString = _jsonPathsArray->getReference(i);
        const int _songsSize = songsList.GetDataList().size();
        for (int j = 0; j < _songsSize; ++j)
        {
            SongTableElement* _songElement = songsList.GetDataList().getReference(j);
            // If songsList[j] is favorite
            if (_songElement->GetAssociatedFile().getFullPathName() == _favoritePathAsString)
            {
                _songElement->SetIsFavorite(true);
                _favoritesList.add(_songElement->GetAssociatedFile());
                break;
            }
        }
    }
    return _favoritesList;
}

void SceneComponent::onSongChose(SongTableElement& _song)
{
    currentPlaying.OnSongChose(_song);

    if (tabComponent.getCurrentTabIndex() == 0) //If Songlist
    {
        favoritesList.SetCurrentSelected(_song);
    }
    else if (tabComponent.getCurrentTabIndex() == 1) //If FavoriteList
    {
        songsList.SetCurrentSelected(_song);
    }

    repaint();
    resized();
}

void SceneComponent::onFavoriteClicked(SongTableElement& _song)
{
    juce::var _parsedJson;
    juce::var _favorites;
    // Retrieve favorites as var (to write in JSON) and Array (to check if already stored)
    juce::Array<juce::var>* _favoritesArray = Utils::GetJsonPropertyArray(_favorites,
                                                                          "favorites",
                                                                          _parsedJson);

    // Updates favoriteList.dataList and json var content
    if (_song.GetIsFavorite())
    {
        favoritesList.AddSongToList(&_song);
        _favorites.append(_song.GetAssociatedFile().getFullPathName());
    }
    else
    {
        const int _favoritesSize = favoritesList.GetDataList().size();
        for (int i = 0; i < _favoritesSize; ++i)
        {
            juce::String _favorite = _favoritesArray->getReference(i);
            if (_favorite == _song.GetAssociatedFile().getFullPathName())
            {
                _favorites.remove(i);
                break;
            }
        }
        favoritesList.RemoveSongFromList(&_song);
    }

    const juce::String _jsonFormattedString = juce::JSON::toString(_parsedJson);
    // Replace json file's content with new (formatted) string
    jassert(Utils::GetJSONFile().replaceWithText(_jsonFormattedString));
}

void SceneComponent::buttonClicked(juce::Button* _button)
{
    currentTab = tabComponent.getCurrentTabIndex();
    resized();
    repaint();
}

void SceneComponent::importButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser>("Select folder to import",
                                                  juce::File{});

    constexpr auto chooserFlags =
        juce::FileBrowserComponent::openMode |
        juce::FileBrowserComponent::canSelectDirectories |
        juce::FileBrowserComponent::canSelectMultipleItems;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& _chooser)
    {
        // Create File variable from given path
        juce::var _parsedJson;
        juce::var _foldersPaths;
        // Retrieve favorites as var (to write in JSON) and Array (to check if already stored)
        const juce::Array<juce::var>* _pathsArray = Utils::GetJsonPropertyArray(_foldersPaths,
            "paths",
            _parsedJson);

        // Array that will holds folder's children files
        juce::Array<juce::File> _childrenFiles;

        // Append selected folders' path to array
        for (const juce::File& _result : _chooser.getResults())
        {
            juce::String _folderPath = _result.getFullPathName();
            // Checks if path is already stored
            if (_pathsArray->contains(_folderPath))
            {
                return;
            }
            _foldersPaths.append(_folderPath);

            for (juce::File& _childFile : _result.findChildFiles(2, true, "*.mp3"))
            {
                _childrenFiles.add(_childFile);
            }
        }
        const juce::String _jsonFormattedString = juce::JSON::toString(_parsedJson);
        jassert(Utils::GetJSONFile().replaceWithText(_jsonFormattedString));

        songsList.InitTableList(_childrenFiles);
    });
}
