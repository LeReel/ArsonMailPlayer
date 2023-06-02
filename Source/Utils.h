#pragma once

#include "IMyComponent.h"

namespace Utils
{
    void InitButton(juce::Component* _parent,
                    juce::Button& _button,
                    juce::String _text,
                    std::function<void()> _callback,
                    juce::Colour _colour,
                    bool _isEnabled);

    void SetComponentOwner(IMyComponent* _owned, IMyComponent* _owner);
}
