#pragma once

#include "SceneComponent.h"

/*
    This class implements the desktop window that contains an instance of our MainComponent class.
    It serves as the frame in which the app's graphical interface lives.
*/
class MainWindow : public juce::DocumentWindow
{
    SceneComponent sceneComponent;
    
    //Shorthand for deleting a class's copy constructor and copy assignment operator (NON_COPYABLE)
    //+ Embeds a leak-detecting object inside class (LEAK_DETECTOR)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)

public:
    MainWindow(const juce::String& _name);

    void closeButtonPressed() override;
};
