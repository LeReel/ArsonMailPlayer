#include "IMyComponent.h"

void IMyComponent::SetOwner(IMyComponent* _owner)
{
    componentOwner = dynamic_cast<juce::Component*>(_owner);
}
