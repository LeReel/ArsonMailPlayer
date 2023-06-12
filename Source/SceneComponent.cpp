#include "SceneComponent.h"

SceneComponent::SceneComponent()
{
    setSize(getWidth(), getHeight());

    const juce::Array<juce::File> _songFiles = Utils::LoadSongListFromJson();

    songsList.SetOwner(this);
    // Adds selected files to songsList
    //TODO: uncomment when writingToJsonFile is done
    // songsList.InitTableList(_songFiles);
    songsList.InitTableList({});
    addAndMakeVisible(&songsList);

    favoritesList.SetOwner(this);
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

    constexpr auto chooserFlags = juce::FileBrowserComponent::openMode |
        juce::FileBrowserComponent::canSelectDirectories;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& _chooser)
    {
        //TODO: Write selected folder to Json
        const juce::String _jsonPath = Utils::GetJsonFilePath();
        juce::File _jsonFile(_jsonPath);

        Utils::

        auto _obj = std::make_unique<juce::DynamicObject>();
        _obj->setProperty("paths", "path1");
        auto _jsonVar = juce::var(_obj.release());
        juce::FileOutputStream _output(_jsonFile);
        juce::JSON::writeToStream(_output, _jsonVar);
        
        const juce::Array<juce::File> _children = _chooser.getResult().findChildFiles(2, true, "*.mp3");
        // Adds selected files to songsList
        songsList.InitTableList(_children);

        //! Init with JSon (?)
        favoritesList.InitTableList({/*favoriteElements*/});
    });
}

//		"C:\\Users\\GUYO0208\\Music\\Musique\\DirtyWater"
