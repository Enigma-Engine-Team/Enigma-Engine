#pragma once
#include <filesystem>

#include "../utilities/macro.h"

class ENIGMA_API LibLoader
{
public:
    LibLoader() = delete;
    static bool CompileScript();
    static bool LoadScriptDLL();
    static bool UnloadScriptDLL();

    static void WriteInJson();
    static void CreateCppFromJson();

    static int reloadIndex;
private:
    //Path Getter
    static std::filesystem::path GetEditorRootDirectory();
    static std::filesystem::path GetScriptingDirectory();
    static std::filesystem::path GetScriptingConfigDirectory();
    static std::filesystem::path GetScriptingBuildDirectory();
    static std::filesystem::path GetUserScriptPath(const std::string& extension);
    static std::filesystem::path GetJsonPath();
    static std::filesystem::path GetGeneratedCppPath();
    static std::filesystem::path GetBuildLogPath();

    //Artifact Resolution for Devs
    static std::filesystem::path ResolveLatestScriptArtifact(const char* extension);
    static void PromoteBuiltScriptArtifacts();

    //Log error
    static bool BuildLogContains(const std::string_view needle);
    static bool BuildLogIndicatesCacheMismatch();

    //Build Command
    static std::string GetCurrentConfig();
    static bool RunScriptCommand(const std::string& command);
    static bool ConfigureScriptBuild(const std::filesystem::path& buildDirectory);
    static bool BuildScripts(const std::filesystem::path& buildDirectory);
};
