#include "serialization/serializer.h"
#include "scenes/scene_graph.h"
#include "components/mesh_renderer.h"
#include "scripting/iscript.h"
#include <filesystem>
#include <fstream>

Serializer& Serializer::GetInstance()
{
    static Serializer serializer;
    return serializer;
}

void Serializer::SerializeScene(Scene* scene)
{
    nlohmann::json obj = nlohmann::json::object();
    obj["gameObjects"] = nlohmann::json::array();

    DeleteFileInfos("assets\\scripts", scene->GetName() + ".script");
    for (GameObject* go : scene->GetGameObject(0)->GetChildren())
    {
        nlohmann::json gameObjectJson;
        RecursiveSerialize(go, gameObjectJson);
        obj["gameObjects"].push_back(gameObjectJson);
    }
    obj["skyboxPath"] = scene->GetSkyboxPath();

    EngineCamera* engineCam = scene->GetEngineCam();
    RecursiveSerialize(engineCam, obj);

    WriteToFile("assets\\scripts", scene->GetName() + ".script", script, false);
    WriteToFile("assets\\scenes", scene->GetName() + ".scene", obj);
}

void Serializer::DeserializeScene(Scene* scene)
{
    nlohmann::json data;
    if (!ReadFromFile("assets\\scenes\\" + scene->GetName() + ".scene", data))
        return;

    DeleteFileInfos("assets\\scripts", scene->GetName() + ".script");

    auto& gameObjects = data["gameObjects"];
    currentSceneContext = scene;

    for (const auto& wrapper : gameObjects)
    {
        nlohmann::json wrapperJson = wrapper;
        RecursiveDeserialize(scene, wrapperJson, scene->GetGameObject(0));
    }

    scene->SetSkyboxPath(data["skyboxPath"]);

    EngineCamera* engineCam = scene->GetEngineCam();
    WriteToFile("assets\\scripts", scene->GetName() + ".script", script, false);
    RecursiveSetProperties(engineCam, data["EngineCamera"]);
	AttachedScript(scene);

    scene->SetLoaded(true);
}

void Serializer::SaveBeforePlay(Scene* scene)
{
    nlohmann::json obj = nlohmann::json::object();
    obj["gameObjects"] = nlohmann::json::array();

    DeleteFileInfos("assets\\scripts", scene->GetName() + ".script");
    for (GameObject* go : scene->GetGameObject(0)->GetChildren())
    {
        nlohmann::json gameObjectJson;
        RecursiveSerialize(go, gameObjectJson);
        obj["gameObjects"].push_back(gameObjectJson);
    }

    WriteToFile("assets\\scripts", scene->GetName() + ".script", script, false);
    WriteToFile("assets\\scenes", scene->GetName() + "_temp.scene", obj);
}

void Serializer::ResetAfterPlay(Scene* scene)
{
    scene->Clean();
    nlohmann::json data;
    std::string tempScenePath = "assets\\scenes\\" + scene->GetName() + "_temp.scene";
    ReadFromFile(tempScenePath, data);
    auto& gameObjects = data["gameObjects"];
    currentSceneContext = scene;

    for (const auto& wrapper : gameObjects)
    {
        nlohmann::json wrapperJson = wrapper;
        RecursiveDeserialize(scene, wrapperJson, scene->GetGameObject(0));
    }

	AttachedScript(scene);
    std::filesystem::remove(tempScenePath);
}

void Serializer::WritePrefab(GameObject* gameObject)
{
    gameObject->SetInstancePrefab(gameObject->GetName());

    nlohmann::json obj = nlohmann::json::object();
    RecursiveSerialize(gameObject, obj);

    WriteToFile("assets\\prefabs", gameObject->GetName() + ".prefab", obj);
}

void Serializer::ReadPrefab(Scene* scene, std::string name)
{
    nlohmann::json data;
    ReadFromFile("assets\\prefabs\\" + name + ".prefab", data);
    RecursiveDeserialize(scene, data, scene->GetGameObject(0));
}

void Serializer::SerializeMaterial(Material* mat)
{
    nlohmann::json obj = nlohmann::json::object();
    RecursiveSerialize(mat, obj);

    WriteToFile("assets\\materials", mat->name + ".mat", obj);
}

void Serializer::DeserializeMaterial(Material* mat)
{
    nlohmann::json data;
    ReadFromFile("assets\\materials\\" + mat->name + ".mat", data);
    RecursiveSetProperties(mat, data["Material"]);
}

