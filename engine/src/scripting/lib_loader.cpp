#include "../../include/scripting/lib_loader.h"

#include <windows.h>
#include <exception>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "debug/log.h"

static HMODULE scriptModule = nullptr;
int LibLoader::reloadIndex = 0;

namespace
{
    constexpr const char* RegisterScriptReflectionFunction = "EnigmaRegisterScriptReflection";
    constexpr const char* UnregisterScriptReflectionFunction = "EnigmaUnregisterScriptReflection";
    using ScriptReflectionFn = void (*)();
}

//Utilities functions
std::string QuotePath(const std::filesystem::path& path)
{
    return "\"" + path.string() + "\"";
}

std::string Trim(const std::string& source)
{
    std::string trimmed(source);
    const size_t first = trimmed.find_first_not_of(" \n\r\t");
    if (first == std::string::npos)
        return "";

    const size_t last = trimmed.find_last_not_of(" \n\r\t");
    trimmed.erase(last + 1);
    trimmed.erase(0, first);
    return trimmed;
}



bool LibLoader::CompileScript()
{
    try
    {
        std::filesystem::create_directories(GetScriptingConfigDirectory());
        std::filesystem::create_directories(GetScriptingBuildDirectory());

        Debug::Log("Compiling scripts...");

        const std::filesystem::path buildDirectory = GetScriptingBuildDirectory();
        bool configureSucceeded = false;

        configureSucceeded = ConfigureScriptBuild(buildDirectory);

        if (!configureSucceeded)
        {
            Debug::LogError("Script configure failed. Check build_log.txt in editor.");
            return false;
        }

        if (!BuildScripts(buildDirectory))
        {
            Debug::LogError("Script compilation failed. Check build_log.txt in editor.");
            return false;
        }

        PromoteBuiltScriptArtifacts();
        Debug::LogSuccess("Script compilation succeeded.");
        return true;
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        Debug::LogError(std::string("Filesystem error while compiling scripts: ") + e.what());
    }
    catch (const std::exception& e)
    {
        Debug::LogError(std::string("Exception while compiling scripts: ") + e.what());
    }

    Debug::LogError("Script compilation failed unexpectedly. Check build_log.txt in editor.");
    return false;
}

bool LibLoader::LoadScriptDLL()
{
    std::filesystem::create_directories(GetScriptingConfigDirectory());
    std::filesystem::create_directories(GetScriptingBuildDirectory());

    if (scriptModule)
    {
        Debug::LogWarning("A script DLL is already loaded. Unloading it before loading a new one.");
        UnloadScriptDLL();
    }

    const std::filesystem::path source = GetUserScriptPath(".dll");
    const std::filesystem::path temp = GetScriptingConfigDirectory() / ("script_reloaded_" + ToString(reloadIndex) + ".dll");

    reloadIndex++;

    try
    {
        if (!std::filesystem::exists(source))
        {
            Debug::LogError("Script DLL not found: " + source.string());
            return false;
        }

        std::filesystem::copy_file(source, temp, std::filesystem::copy_options::overwrite_existing);

        scriptModule = LoadLibraryA(temp.string().c_str());
        if (!scriptModule)
        {
            Debug::LogError("Failed to load script DLL: " + temp.string() + " (Win32 error " + std::to_string(GetLastError()) + ")");
            return false;
        }

        const auto registerReflection = reinterpret_cast<ScriptReflectionFn>(GetProcAddress(scriptModule, RegisterScriptReflectionFunction));
        if (!registerReflection)
        {
            Debug::LogError(std::string("Script DLL does not export ") + RegisterScriptReflectionFunction);
            FreeLibrary(scriptModule);
            scriptModule = nullptr;
            return false;
        }

        registerReflection();
        Debug::LogSuccess("Script DLL loaded successfully.");
        return true;
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        Debug::LogError(std::string("Filesystem error: ") + e.what());
    }
    catch (const std::exception& e)
    {
        Debug::LogError(std::string("Exception: ") + e.what());
    }
    return false;
}

bool LibLoader::UnloadScriptDLL()
{
    if (!scriptModule)
    {
        Debug::Log("No script DLL currently loaded.");
        return true;
    }
    if (const auto unregisterReflection = reinterpret_cast<ScriptReflectionFn>(GetProcAddress(scriptModule, UnregisterScriptReflectionFunction)))
        unregisterReflection();
    if (FreeLibrary(scriptModule) != 0)
    {
        scriptModule = nullptr;
        Debug::LogSuccess("Successfully unloaded script library.");
        return true;
    }
    Debug::LogError("Failed to unload script library.");
    return false;
}

