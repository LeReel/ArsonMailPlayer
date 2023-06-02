#include "Utils.h"

void Utils::InitButton(juce::Component* _parent,
                       juce::Button& _button,
                       juce::String _text,
                       std::function<void()> _callback,
                       juce::Colour _colour,
                       bool _isEnabled)
{
    _parent->addAndMakeVisible(&_button);
    _button.setButtonText(_text);
    _button.onClick = _callback;
    _button.setColour(juce::TextButton::buttonColourId, _colour);
    _button.setEnabled(_isEnabled);
}

void Utils::SetComponentOwner(IMyComponent* _owned, IMyComponent* _owner)
{
    _owned->SetOwner(_owner);
}
