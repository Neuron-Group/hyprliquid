#pragma once
#include <hyprland/src/render/pass/PassElement.hpp>

class CBackgroundSharePassElement : public IPassElement
{
public:
    virtual std::vector<UP<IPassElement>> draw();
    virtual bool                          needsLiveBlur();
    virtual bool                          needsPrecomputeBlur();
    virtual const char*                   passName();
    virtual ePassElementType              type();
};