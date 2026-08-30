#pragma once
#include <functional>
#include <string>

#include "macro.h"

struct Action
{
    std::string actionName;
    std::function<void()> action;
};

class ENIGMA_API Shortcut
{
public:
    Shortcut() = default;
    ~Shortcut() = default;

    void AddUndoAction(const Action& action);
    void AddUndoAction(const std::string& actionName, const std::function<void()>& action);
    void AddRedoAction(const Action& action);
    void AddRedoAction(const std::string& actionName, const std::function<void()>& action);
    void Undo();
    void Redo();

    static Shortcut& GetInstance();

private:
    std::vector<Action> undoActions;
    std::vector<Action> redoActions;
};
