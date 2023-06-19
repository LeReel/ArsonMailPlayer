#pragma once

#include "TableSongListComponent.h"

class SceneComponent;

enum TransportState
{
    Stopped,
    Starting,
    Playing,
    Pausing,
    Paused,
    Stopping
};

class CurrentPlayingComponent : public juce::AudioAppComponent,
                                public MyComponent,
                                public juce::ChangeListener,
                                public juce::Timer
{
public:
    CurrentPlayingComponent();
    ~CurrentPlayingComponent() override;

    // Called when changes in transportSource are reported
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    // Puts transportSource into 'prepared' state
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    // Allows the source to release anything it no longer needs after playback has stopped.
    // This will be called when the source is no longer going to have its getNextAudioBlock() method called,
    // Releases any spare memory, etc. that it might have allocated during the prepareToPlay() call.
    void releaseResources() override;
    // Called repeatedly to fetch subsequent blocks of audio data.
    // After calling the prepareToPlay() method, this callback will be made each time the audio playback hardware
    // (or whatever other destination the audio data is going to) needs another block of data.
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    void timerCallback() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void OnSongChose(SongTableElement& _song);
    void ChangeState(TransportState _state);
    void UpdateLoopState(bool _shouldLoop);
    void UpdateCurrentPlayingPosition(double _newPosition);

    void playButtonClicked();
    void stopButtonClicked();
    void prevButtonClicked() const;
    void nextButtonClicked() const;
    void changeSongClicked(const int _move) const;
    void loopButtonClicked();
    void loopAllButtonClicked();
    void randomButtonClicked();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CurrentPlayingComponent)

    bool isLooping = false;
    bool isLoopAll = false;
    bool isRandom = false;

    bool isSongFinished = false;

    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton prevButton;
    juce::TextButton nextButton;
    juce::ToggleButton loopButton;
    juce::ToggleButton loopAllButton;
    juce::ToggleButton randomButton;

    juce::String currentPlayingString;
    juce::String currentPlayingTimeString;

    std::vector<juce::Button*> transportButtons;

    class CurrentPlayingSlider : public juce::Slider
    {
        CurrentPlayingComponent* currentPlayingComponent = nullptr;

    public:
        CurrentPlayingSlider()
        {
            setMouseDragSensitivity(1);
            setSliderSnapsToMousePosition(true);
        }
        ~CurrentPlayingSlider() override
        {
            currentPlayingComponent = nullptr;
        }
        
        void SetCurrentPlayingComponent(CurrentPlayingComponent* _currentPlayingComponent)
        {
            currentPlayingComponent = _currentPlayingComponent;
        }
        CurrentPlayingComponent& GetCurrentPlayingComponent() const
        {
            return *currentPlayingComponent;
        }
        void valueChanged() override
        {
            if (!currentPlayingComponent)return;
            currentPlayingComponent->UpdateCurrentPlayingPosition(getValue());
        }
    };

    CurrentPlayingSlider currentPlayingSlider;

    /**
     * \brief Stored in a unique_ptr because we need to create these objects dynamically based on the user's actions.
     * It also allows us to check for nullptr for invalid objects.
     */
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::AudioFormatManager formatManager;
    TransportState currentState = Stopped;
};
