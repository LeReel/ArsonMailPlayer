#pragma once

#include "SceneComponent.h"

/**
* \brief The component living inside our window.
 This is where everything goes.
 All other components will be children of this main component.
 */
class MainComponent : public juce::Component
{
public:
    MainComponent();

    /**
     * \brief Callback called by the operating system when it is time
     * to render the Component on screen
     */
    void paint(juce::Graphics&) override;
    /**
    * \brief Called when this component's size has been changed.
    * Called synchronously as a result of the setBounds or setSize methods.
    * Repeatedly changing size will repeatedly call resized()
    * (unlike repainting, where multiple calls to repaint() are coalesced together).
     */
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)

    SceneComponent sceneComponent;

    int sizeX = 0, sizeY = 0;
};
