#include "Utils.h"

void Utils::InitButton(juce::Component* _parent,
                       juce::Button& _button,
                       const juce::String& _text,
                       const std::function<void()>& _callback,
                       juce::Colour _colour,
                       bool _isEnabled)
{
    std::vector<juce::Button*> _dumpVec;
    InitButton(_parent,
               _dumpVec,
               _button,
               _text,
               _callback,
               _colour,
               _isEnabled);
}

void Utils::InitButton(juce::Component* _parent,
                       std::vector<juce::Button*>& _buttonsArray,
                       juce::Button& _button,
                       const juce::String& _text,
                       const std::function<void()>& _callback,
                       juce::Colour _colour,
                       bool _isEnabled)
{
    _button.setButtonText(_text);
    _button.onClick = _callback;
    _button.setColour(juce::TextButton::buttonColourId, _colour);
    _button.setEnabled(_isEnabled);

    _buttonsArray.push_back(&_button);

    _parent->addAndMakeVisible(&_button);
}

void Utils::DrawGrid(juce::Graphics& g, unsigned _width, unsigned _height, unsigned _spacingX, unsigned _spacingY)
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

void Utils::SetComponentOwner(IMyComponent* _owned, IMyComponent* _owner)
{
    _owned->SetOwner(_owner);
}
