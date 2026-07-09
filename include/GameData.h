#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <unordered_map>
#include <string>

enum class TutorialStep {
    TalkToBoss,
    OpenBoard,
    SolveBoard,
    SolveWhisper,
    TutorialComplete
};

class GameData {
public:
    static std::unordered_map<std::string, bool> flags;
    static void SetFlag(const std::string& flagName, bool value);
    static bool GetFlag(const std::string& flagName);
    static bool playerVictory;
    static TutorialStep GetTutorialStep();
    static bool AdvanceTutorial(TutorialStep expected, TutorialStep next);
    static const char* GetTutorialStepName();

private:
    static TutorialStep tutorialStep;
};

#endif
