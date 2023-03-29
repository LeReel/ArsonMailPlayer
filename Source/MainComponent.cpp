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
    const float _width = sizeX, _height = sizeY;

    DrawGrid(g, _width, _height);

    // Draws a red text displaying windowSize
    g.setColour(juce::Colours::red);
    g.setFont(juce::Font("Times New Roman",
                         20.0f,
                         juce::Font::italic));
    g.drawText(currentSizeAsString,
               getLocalBounds(),
               juce::Justification::centredTop,
               true);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    // It is also where you should set the bounds of any child
    // components that your component contains.

    sizeX = static_cast<float>(getWidth());
    sizeY = static_cast<float>(getHeight());
    currentSizeAsString = juce::String(sizeX) + " x " + juce::String(sizeY);

    juce::Rectangle<int> _sceneRect(sizeX / 16, 0, sizeX - sizeX / 8, sizeY);
    // Coordinates are relative to parent
    sceneComponent.setBounds(_sceneRect);
}

#pragma region Utils
void DrawGrid(juce::Graphics& g,
              unsigned int _width,
              unsigned int _height,
              unsigned int _spacingX,
              unsigned int _spacingY)
{
    for (unsigned int x = 0; x < _width; x += _spacingX)
    {
        for (unsigned int y = 0; y < _height; y += _spacingY)
        {
            g.drawLine(x, y, _width, y, 3);
        }
        g.drawLine(x, 0, x, _height, 3);
    }
}
#pragma endregion Utils