void LibLoader::WriteInJson()
{
    std::filesystem::create_directories(GetScriptingConfigDirectory());
    std::filesystem::create_directories(GetScriptingBuildDirectory());

    nlohmann::json j;
    std::ofstream saveFile(GetJsonPath());
    Debug::Log("Writing properties in JSON file...");
    for (const auto& entry : std::filesystem::directory_iterator(GetScriptingDirectory()))
    {
   if (entry.path().extension() == ".h")
        {
            bool waitingForProperty = false;
            std::string className;

            std::ifstream file(entry.path());
            std::string line;

            while (std::getline(file, line))
            {
                std::string currentLine = Trim(line);

                if (currentLine.starts_with("ENIGMA_CLASS"))
                {
                    currentLine.erase(0, 13);
                    if (currentLine.find(';') != std::string::npos)
                        currentLine.erase(currentLine.find(';'));
                    currentLine.erase(currentLine.find(')'));

                    className = currentLine;
                    j["classes"][className] =
                    {
                        {"path", entry.path().filename().string()},
                        {"properties", nlohmann::json::object()}
                    };
                }
                else if (currentLine == "ENIGMA_PROPERTY()")
                {
                    waitingForProperty = true;
                }
                else if (waitingForProperty)
                {
                    currentLine = currentLine.substr(0, currentLine.find(';'));

                    const size_t spacePos = currentLine.find(' ');
                    const size_t eqPos = currentLine.find('=');

                    const std::string type = Trim(currentLine.substr(0, spacePos));
                    const std::string name = Trim(currentLine.substr(spacePos, eqPos - spacePos));
                    const std::string value = Trim(currentLine.substr(eqPos + 2, currentLine.find(';') - 1));

                    waitingForProperty = false;

                    j["classes"][className]["properties"][name] = {
                        {"type", type},
                        {"value", value}
                    };
                }
            }
        }
    }

    saveFile << j.dump(4);
    Debug::LogSuccess("Properties successfully written in JSON file.");
}

void SetTypeInfo(const std::string& type, std::ofstream& compileFile)
{
    if (type == "float")
        compileFile << "DRAG_FLOAT, 0.f, 1000000.f";
    else if (type == "int")
        compileFile << "INT, 0, 1000000";
    else if (type == "bool")
        compileFile << "BOOL";
    else if (type == "Vector3D")
        compileFile << "VEC3";
    else if (type == "GameObject" || type == "GameObject*")
        compileFile << "GAMEOBJECT_INPUT";
}

void LibLoader::CreateCppFromJson()
{
    std::filesystem::create_directories(GetScriptingConfigDirectory());
    std::filesystem::create_directories(GetScriptingBuildDirectory());

    std::ifstream loadFile(GetJsonPath());
    std::ofstream compileFile(GetGeneratedCppPath());

    nlohmann::json j;
    loadFile >> j;

    compileFile << "#include \"engine/engine.h\"\n";
    compileFile << "#include <rttr/detail/registration/registration_manager.h>\n";
    for (auto& [className, classData] : j["classes"].items())
    {
        const std::string path = classData["path"];
        compileFile << "#include \"" << "../" << std::filesystem::path(path).filename().generic_string() << "\"\n";
    }
    compileFile << "\nextern \"C\" __declspec(dllexport) void EnigmaRegisterScriptReflection()\n{";

    for (auto& [className, classData] : j["classes"].items())
    {
        compileFile << "\n    rttr::registration::class_<" << className << ">(\"" << className << "\")";
        compileFile << "\n    .constructor<>()(rttr::policy::ctor::as_raw_ptr)";
        for (auto& [propName, propData] : classData["properties"].items())
        {
            const std::string type = propData["type"];
            compileFile <<"\n    .property(\"" << propName << "\", &" << className << "::" << propName << ")";
            compileFile <<"\n        (rttr::metadata(UI_DATA_METADATA_KEY, UIData(EUIType::";
            SetTypeInfo(type, compileFile);
            compileFile << ")))";
        }

        compileFile << ";\n\n";

        compileFile << "    Engine::componentsMap.insert_or_assign(rttr::type::get<" << className << ">(),"
            " Engine::ComponentData { "
                "\"" << className << "\", "
                "\"cpp_icon.png\", "
                "[&] "
                "{ "
                    "Engine::GetRenderer()->GetSelectedGameObject()->AddComponent<" << className << ">();"
                " } "
            " } "
        ");\n";
        compileFile << "    Engine::CachePropertyData(rttr::type::get<" << className << ">());\n";
    }
    compileFile << "}\n";
    compileFile << "\nextern \"C\" __declspec(dllexport) void EnigmaUnregisterScriptReflection()\n";
    compileFile << "{\n";
    compileFile << "    rttr::detail::get_registration_manager().unregister();\n";
    compileFile << "}\n";
    Debug::LogSuccess("CPP file successfully generated from JSON file.");
}


