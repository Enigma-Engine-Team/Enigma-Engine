#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include "components/icomponent.h"
#include "emath/vector.h"
#include "../utilities/macro.h"

namespace Scripting
{
    class IScript;

    using ScriptFactory = std::function<IScript*()>;
    using ScriptPropertyValue = std::variant<float, int, bool, Math::Vector3D, Math::Vector4D>;

    struct ScriptPropertyDescriptor
    {
        std::string name;
        UIData uiData;
        std::function<ScriptPropertyValue(IScript*)> getter;
        std::function<void(IScript*, const ScriptPropertyValue&)> setter;
    };

    class ENIGMA_API ScriptRegistry
    {
    public:
        static ScriptRegistry& GetInstance();

        template<typename T>
        void Register(const std::string& name)
        {
            factories[name] = []() -> IScript* { return new T(); };
        }

        int GetNumberOfRegisteredScripts() const { return static_cast<int>(factories.size()); }
        std::vector<std::string> GetRegisteredNames() const;
        IScript* Create(const std::string& name) const;

        void DeleteClassRegistry();

    private:
        std::unordered_map<std::string, ScriptFactory> factories;
    };
}