#include "IMyComponent.h"

void IMyComponent::SetOwner(IMyComponent* _owner)
{
    componentOwner = _owner;
}
