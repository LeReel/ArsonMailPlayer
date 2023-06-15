#pragma once

#include "CurrentPlayingComponent.h"

class SceneComponent : public juce::Component,
                       public juce::TabBarButton::Listener,
                       public IMyComponent
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
    // Returns an Array containing associated files of each favorite song of JSON file
    juce::Array<juce::File> GetFavoritesArrayFromJson(juce::Array<juce::var>* _jsonPathsArray);

    /// Event functions
    // Called when prevButton/nextButton is clicked or when a cell is double_clicked
    void onSongChose(SongTableElement& _song);
    // Called when a cell from favoriteColumn is clicked
    void onFavoriteClicked(SongTableElement& _song);
    // Override that lets this class listens to TabBarButton
    void buttonClicked(juce::Button* _button) override;
    // Opens a file chooser, appends selected paths to JSON and then init songsList
    void openButtonClicked();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SceneComponent)
    
    juce::TabbedComponent tabComponent{juce::TabbedButtonBar::TabsAtLeft};

    TableSongListComponent songsList;
    TableSongListComponent favoritesList;
    // Component that will holds current song's time infos, metadata and transport buttons
    CurrentPlayingComponent currentPlaying;

    juce::TextButton openButton;
    std::unique_ptr<juce::FileChooser> chooser;

    int currentTab = 0;

};
