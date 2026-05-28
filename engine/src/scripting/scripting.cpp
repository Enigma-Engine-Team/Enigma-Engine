#include "scripting/scripting.h"

#include <ranges>

Scripting::ScriptRegistry& Scripting::ScriptRegistry::GetInstance()
{
    static ScriptRegistry registry;
    return registry;
}

std::vector<std::string> Scripting::ScriptRegistry::GetRegisteredNames() const
{
    std::vector<std::string> names;
    for (auto const& [name, factory] : factories) {
        names.push_back(name);
    }
    return names;
}

Scripting::IScript * Scripting::ScriptRegistry::Create(const std::string &name) const
{
    auto it = factories.find(name);
    return it != factories.end() ? it->second() : nullptr;
}

void Scripting::ScriptRegistry::DeleteClassRegistry()
{
    for (auto &factory: factories)
        factory.second = nullptr;
     factories.clear();
}
