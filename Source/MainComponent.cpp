#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(800, 800);

    // This is where any child component is added
    // and initialised with any special settings that it needs.
    addAndMakeVisible(sceneComponent);
}

void MainComponent::paint(juce::Graphics& g)
{
    Utils::DrawGrid(g, sizeX, sizeY);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    // It is also where you should set the bounds of any child
    // components that your component contains.

    sizeX = getWidth();
    sizeY = getHeight();

    const juce::Rectangle<int> _sceneRect(sizeX / 16, 0, sizeX - sizeX / 8, sizeY);
    // Coordinates are relative to parent
    sceneComponent.setBounds(_sceneRect);
}
