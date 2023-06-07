#pragma once

#include <JuceHeader.h>

class IMyComponent
{
public:
    virtual ~IMyComponent() = default;
    
    virtual void SetOwner(IMyComponent* _owner);

protected:
    IMyComponent* componentOwner = nullptr;
};
