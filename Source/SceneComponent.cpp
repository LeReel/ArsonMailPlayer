#include "SceneComponent.h"

SceneComponent::SceneComponent()
{
    setSize(getWidth(), getHeight());

    songsList.SetOwner(this);
    // Adds selected files to songsList
    songsList.InitTableList({});
    addAndMakeVisible(&songsList);

    favoritesList.SetOwner(this);
    //! TEMPORARY (init with JSon)
    favoritesList.InitTableList({});
    addAndMakeVisible(&favoritesList);

    tabComponent.addTab("Song list", juce::Colours::grey, &songsList, false);
    tabComponent.addTab("Favorites", juce::Colours::grey, &favoritesList, false);

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
}

void SceneComponent::openButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser>("Select audio files to import",
                                                  juce::File{},
                                                  "*.mp3;*.wav");

    constexpr auto chooserFlags = juce::FileBrowserComponent::openMode |
        juce::FileBrowserComponent::canSelectMultipleItems;

    //TODO: Init tableList with a JSon at first launch of app (retains paths, favorites,...)

    //juce::String _jsonPath = "../../Source/songs_infos.json";
    juce::String _jsonPath = juce::File::getCurrentWorkingDirectory().getFullPathName() +
        "\\..\\..\\Source\\songs_infos.json";
    juce::File _jsonFile(_jsonPath);
    juce::String _jsonString = _jsonFile.loadFileAsString();
    juce::var _parsedJson;
    if (juce::JSON::parse(_jsonString, _parsedJson).wasOk())
    {
        juce::String _test = _parsedJson["testProperty"];
    }

    juce::Array<juce::File> _files;
    juce::Array<juce::String> _paths;
    const int _pathsSize = _paths.size();
    for (int i = 0; i < _pathsSize; ++i)
    {
        const juce::String _absolutePath = _paths[i];
        juce::File _file(_absolutePath);
        _files.add(_file);
    }

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& _chooser)
    {
        // Adds selected files to songsList
        songsList.InitTableList(_chooser.getResults());

        //! TEMPORARY (init with JSon)
        favoritesList.InitTableList({});
    });
}
