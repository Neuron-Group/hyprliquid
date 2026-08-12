#include "BackgroundSharePassElement.h"
#include "Context/MonitorContext.h"
#include "Utils/BackgroundManager.h"
#include <hyprland/src/render/GLRenderer.hpp>

std::vector<UP<IPassElement>> CBackgroundSharePassElement::draw()
{
    auto& m_renderData = g_pHyprRenderer->m_renderData;
    BackgroundManager::ShareBackground(m_renderData.pMonitor.lock(), m_renderData.currentFB->getTexture());

    return std::vector<UP<IPassElement>>();
}

bool CBackgroundSharePassElement::needsLiveBlur()
{
    return false;
}

bool CBackgroundSharePassElement::needsPrecomputeBlur()
{
    return false;
}

const char *CBackgroundSharePassElement::passName()
{
    return "CBackgroundSharePassElement";
}

ePassElementType CBackgroundSharePassElement::type()
{
    return EK_CUSTOM;
};
