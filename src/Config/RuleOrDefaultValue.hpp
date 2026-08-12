#pragma once
#include "ConfigManager.h"
#include "Context/ClientContext.h"
#include <hyprland/src/config/ConfigValue.hpp>

template <typename T>
class RuleOrDefaultValue
{
public:
    RuleOrDefaultValue(const ConfigManager::ConfigType config_type, const T&& default_value)
        : m_ConfigType(config_type)
        , m_DefaultValue(default_value)
    {
    }

    T GetValue(WP<ClientContext> client_context)
    {
        if (!client_context)
            return m_DefaultValue;

        if (!client_context->RuleConfigValueCache)
            return m_DefaultValue;

        auto& value = client_context->RuleConfigValueCache->at(m_ConfigType);
        if (!std::holds_alternative<std::monostate>(value))
            return std::get<T>(value);

        return m_DefaultValue;
    }

    T GetValue(client_t client)
    {
        if (!client)
            return m_DefaultValue;

        auto client_context = ClientContext::GetContext(client);
        if (!client_context)
            return m_DefaultValue;

        if (!client_context->RuleConfigValueCache)
            return m_DefaultValue;

        auto& value = client_context->RuleConfigValueCache->at(m_ConfigType);
        if (!std::holds_alternative<std::monostate>(value))
            return std::get<T>(value);

        return m_DefaultValue;
    }

    T operator[](WP<ClientContext> client_context)
    {
        return GetValue(client_context);
    }

    T operator[](client_t client)
    {
        return GetValue(client);
    }

private:
    ConfigManager::ConfigType m_ConfigType;
    T                         m_DefaultValue;
};