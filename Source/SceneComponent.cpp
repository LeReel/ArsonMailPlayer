#include "SceneComponent.h"

SceneComponent::SceneComponent()
{
    setSize(getWidth(), getHeight());

    //TODO: launch quickstart.py to log into mailbox that holds .mp3 attachments
    //TODO: launch attachments retrieving to populate songList

    //songsList.onChange = [this] { onSongChoosed(); };
    songsList.SetSceneOwner(this);
    addAndMakeVisible(&songsList);
    addAndMakeVisible(&currentPlaying);
    currentPlaying.SetSceneComponent(this);

    addAndMakeVisible(&openButton);
    openButton.setButtonText("Open");
    openButton.onClick = [this] { openButtonClicked(); };
    openButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslategrey);
    openButton.setEnabled(true);

    addAndMakeVisible(fetchButton);
    fetchButton.onClick = [this] { fetchButtonClicked(); };

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
    int _width = getWidth(), _height = getHeight();

    songsList.setBounds(0, 0, _width, _height / 3);

    currentPlaying.setBounds(0, _height - _height / 4, _width, _height / 4);

    openButton.setBounds(0, _height - _height / 4, _width / 8, _height / 8);
    fetchButton.setBounds(0, _height - _height / 8, _width / 8, _height / 8);

    if(isAPIManagerEnabled)
    {
        APIManager.setBounds(0, _height/3, _width, _height/3);
    }
}

void SceneComponent::onSongChoosed(SongTableElement& _song)
{
    currentPlaying.OnSongChoose(_song);
}

void SceneComponent::openButtonClicked()
{
    juce::Array<juce::String> _s = {"*.wav ", " *.mp3"};
    chooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...",
                                                  juce::File{},
                                                  "*.mp3");

    constexpr auto chooserFlags = juce::FileBrowserComponent::openMode |
        juce::FileBrowserComponent::canSelectMultipleItems;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& _chooser)
    {
        const juce::Array<juce::File> _files = _chooser.getResults();

        // Adds selected files to songsList
        songsList.InitTableList(_files);
    });
}

void SceneComponent::fetchButtonClicked()
{
    if(isAPIManagerEnabled)return;
    addAndMakeVisible(&APIManager, -3);
    isAPIManagerEnabled = true;
}
