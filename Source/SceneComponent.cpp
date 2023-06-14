#include "SceneComponent.h"

SceneComponent::SceneComponent()
{
    setSize(getWidth(), getHeight());

    songsList.SetOwner(this);
    // Adds selected files to songsList
    songsList.InitTableList(Utils::LoadSongListFromJson());
    addAndMakeVisible(&songsList);

    favoritesList.SetOwner(this);
    juce::var _favorites;
    juce::Array<juce::var>* _favoritesPathsArray = Utils::GetJsonPropertyArray(_favorites,
                                                                               "favorites");
    favoritesList.InitTableList(GetFavoritesArrayFromJson(_favoritesPathsArray));
    for (SongTableElement* _favorite : favoritesList.GetDataList())
    {
        _favorite->SetIsFavorite(true);
    }
    addAndMakeVisible(&favoritesList);

    tabComponent.addTab("Song list", juce::Colours::grey, &songsList, false);
    tabComponent.addTab("Favorites", juce::Colours::grey, &favoritesList, false);

    tabComponent.getTabbedButtonBar().getTabButton(0)->addListener(this);
    tabComponent.getTabbedButtonBar().getTabButton(1)->addListener(this);

    addAndMakeVisible(&tabComponent);

    currentPlaying.SetOwner(this);
    addAndMakeVisible(&currentPlaying);

    Utils::InitButton(this,
                      openButton,
                      "Open",
                      [this] { openButtonClicked(); },
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

    const int _heightBy4 = _height / 4;

    const int _heightMinusBy4 = _height - _heightBy4;

    menuBar.setBounds(0, 0, _width, _heightBy4);

    tabComponent.setBounds(0, 0, _width, _heightMinusBy4);

    currentPlaying.setBounds(0, _heightMinusBy4, _width, _heightBy4);

    openButton.setBounds(0, _heightMinusBy4, _width / 8, _height / 8);
}

juce::Array<juce::File> SceneComponent::GetFavoritesArrayFromJson(juce::Array<juce::var>* _pathsArray)
{
    juce::Array<juce::File> _favoritesList;
    for (int i = 0; i < _pathsArray->size(); ++i)
    {
        juce::String _favoritePathAsString = _pathsArray->getReference(i);
        const int _songsSize = songsList.GetDataList().size();
        for (int j = 0; j < _songsSize; ++j)
        {
            SongTableElement* _songElement = songsList.GetDataList().getReference(j);
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
    jassert(Utils::GetJSONFile().replaceWithText(_jsonFormattedString));

    repaint();
    resized();
}

void SceneComponent::buttonClicked(juce::Button* _button)
{
    resized();
    repaint();
}

void SceneComponent::openButtonClicked()
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
        juce::var _paths;
        // Retrieve favorites as var (to write in JSON) and Array (to check if already stored)
        const juce::Array<juce::var>* _pathsArray = Utils::GetJsonPropertyArray(_paths,
            "paths",
            _parsedJson);

        // Append selected folders' path to array
        for (const juce::File& _result : _chooser.getResults())
        {
            juce::String _path = _result.getFullPathName();
            // Checks if path is already stored
            if (_pathsArray->contains(_path))
            {
                return;
            }
            _paths.append(_path);
        }
        const juce::String _jsonFormattedString = juce::JSON::toString(_parsedJson);
        jassert(Utils::GetJSONFile().replaceWithText(_jsonFormattedString));

        // Adds new selected folder's children files to songsList
        const juce::Array<juce::File> _children = _chooser.getResult().findChildFiles(2, true, "*.mp3");
        songsList.InitTableList(_children);
    });
}
