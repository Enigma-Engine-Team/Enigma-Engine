#include "Scoreboard.h"
#include <components/button.h>
#include <components/text.h>

void Scoreboard::Start()
{
    if (restartButton)
        restartButton->GetComponent<Button>()->SetOnClick([this] { RestartGame(); });
}

void Scoreboard::ShowScoreBoard(const std::string& name)
{
    winnerText = winnerGO->GetComponent<Text>();
    winnerText->SetText(name + " win !");
    winnerGO->transform.SetPosition(Vector3D(0.f, 0.f, 0.f));
    restartButton->transform.SetPosition(Vector3D(-300.f, -300.f, 0.f));
    quitButton->transform.SetPosition(Vector3D(300.f, -300.f, 0.f));
}

void Scoreboard::RestartGame()
{
    winnerText->SetText("");
    restartButton->transform.SetPosition(Vector3D(0.f, 1000.f, 0.f));
    playButton->transform.SetPosition(Vector3D(0.f, 150.f, 0.f));
    quitButton->transform.SetPosition(Vector3D(0.f, -150.f, 0.f));
}
