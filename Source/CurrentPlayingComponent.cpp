#include <utility>
#include "CurrentPlayingComponent.h"
#include "SceneComponent.h"

CurrentPlayingComponent::CurrentPlayingComponent()
{
    setSize(getBounds().getWidth(), getBounds().getHeight());

    currentPlayingString = "No selected song";

    addAndMakeVisible(&currentPlayingSlider);
    currentPlayingSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    currentPlayingSlider.SetCurrentPlayingComponent(this);

    Utils::InitButton(this,
                      transportButtons,
                      prevButton,
                      "<<",
                      [this] { prevButtonClicked(); },
                      juce::Colours::darkorange,
                      true);
    Utils::InitButton(this,
                      transportButtons,
                      playButton,
                      "Play",
                      [this] { playButtonClicked(); },
                      juce::Colours::darkolivegreen,
                      false);
    Utils::InitButton(this,
                      transportButtons,
                      stopButton,
                      "Stop",
                      [this] { stopButtonClicked(); },
                      juce::Colours::indianred,
                      false);
    Utils::InitButton(this,
                      transportButtons,
                      nextButton,
                      ">>",
                      [this] { nextButtonClicked(); },
                      juce::Colours::darkorange,
                      true);
    Utils::InitButton(this,
                      randomButton,
                      "Random",
                      [this] { randomButtonClicked(); },
                      juce::Colours::yellow,
                      true);
    Utils::InitButton(this,
                      loopButton,
                      "Loop",
                      [this] { loopButtonClicked(); },
                      juce::Colours::yellow,
                      true);
    Utils::InitButton(this,
                      loopAllButton,
                      "Loop all",
                      [this] { loopAllButtonClicked(); },
                      juce::Colours::yellow,
                      true);

    // Registers a list of standards formats. Creates readers for wav/aiff/mp3
    formatManager.registerBasicFormats();

    // Add this as a listener to transportSource object so that
    // we can respond to changes in its state (for example, when it stops):
    transportSource.addChangeListener(this);

    setAudioChannels(0, 2);
}

CurrentPlayingComponent::~CurrentPlayingComponent()
{
    shutdownAudio();
    stopTimer();
}

void CurrentPlayingComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source != &transportSource) return;

    if (transportSource.isPlaying())
        ChangeState(Playing);
    else if ((currentState == Stopping) || (currentState == Playing))
        ChangeState(Stopped);
    else if (currentState == Pausing)
        ChangeState(Paused);
}

//This method needs to be called to put transportSource into 'prepared' state
//(before any call made to getNextAudioBlock().
void CurrentPlayingComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void CurrentPlayingComponent::releaseResources()
{
    transportSource.releaseResources();
}

void CurrentPlayingComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!readerSource)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    transportSource.getNextAudioBlock(bufferToFill);
}

void CurrentPlayingComponent::timerCallback()
{
    const double _position = transportSource.getCurrentPosition();
    const juce::int64 _totalLength = transportSource.getTotalLength();
    const juce::RelativeTime _positionRelative(_position);

    int _min = static_cast<int>(_positionRelative.inMinutes()) % 60;
    int _sec = static_cast<int>(_positionRelative.inSeconds()) % 60;
    int _mil = static_cast<int>(_positionRelative.inMilliseconds()) % 1000;

    currentPlayingTimeString = juce::String::formatted("%02d:%02d:%03d / %d", _min, _sec, _mil, _totalLength);

    currentPlayingSlider.setValue(_position);

    repaint();
}

void CurrentPlayingComponent::paint(juce::Graphics& graphics)
{
    graphics.setColour(juce::Colours::darkorange);

    const juce::Rectangle<int> _localBounds = getLocalBounds();
    graphics.drawText(currentPlayingString,
                      _localBounds,
                      juce::Justification::centredTop,
                      true);
    graphics.drawText(currentPlayingTimeString,
                      _localBounds,
                      juce::Justification::centred,
                      true);
}

