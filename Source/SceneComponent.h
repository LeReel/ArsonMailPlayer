#pragma once

#include "CurrentPlayingComponent.h"

class SceneComponent : public juce::Component,
                       public IMyComponent
    //? Class can inherit typeWanted Listener
{
public:
    SceneComponent();
    ~SceneComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    TableSongListComponent* GetSongList()
    {
        return &songsList;
    }

    /// Event function
    void onSongChose(SongTableElement& _song);

    void openButtonClicked();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SceneComponent)

    TableSongListComponent songsList;

    CurrentPlayingComponent currentPlaying;

    juce::TextButton openButton;
    std::unique_ptr<juce::FileChooser> chooser;
};
