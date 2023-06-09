#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(800, 800);

    addAndMakeVisible(sceneComponent);
}

void MainComponent::paint(juce::Graphics& g)
{
}

void MainComponent::resized()
{
    sizeX = getWidth();
    sizeY = getHeight();

    const juce::Rectangle<int> _sceneRect(0, 0, sizeX, sizeY);
    // Coordinates are relative to parent
    sceneComponent.setBounds(_sceneRect);
}