void CurrentPlayingComponent::resized()
{
    const int _width = getWidth(), _height = getHeight();
    setSize(_width, _height);

    const int _widthBy4 = _width / 4,
              _widthBy8 = _width / 8,
              _heightBy2 = _height / 2,
              _heightBy3 = _height / 3,
              _heightBy4 = _height / 4;
    const int _transportButtonsX = _width - _widthBy8;

    const int _buttonAmnt = transportButtons.size();
    const int _buttonWidth = _width / _buttonAmnt;

    for (int i = 0; i < _buttonAmnt; ++i)
    {
        juce::Button* _button = transportButtons[i];
        _button->setBounds(_widthBy4 * i, _height - _heightBy3, _buttonWidth, _heightBy4);
    }

    randomButton.setBounds(_transportButtonsX, _heightBy4 - 30, _widthBy8, _heightBy4 - 20);
    loopButton.setBounds(_transportButtonsX, _heightBy4, _widthBy8, _heightBy4 - 20);
    loopAllButton.setBounds(_transportButtonsX, _heightBy4 + 30, _widthBy8, _heightBy4 - 20);

    currentPlayingSlider.setBounds(_widthBy4, _heightBy4, _width / 2, _heightBy3);
}

void CurrentPlayingComponent::OnSongChose(SongTableElement& _song)
{
    //Stops currentPlaying
    if (transportSource.isPlaying())
    {
        stopButtonClicked();
        stopTimer();
    }

    juce::AudioFormatReader* _reader = formatManager.createReaderFor(_song.GetAssociatedFile());
    if (!_reader) return;

    std::unique_ptr<juce::AudioFormatReaderSource> _newSource = std::make_unique<juce::AudioFormatReaderSource>(
        _reader, true);

    transportSource.setSource(_newSource.get(), 0, nullptr, _reader->sampleRate);
    playButton.setEnabled(true);
    readerSource = std::move(_newSource);

    currentPlayingString = _song.GetStringAttribute("Title");
    currentPlayingSlider.setRange(0.0, transportSource.getLengthInSeconds());

    repaint();
    startTimer(1);

    ChangeState(Starting);
}

void CurrentPlayingComponent::ChangeState(TransportState _state)
{
    if (_state == currentState) return;

    currentState = _state;

    switch (currentState)
    {
    case Stopped:
        playButton.setButtonText("Play");
        stopButton.setButtonText("Stop");
        stopButton.setEnabled(false);
        playButton.setEnabled(true);
        transportSource.setPosition(0.0);
        break;

    case Starting:
        transportSource.start();
        break;

    case Playing:
        playButton.setButtonText("Pause");
        stopButton.setButtonText("Stop");
        stopButton.setEnabled(true);
        break;

    case Pausing:
        transportSource.stop();
        break;

    case Paused:
        playButton.setButtonText("Resume");
        stopButton.setButtonText("To Zero");
        break;

    case Stopping:
        transportSource.stop();
        break;
    }
}

void CurrentPlayingComponent::UpdateLoopState(bool _shouldLoop) const
{
    if (!readerSource)return;
    readerSource->setLooping(_shouldLoop);
}

void CurrentPlayingComponent::UpdateCurrentPlayingPosition(double _newPosition)
{
    transportSource.setPosition(_newPosition);
}

void CurrentPlayingComponent::playButtonClicked()
{
    if ((currentState == Stopped) || (currentState == Paused))
        ChangeState(Starting);
    else if (currentState == Playing)
        ChangeState(Pausing);
}

void CurrentPlayingComponent::stopButtonClicked()
{
    if (currentState == Paused)
        ChangeState(Stopped);
    else
        ChangeState(Stopping);
}

void CurrentPlayingComponent::prevButtonClicked()
{
    changeSongClicked(-1);
}

void CurrentPlayingComponent::nextButtonClicked()
{
    changeSongClicked(1);
}

void CurrentPlayingComponent::changeSongClicked(const int _move)
{
    if (SceneComponent* _sC = dynamic_cast<SceneComponent*>(componentOwner))
    {
        _sC->GetSongList()->ChangeCell(_move, isLoopAll, isRandom);
    }
}

void CurrentPlayingComponent::loopButtonClicked() const
{
    UpdateLoopState(loopButton.getToggleState());
}

void CurrentPlayingComponent::loopAllButtonClicked()
{
    isLoopAll = loopAllButton.getToggleState();
}

void CurrentPlayingComponent::randomButtonClicked()
{
    isRandom = randomButton.getToggleState();
}
