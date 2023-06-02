#include "SceneComponent.h"

SceneComponent::SceneComponent()
{
    setSize(getWidth(), getHeight());

    songsList.SetOwner(this);
    addAndMakeVisible(&songsList);
    currentPlaying.SetOwner(this);
    addAndMakeVisible(&currentPlaying);

    Utils::InitButton(this, openButton, "Open", [this] { openButtonClicked(); }, juce::Colours::darkslategrey, true);
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

    songsList.setBounds(0, 0, _width, _height - _height / 3);

    currentPlaying.setBounds(0, _height - _height / 4, _width, _height / 4);

    openButton.setBounds(0, _height - _height / 4, _width / 8, _height / 8);
}

void SceneComponent::onSongChose(SongTableElement& _song)
{
    currentPlaying.OnSongChoose(_song);
}

void SceneComponent::openButtonClicked()
{
    juce::Array<juce::String> _s = {"*.wav ", "*.mp3"};
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
