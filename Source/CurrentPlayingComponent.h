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
                                public IMyComponent,
                                public juce::ChangeListener,
                                public juce::Timer
{
public:
    CurrentPlayingComponent();
    ~CurrentPlayingComponent() override;

    /**
     * \brief Called when changes in transportSource are reported
     */
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    /**
     * \brief Hands off the processing to transportSource by passing it
     * the AudioSourceChannelInfo struct that we have been passed via the AudioAppComponent class
     */
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
