#pragma once

#include "CurrentPlayingComponent.h"

class SceneComponent : public juce::Component //, private juce::Thread
    // Class can inherit typeWanted Listener
    // public juce::ComboBox::Listener
{
public:
    SceneComponent();
    ~SceneComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    //void run() override;

    TableSongListComponent* GetSongList()
    {
        return &songsList;
    }

    //   juce::String GetResultText(const juce::URL& _url);

    // bool RefreshToken(int& statusCode);
    // void RetrieveAttachments(const juce::String& _jsonString);

    void Fetch();

    /// Event function
    /// Override event function if inheriting Listener
    void onSongChoosed(SongTableElement& _song);

    void openButtonClicked();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SceneComponent)

    juce::TextEditor urlBox;
    juce::TextButton fetchButton{"Download URL Contents"};

    juce::CodeDocument resultsDocument;
    juce::CodeEditorComponent resultsBox{resultsDocument, nullptr};

    TableSongListComponent songsList;

    CurrentPlayingComponent currentPlaying;

    juce::TextButton openButton;
    std::unique_ptr<juce::FileChooser> chooser;
};

