#pragma once

#define ENIGMA_CLASS(ClassName) \
public: \
    using Self = ClassName; \
    static constexpr const char* GetStaticName() { return #ClassName; } \
private: \
    inline static bool _dummy_name_registered = [](){ \
        Scripting::ScriptRegistry::GetInstance().Register<ClassName>(#ClassName); \
        return true; \
    }(); \
    std::string _enigma_init = (this->scriptName = #ClassName, ""); \
    RTTR_ENABLE(Scripting::IScript) \

#define ENIGMA_PROPERTY() \

#ifdef BUILDING_ENIGMA_ENGINE
    #define ENIGMA_API __declspec(dllexport)
#else
    #define ENIGMA_API __declspec(dllimport)
#endif