void Serializer::RecursiveSerialize(const rttr::instance& instance, nlohmann::json& j)
{
    rttr::instance obj = instance.get_type().is_wrapper() ? instance.get_wrapped_instance() : instance;
    if (!obj.is_valid()) return;

    rttr::type t = obj.get_derived_type();
    if (t.is_pointer()) t = t.get_raw_type();

    std::string objName = t.get_name().to_string();
    j[objName] = nlohmann::json::object();

    for (auto& prop : t.get_properties())
    {
        rttr::variant val = prop.get_value(obj);
        rttr::type val_type = val.get_type();
        rttr::type original_val_type = val_type;
        if (val_type.is_pointer()) val_type = val_type.get_raw_type();

        std::string prop_name = prop.get_name().to_string();
        if (original_val_type == rttr::type::get<GameObject*>())
        {
            GameObject* referencedGameObject = val.get_value<GameObject*>();
            j[objName][prop_name] = referencedGameObject ? referencedGameObject->GetName() : "";
        }
        else if (val_type.is_sequential_container())
        {
            auto view = val.create_sequential_view();
            j[objName][prop_name] = nlohmann::json::array();

            for (auto& v : view)
            {
                nlohmann::json item;
                rttr::variant wrapped_val = v.extract_wrapped_value();

                if (wrapped_val.can_convert<Scripting::IScript*>())
                {
                    Scripting::IScript* scriptComponent = wrapped_val.convert<Scripting::IScript*>();
                    SetScriptElement(wrapped_val, scriptComponent->gameObject->GetName(), scriptComponent->gameObject->GetUUID().ToString());
                }

                RecursiveSerialize(wrapped_val, item);
                j[objName][prop_name].push_back(item);
            }
        }
        else if (val_type.is_class() && !val_type.is_arithmetic())
        {
            if (val_type == rttr::type::get<std::string>())
            {
                j[objName][prop_name] = val.get_value<std::string>();
            }
            else if (val_type == rttr::type::get<UUID>())
            {
                j[objName][prop_name] = val.get_value<UUID>().ToString();
            }
            else
            {
                RecursiveSerialize(val, j[objName][prop_name]);
            }
        }
        else
        {
            if (val.is_type<int>())        j[objName][prop_name] = val.get_value<int>();
            else if (val.is_type<float>()) j[objName][prop_name] = val.get_value<float>();
            else if (val.is_type<bool>())  j[objName][prop_name] = val.get_value<bool>();
            else                           j[objName][prop_name] = val.to_string();
        }
    }
}

void Serializer::RecursiveDeserialize(Scene* scene, const nlohmann::json& j, GameObject* go)
{
    nlohmann::json goData = j["GameObject"];
    std::string goName = goData["name"];
    std::string goUUID = goData["uuid"];

    GameObject* gameObject = scene->AddGameObject(UUID::ToUUID(goUUID));
    gameObject->SetName(goName);
    go->AddChild(gameObject);

    const auto& transformJSON = goData["transform"];
    Transform* transform = &gameObject->transform;
    RecursiveSetProperties(transform, transformJSON["Transform"]);

    for (auto& child : goData["children"])
    {
        nlohmann::json childJSON = child;
        RecursiveDeserialize(scene, childJSON, gameObject);
    }

    for (auto& compWrapper : goData["components"])
    {
        for (auto it = compWrapper.begin(); it != compWrapper.end(); ++it)
        {
            const std::string& compTypeName = it.key();
            rttr::type t = rttr::type::get_by_name(compTypeName);
            rttr::type scriptBase = rttr::type::get<Scripting::IScript>();

            if (t.is_valid() && t.is_derived_from(scriptBase))
                continue;

            if (t.is_valid())
            {
                IComponent* comp = gameObject->GetComponentType(t);

                if (comp == nullptr)
                {
                    comp = gameObject->AddComponentType(t);
                }

                RecursiveSetProperties(comp, it.value());

                comp->SetComponent();
            }
            else
                SetScriptElement(compWrapper, compTypeName, goName, goUUID);
        }
    }
}

void Serializer::SetScriptElement(nlohmann::json& j, const std::string& scriptName, const std::string& goName, const std::string& uuid)
{
    nlohmann::json scriptData;
    scriptData["scriptName"] = scriptName;

    if (j.contains(scriptName))
        scriptData["variables"] = j.at(scriptName);
    else
        scriptData["variables"] = nlohmann::json::object();

    PushScriptElement(goName, uuid, scriptData);
}

void Serializer::SetScriptElement(const rttr::instance& instance, const std::string& goName, const std::string& uuid)
{
    rttr::instance inst = instance.get_type().is_wrapper() ? instance.get_wrapped_instance() : instance;
    if (!inst.is_valid()) return;

    rttr::type t = inst.get_derived_type();
    if (t.is_pointer()) t = t.get_raw_type();

    std::string scriptName = t.get_name().to_string();

    nlohmann::json serializedScript;
    RecursiveSerialize(inst, serializedScript);

    nlohmann::json scriptData;
    scriptData["scriptName"] = scriptName;
    scriptData["variables"] = serializedScript.value(scriptName, nlohmann::json::object());
    PushScriptElement(goName, uuid, scriptData);
}

void Serializer::PushScriptElement(const std::string& goName, const std::string& uuid, const nlohmann::json& scriptData)
{
    nlohmann::json& goData = script[goName];
    goData["uuid"] = uuid;

    if (!goData.contains("scripts"))
        goData["scripts"] = nlohmann::json::array();

    goData["scripts"].push_back(scriptData);
}

