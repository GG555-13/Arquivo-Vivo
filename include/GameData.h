#ifndef GAMEDATA_H
#define GAMEDATA_H

enum class TutorialStep {
    TalkToBoss,
    OpenBoard,
    SolveBoard,
    SolveWhisper,
    TutorialComplete
};

class GameData {
public:
    static bool playerVictory;
    static TutorialStep GetTutorialStep();
    static bool AdvanceTutorial(TutorialStep expected, TutorialStep next);
    static const char* GetTutorialStepName();

private:
    static TutorialStep tutorialStep;
};

#endif
