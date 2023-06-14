#pragma once

#include "CurrentPlayingComponent.h"

class SceneComponent : public juce::Component,
                       public::juce::TabBarButton::Listener,
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

    TableSongListComponent* GetActiveList()
    {
        switch (tabComponent.getCurrentTabIndex())
        {
        case 0: //SongList
            return &songsList;
        case 1: //FavoriteList
            return &favoritesList;
        default:
            return nullptr;
        }
    }
    
    juce::Array<juce::File> GetFavoritesArrayFromJson(juce::Array<juce::var>* _pathsArray);

    /// Event functions
    void onSongChose(SongTableElement& _song);
    void onFavoriteClicked(SongTableElement& _song);
    void buttonClicked(juce::Button* _button) override;

    void openButtonClicked();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SceneComponent)

    int currentTab = 1;

    juce::TabbedComponent tabComponent{juce::TabbedButtonBar::TabsAtLeft};

    juce::MenuBarComponent menuBar;

    TableSongListComponent songsList;
    TableSongListComponent favoritesList;

    CurrentPlayingComponent currentPlaying;

    juce::TextButton openButton;
    std::unique_ptr<juce::FileChooser> chooser;
};
