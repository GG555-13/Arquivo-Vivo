#include "GameData.h"

bool GameData::playerVictory = false;
TutorialStep GameData::tutorialStep = TutorialStep::TalkToBoss;

TutorialStep GameData::GetTutorialStep() {
    return tutorialStep;
}

bool GameData::AdvanceTutorial(TutorialStep expected, TutorialStep next) {
    if (tutorialStep != expected) {
        return false;
    }
    tutorialStep = next;
    return true;
}

const char* GameData::GetTutorialStepName() {
    switch (tutorialStep) {
        case TutorialStep::TalkToBoss: return "TalkToBoss";
        case TutorialStep::OpenBoard: return "OpenBoard";
        case TutorialStep::SolveBoard: return "SolveBoard";
        case TutorialStep::SolveWhisper: return "SolveWhisper";
        case TutorialStep::TutorialComplete: return "TutorialComplete";
    }
    return "Unknown";
}
