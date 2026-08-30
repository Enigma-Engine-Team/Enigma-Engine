#include "utilities/shortcut.h"

void Shortcut::AddUndoAction(const Action& action)
{
    undoActions.push_back(action);
    redoActions.clear();
}

void Shortcut::AddUndoAction(const std::string& actionName, const std::function<void()>& action)
{
    AddUndoAction({ actionName, action });
}

void Shortcut::AddRedoAction(const Action& action)
{
    redoActions.push_back(action);
}

void Shortcut::AddRedoAction(const std::string& actionName, const std::function<void()>& action)
{
    AddRedoAction({ actionName, action });
}

void Shortcut::Undo()
{
    if (!undoActions.empty())
    {
        Action action = undoActions.back();
        undoActions.pop_back();
        action.action();
        AddRedoAction(action);
    }
}

void Shortcut::Redo()
{
    if (!redoActions.empty())
    {
        Action action = redoActions.back();
        redoActions.pop_back();
        action.action();
        AddUndoAction(action);
    }
}

Shortcut& Shortcut::GetInstance()
{
    static Shortcut instance;
    return instance;
}
