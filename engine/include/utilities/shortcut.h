#pragma once
#include <functional>
#include <string>
#include <vector>

#include "macro.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
#include "utilities/utility.h"

enum ActionContext
{
    UNDO,
    REDO
};

struct Action
{
    Action();
    Action(const std::string& actionName, const std::function<void()>& action, const UUID& goID, const nlohmann::json& snapshot = {});
    std::string actionName;
    std::function<void()> action;
    nlohmann::json snapshot;
    UUID goID;
};

class ENIGMA_API Shortcut
{
public:
    Shortcut() = default;
    ~Shortcut() = default;

    static void AddAction(const Action& action);
    static void Undo();
    static void Redo();
    static void Clear();

private:
    static std::vector<Action> undoActions;
    static std::vector<Action> redoActions;
    static ActionContext context;
};
