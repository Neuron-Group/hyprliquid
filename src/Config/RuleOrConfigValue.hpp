#pragma once
#include "ConfigManager.h"
#include "Context/ClientContext.h"
#include <hyprland/src/config/ConfigValue.hpp>
#include <hyprland/src/config/values/types/ColorValue.hpp>

template <typename T>
class RuleOrConfigValueBase
{
public:
    RuleOrConfigValueBase(const ConfigManager::ConfigType config_type)
        : m_ConfigType{config_type}
    {
    }

    T GetValue(WP<ClientContext> client_context)
    {
        if (!client_context)
            return GetConfigValue();

        if (!client_context->RuleConfigValueCache)
            return GetConfigValue();

        auto& value = client_context->RuleConfigValueCache->at(m_ConfigType);
        if (!std::holds_alternative<std::monostate>(value))
            return std::get<T>(value);

        return GetConfigValue();
    }

    T GetValue(client_t client)
    {
        if (!client)
            return GetConfigValue();

        auto client_context = ClientContext::GetContext(client);
        if (!client_context)
            return GetConfigValue();

        if (!client_context->RuleConfigValueCache)
            return GetConfigValue();

        auto& value = client_context->RuleConfigValueCache->at(m_ConfigType);
        if (!std::holds_alternative<std::monostate>(value))
            return std::get<T>(value);

        return GetConfigValue();
    }

    T operator[](WP<ClientContext> client_context)
    {
        return GetValue(client_context);
    }

    T operator[](client_t client)
    {
        return GetValue(client);
    }

protected:
    virtual T GetConfigValue() = 0;

private:
    ConfigManager::ConfigType m_ConfigType;
};

template <typename T>
class RuleOrConfigValue : public RuleOrConfigValueBase<T>
{
public:
    RuleOrConfigValue(const ConfigManager::ConfigType config_type)
        : RuleOrConfigValueBase<T>(config_type)
        , m_ConfigValue(makeUnique<CConfigValue<T>>(std::string(ConfigManager::ConfigNames[config_type])))
    {
    }

private:
    virtual T GetConfigValue() override
    {
        return **m_ConfigValue;
    }

private:
    UP<CConfigValue<T>> m_ConfigValue;
};


template <>
class RuleOrConfigValue<CHyprColor> : public RuleOrConfigValueBase<CHyprColor>
{
public:
    RuleOrConfigValue(const ConfigManager::ConfigType config_type)
        : RuleOrConfigValueBase<CHyprColor>(config_type)
        , m_ConfigValue(makeUnique<CConfigValue<Config::INTEGER>>(std::string(ConfigManager::ConfigNames[config_type])))
    {
    }

private:
    virtual CHyprColor GetConfigValue() override
    {
        return CHyprColor(**m_ConfigValue);
    }

private:
    UP<CConfigValue<Config::INTEGER>> m_ConfigValue;
};
