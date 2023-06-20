#pragma once

#include <JuceHeader.h>

class MyComponent
{
public:
    virtual ~MyComponent() = default;
    
    virtual void SetOwner(MyComponent* _owner)
    {
        componentOwner = _owner;
    }

protected:
    MyComponent* componentOwner = nullptr;
};
