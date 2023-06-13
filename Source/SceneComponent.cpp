#include "SceneComponent.h"

SceneComponent::SceneComponent()
{
    setSize(getWidth(), getHeight());

    const juce::Array<juce::File> _songFiles = Utils::LoadSongListFromJson();

    songsList.SetOwner(this);
    // Adds selected files to songsList
    songsList.InitTableList(_songFiles);
    songsList.InitTableList({});
    addAndMakeVisible(&songsList);

    favoritesList.SetOwner(this);
    //TODO: Init with registered favorites
    favoritesList.InitTableList({});
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
    switch (_song.GetIsFavorite())
    {
    case true:
        favoritesList.AddSongToList(&_song);
        break;
    case false:
        favoritesList.RemoveSongFromList(&_song);
    }

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
        //TODO: Store this method in Utils ?
        // Create File variable from given path
        juce::File _jsonFile(Utils::GetJsonFilePath());
        // Reads JSON and convert it to String
        juce::String _jsonString = _jsonFile.loadFileAsString();
        // Create a var that will store JSON structure
        juce::var _parsedJson;
        // Parse the JSON string
        juce::JSON::parse(_jsonString, _parsedJson);
        // Retrieve paths array as var
        juce::var _paths = _parsedJson.getProperty("paths", 0);
        // Append selected folders' path to array
        for (auto _result : _chooser.getResults())
        {
            juce::String _path = _result.getFullPathName();
            //TODO: Check if path is already stored
            _paths.append(_path);
        }

        juce::String _string = juce::JSON::toString(_parsedJson);

        jassert(_jsonFile.replaceWithText(_string));

        const juce::Array<juce::File> _children = _chooser.getResult().findChildFiles(2, true, "*.mp3");
        // Adds selected files to songsList
        songsList.InitTableList(_children);

        favoritesList.InitTableList({/*favoriteElements*/});
    });
}
