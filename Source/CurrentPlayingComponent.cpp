#include <utility>
#include "CurrentPlayingComponent.h"
#include "SceneComponent.h"

CurrentPlayingComponent::CurrentPlayingComponent()
{
    // Passing transportButtons as parameter for resized() call
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
}

CurrentPlayingComponent::~CurrentPlayingComponent()
{
    shutdownAudio();
    stopTimer();
}

// Called when changes in transportSource are reported
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

//Called by setSource() and needs to be called to put transportSource into 'prepared' state
// (before any call made to getNextAudioBlock()
void CurrentPlayingComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
// Allows the source to release anything it no longer needs after playback has stopped.
// This will be called when the source is no longer going to have its getNextAudioBlock() method called,
// Releases any spare memory, etc. that it might have allocated during the prepareToPlay() call.
void CurrentPlayingComponent::releaseResources()
{
    transportSource.releaseResources();
}
// Called repeatedly to fetch subsequent blocks of audio data.
// After calling the prepareToPlay() method, this callback will be made each time the audio playback hardware
// (or whatever other destination the audio data is going to) needs another block of data.
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
    const double _position = transportSource.getCurrentPosition(), _totalLength = transportSource.getLengthInSeconds();
    const juce::RelativeTime _relativePosition(_position), _relativeLength(_totalLength);

    const int _posMin = static_cast<int>(_relativePosition.inMinutes()) % 60,
              _posSec = static_cast<int>(_relativePosition.inSeconds()) % 60,
              _posMil = static_cast<int>(_relativePosition.inMilliseconds()) % 1000,

              _totalMin = static_cast<int>(_relativeLength.inMinutes()) % 60,
              _totalSec = static_cast<int>(_relativeLength.inSeconds()) % 60,
              _totalMil = static_cast<int>(_relativeLength.inMilliseconds()) % 1000;

    if (_posMin >= _totalMin)
    {
        if (_posSec >= _totalSec)
        {
            if (_posMil >= _totalMil)
            {
                isSongFinished = true;
            }
        }
    }

    currentPlayingTimeString = juce::String::formatted("%02d:%02d:%03d / %02d:%02d:%03d",
                                                       _posMin,
                                                       _posSec,
                                                       _posMil,
                                                       _totalMin,
                                                       _totalSec,
                                                       _totalMil);

    currentPlayingSlider.setValue(_position);

    repaint();
}

void CurrentPlayingComponent::paint(juce::Graphics& graphics)
{
    graphics.setColour(juce::Colours::darkorange);

    const juce::Rectangle<int> _localBounds = getLocalBounds();
    graphics.drawFittedText(currentPlayingString,
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
              _heightBy3 = _height / 3,
              _heightBy4 = _height / 4;
    const int _transportButtonsX = _width - _widthBy8;

    const int _buttonAmnt = transportButtons.size(), _buttonWidth = _width / _buttonAmnt;

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

    //Creates a reader for new selected song
    juce::AudioFormatReader* _reader = formatManager.createReaderFor(_song.GetAssociatedFile());
    if (!_reader) return;
    // Creates new source for reader
    std::unique_ptr<juce::AudioFormatReaderSource> _newSource = std::make_unique<juce::AudioFormatReaderSource>(
        _reader, true);

    // Sets newSource as transportSource.masterSource
    transportSource.setSource(_newSource.get(), 0, nullptr, _reader->sampleRate);
    playButton.setEnabled(true);
    // Transfers ownership of the object originally managed by newSource to readerSource
    // so it can be accessed via readerSource.
    readerSource = std::move(_newSource);
    readerSource->setLooping(isLooping);

    currentPlayingString = _song.GetStringAttribute("Title") + "\n" + _song.GetStringAttribute("Artist") + "\n" + _song.GetStringAttribute("Album");
    currentPlayingSlider.setRange(0.0, transportSource.getLengthInSeconds());

    repaint();
    startTimer(1);

    ChangeState(Starting);
}

//Called by transportButtons and this->OnSongChose
void CurrentPlayingComponent::ChangeState(TransportState _state)
{
    if (_state == currentState) return;

    currentState = _state;

    switch (currentState)
    {
    case Stopped:
        if (isSongFinished)
        {
            isSongFinished = false;
            if (!isLooping)
            {
                nextButtonClicked();
            }
        }
        else //If stop is clicked
        {
            playButton.setButtonText("Play");
            stopButton.setButtonText("Stop");
            stopButton.setEnabled(false);
            playButton.setEnabled(true);
            transportSource.setPosition(0.0);
        }
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

void CurrentPlayingComponent::UpdateLoopState(bool _shouldLoop)
{
    if (!readerSource)return;
    isLooping = _shouldLoop;
    readerSource->setLooping(isLooping);
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
void CurrentPlayingComponent::prevButtonClicked() const
{
    changeSongClicked(-1);
}
void CurrentPlayingComponent::nextButtonClicked() const
{
    changeSongClicked(1);
}
void CurrentPlayingComponent::changeSongClicked(const int _move) const
{
    if (const auto _sC = dynamic_cast<SceneComponent*>(componentOwner))
    {
        // Updates TableSongList.currentSelected before calling sceneComponent.SongChose()
        _sC->GetActiveList()->ChangeCell(_move, isLoopAll, isRandom);
    }
}

void CurrentPlayingComponent::loopButtonClicked()
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