void Serializer::AttachedScript(Scene* scene)
{
    nlohmann::json obj;
    if (!ReadFromFile("assets\\scripts\\" + scene->GetName() + ".script", obj))
        return;

    for (auto& [goName, goData] : obj.items())
    {
        UUID uuid = UUID::ToUUID(goData["uuid"].get<std::string>());

        GameObject* go = scene->GetGameObject(uuid);
        if (!go)
            go = scene->GetGameObject(goName);
        if (!go)
            continue;

        const auto& scriptData = goData["scripts"];

        if (scriptData.is_array())
        {
            for (const nlohmann::json& scriptEntry : scriptData)
                AttachScriptData(go, scriptEntry, obj);
        }
        else
        {
            AttachScriptData(go, scriptData, obj);
        }
    }
}

void Serializer::AttachScriptData(GameObject* go, const nlohmann::json& scriptData, const nlohmann::json& scriptRoot)
{
    if (!scriptData.is_object() || !scriptData.contains("scriptName"))
        return;

    std::string scriptName = scriptData["scriptName"];

    Scripting::IScript* scriptInstance = go->AddScript(scriptName);
    if (!scriptInstance)
        return;

    nlohmann::json variables = scriptData.contains("variables") ? scriptData.at("variables") : nlohmann::json::object();
    if (variables.empty() && scriptRoot.contains(scriptName) && scriptRoot.at(scriptName).is_object() && scriptRoot.at(scriptName).contains("variables"))
        variables = scriptRoot.at(scriptName).at("variables");

    RecursiveSetProperties(scriptInstance, variables);
}

void Serializer::RecursiveSetProperties(rttr::instance obj, const nlohmann::json& j)
{
    rttr::type t = obj.get_derived_type();
    if (t.is_pointer()) t = t.get_raw_type();

    for (auto prop : t.get_properties())
    {
        const std::string prop_name = prop.get_name().to_string();

        if (j.contains(prop_name))
        {
            const nlohmann::json& val = j.at(prop_name);
            rttr::type prop_type = prop.get_type();
            rttr::type original_prop_type = prop_type;
            if (prop_type.is_pointer()) prop_type = prop_type.get_raw_type();

            if (original_prop_type == rttr::type::get<GameObject*>())
            {
                GameObject* referencedGameObject = nullptr;
                if (val.is_object() && val.contains("GameObject") && val["GameObject"].contains("uuid"))
                {
                    UUID uuid = UUID::ToUUID(val["GameObject"]["uuid"].get<std::string>());
                    if (currentSceneContext)
                        referencedGameObject = currentSceneContext->GetGameObject(uuid);
                }

                //A retirer après tests
                if (referencedGameObject == nullptr)
                {
                    std::string gameObjectName;
                    if (val.is_object() && val.contains("GameObject") && val["GameObject"].contains("name"))
                    {
                        gameObjectName = val["GameObject"]["name"].get<std::string>();
                    }

                    if (currentSceneContext && !gameObjectName.empty())
                        referencedGameObject = currentSceneContext->GetGameObject(gameObjectName);
                }

                prop.set_value(obj, referencedGameObject);
            }
            else if (val.is_number_float())
            {
                prop.set_value(obj, val.get<float>());
            }
            else if (val.is_number_integer())
            {
                prop.set_value(obj, val.get<int>());
            }
            else if (val.is_boolean())
            {
                prop.set_value(obj, val.get<bool>());
            }
            else if (val.is_string())
            {
                prop.set_value(obj, val.get<std::string>());
            }
            else if (val.is_object())
            {
                rttr::variant subObj = prop.get_value(obj);
                RecursiveSetProperties(subObj, val.at(prop_type.get_name().to_string()));
                prop.set_value(obj, subObj);
            }
        }
    }
}

bool Serializer::DeleteFileInfos(const std::string& folder, const std::string& file)
{
    script = nlohmann::json::object();

    std::string path = folder + "\\" + file;
    if (!std::filesystem::exists(path))
    {
        return false;
    }

    std::ofstream filestream(path, std::ios::out | std::ios::trunc);
    return filestream.is_open();
}

bool Serializer::ReadFromFile(std::string path, nlohmann::json& datas)
{
    if (!std::filesystem::exists(path))
    {
        return false;
    }

    std::ifstream f(path);

    if (f.is_open())
    {
        datas = nlohmann::json::parse(f);
        return true;
    }
    return false;
}

bool Serializer::WriteToFile(std::string folder, std::string file, nlohmann::json& datas, bool override)
{
    if (!std::filesystem::exists(folder))
    {
        std::filesystem::create_directories(folder);
    }

    std::ofstream filestream;
    if (override)
    {
        filestream.open(folder + "\\" + file, std::ios::out | std::ios::trunc);
    }
    else
    {
        filestream.open(folder + "\\" + file, std::ios::app);
    }

    if (!filestream.is_open())
    {
        return false;
    }

    filestream << std::setw(4) << datas;
    return true;
}