#pragma region PathGetter
std::filesystem::path LibLoader::GetEditorRootDirectory()
{
    char buffer[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (length == 0)
        return std::filesystem::current_path();

    return std::filesystem::path(buffer).parent_path();
}

std::filesystem::path LibLoader::GetScriptingDirectory()
{
    return GetEditorRootDirectory() / "scripting";
}

std::filesystem::path LibLoader::GetScriptingConfigDirectory()
{
    return GetScriptingDirectory() / "config";
}

std::filesystem::path LibLoader::GetScriptingBuildDirectory()
{
    return GetScriptingDirectory() / "build";
}

std::filesystem::path LibLoader::GetUserScriptPath(const std::string& extension)
{
    std::string dllName = "UserScripts_" + std::to_string(reloadIndex);
    return GetScriptingConfigDirectory() / (dllName + extension);
}

std::filesystem::path LibLoader::GetJsonPath()
{
    return GetScriptingConfigDirectory() / "properties.json";
}

std::filesystem::path LibLoader::GetGeneratedCppPath()
{
    return GetScriptingConfigDirectory() / "generated_reflection.cpp";
}

std::filesystem::path LibLoader::GetBuildLogPath()
{
    return GetEditorRootDirectory() / "build_log.txt";
}
#pragma endregion

//Artifact resolution for devs lauchs
std::filesystem::path LibLoader::ResolveLatestScriptArtifact(const char* extension)
{
    const std::filesystem::path preferred = GetUserScriptPath(extension);

    std::filesystem::path latestArtifact;
    std::filesystem::file_time_type latestWriteTime{};

    for (const auto& entry : std::filesystem::recursive_directory_iterator(GetScriptingConfigDirectory()))
    {
        if (!entry.is_regular_file() || entry.path().extension() != extension)
            continue;

        const std::string stem = entry.path().stem().string();
        if (stem != "UserScripts" && stem != "UserScriptsd")
            continue;

        const auto writeTime = std::filesystem::last_write_time(entry.path());
        if (latestArtifact.empty() || writeTime > latestWriteTime)
        {
            latestArtifact = entry.path();
            latestWriteTime = writeTime;
        }
    }

    return latestArtifact;
}

void LibLoader::PromoteBuiltScriptArtifacts()
{
    const std::filesystem::path preferredDll = GetUserScriptPath(".dll");
    const std::filesystem::path preferredPdb = GetUserScriptPath(".pdb");

    const std::filesystem::path builtDll = ResolveLatestScriptArtifact(".dll");
    const std::filesystem::path builtPdb = ResolveLatestScriptArtifact(".pdb");

    if (!builtDll.empty() && builtDll != preferredDll)
        std::filesystem::copy_file(builtDll, preferredDll, std::filesystem::copy_options::overwrite_existing);

    if (!builtPdb.empty() && builtPdb != preferredPdb)
        std::filesystem::copy_file(builtPdb, preferredPdb, std::filesystem::copy_options::overwrite_existing);
}


//Log
bool LibLoader::BuildLogContains(const std::string_view needle)
{
    std::ifstream logFile(GetBuildLogPath());
    if (!logFile.is_open())
        return false;

    std::string line;
    while (std::getline(logFile, line))
    {
        if (line.find(needle) != std::string::npos)
            return true;
    }

    return false;
}

bool LibLoader::BuildLogIndicatesCacheMismatch()
{
    return BuildLogContains("does not match the source used to generate cache") ||
           BuildLogContains("current CMakeCache.txt directory") ||
           BuildLogContains("The source") ||
           BuildLogContains("different than the directory");
}


//Cmd
std::string LibLoader::GetCurrentConfig()
{
#ifdef _DEBUG
    return "Debug";
#elif defined(NDEBUG)
    return "Release";
#else
    return "Debug";
#endif
}

bool LibLoader::RunScriptCommand(const std::string& command)
{
    const std::string vsCommand = "\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat\"";
    const std::string finalCommand = "call " + vsCommand + " && " + command + " > " + QuotePath(GetBuildLogPath()) + " 2>&1";

    return std::system(finalCommand.c_str()) == 0;
}

bool LibLoader::ConfigureScriptBuild(const std::filesystem::path& buildDirectory)
{
    std::filesystem::create_directories(buildDirectory);

    std::string cmakeCmd = "cmake --fresh "
                           "-G \"Visual Studio 17 2022\" -A x64 "
                           "-DHOTRELOAD_ID=" + std::to_string(reloadIndex) + " "
                           "-S " + QuotePath(GetScriptingDirectory()) +
                           " -B " + QuotePath(buildDirectory);

    return RunScriptCommand(cmakeCmd);
}

bool LibLoader::BuildScripts(const std::filesystem::path& buildDirectory)
{
    const std::string cmakeCmd =
        "cmake --build " + QuotePath(buildDirectory) +
        " --config " + GetCurrentConfig();

    return RunScriptCommand(cmakeCmd);
}
