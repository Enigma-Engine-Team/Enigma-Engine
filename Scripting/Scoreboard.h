#pragma once
#include <components/text.h>
#include <scripting/script_base.h>

class Scoreboard : public Script<Scoreboard>
{
    ENIGMA_CLASS(Scoreboard)
public:
    void Start() override;
    void ShowScoreBoard(const std::string& name);
    ENIGMA_PROPERTY()
    GameObject* winnerGO;    
    ENIGMA_PROPERTY()
    GameObject* playButton;
    ENIGMA_PROPERTY()
    GameObject* restartButton;
    ENIGMA_PROPERTY()
    GameObject* quitButton;
    
private:
    void RestartGame();
    
    Text* winnerText = nullptr;
};
