#include "utilities/shortcut.h"

std::vector<Action> Shortcut::undoActions;
std::vector<Action> Shortcut::redoActions;
ActionContext Shortcut::context = UNDO;

Action::Action()
{
    this->goID = UUID(0, 0);
}

Action::Action(const std::string& actionName, const std::function<void()>& action, const UUID& goID, const nlohmann::json& snapshot)
{
    this->actionName = actionName;
    this->action = action;
    this->snapshot = snapshot;
    this->goID = goID;
}

void Shortcut::AddAction(const Action& action)
{
    if (context == UNDO)
        undoActions.push_back(action);
    else if (context == REDO)
        redoActions.push_back(action);
}

void Shortcut::Undo()
{
    if (!undoActions.empty())
    {
        context = REDO;
        Action action = undoActions.back();
        undoActions.pop_back();
        action.action();
        context = UNDO;
    }
}

void Shortcut::Redo()
{
    if (!redoActions.empty())
    {
        Action action = redoActions.back();
        redoActions.pop_back();
        action.action();
    }
}

void Shortcut::Clear()
{
    undoActions.clear();
    redoActions.clear();
}
