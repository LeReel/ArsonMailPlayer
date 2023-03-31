#pragma once

#include "MailAPIsManager.h"

class SceneComponent : public juce::Component
    // Class can inherit typeWanted Listener
    // public juce::ComboBox::Listener
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
    /// Override event function if inheriting Listener
    void onSongChoosed(SongTableElement& _song);

    void openButtonClicked();
    void fetchButtonClicked();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SceneComponent)

    juce::TextButton fetchButton{"Download URL Contents"};

    TableSongListComponent songsList;

    CurrentPlayingComponent currentPlaying;

    juce::TextButton openButton;
    std::unique_ptr<juce::FileChooser> chooser;
    
    MailAPIsManager APIManager;
    bool isAPIManagerEnabled = false;
};

