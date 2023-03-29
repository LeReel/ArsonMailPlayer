#pragma once

#include "SceneComponent.h"

/**
* \brief This component lives inside our window.\n
 This is where all controls and content goes.\n
 All other components will be children of this main component.
 */
class MainComponent : public juce::Component
{
public:
    MainComponent();

    /**
     * \brief Callback called by the operating system when it is time
     * to render your Component on screen
     */
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)

    SceneComponent sceneComponent;

    int sizeX = 0, sizeY = 0;
    juce::String currentSizeAsString;
};

static void DrawGrid(juce::Graphics& g,
                     unsigned int _width,
                     unsigned int _height,
                     unsigned int _spacingX = 50,
                     unsigned int _spacingY = 50);
