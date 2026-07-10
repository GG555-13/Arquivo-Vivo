#include "GameData.h"

std::unordered_map<std::string, bool> GameData::flags;

void GameData::SetFlag(const std::string& flagName, bool value) {
    flags[flagName] = value;
}

bool GameData::GetFlag(const std::string& flagName) {
    if (flags.find(flagName) != flags.end()) {
        return flags[flagName];
    }
    return false; 
}

bool GameData::playerVictory = false;
StoryStep GameData::storyStep = StoryStep::TutorialTalkToBoss;

StoryStep GameData::GetStoryStep() {
    return storyStep;
}

bool GameData::AdvanceStory(StoryStep expected, StoryStep next) {
    if (storyStep != expected) {
        return false;
    }
    storyStep = next;
    return true;
}

const char* GameData::GetStoryStepName() {
    switch (storyStep) {
        case StoryStep::TutorialTalkToBoss: return "TutorialTalkToBoss";
        case StoryStep::TutorialOpenBoard: return "TutorialOpenBoard";
        case StoryStep::TutorialSolveBoard: return "TutorialSolveBoard";
        case StoryStep::TutorialSolveWhisper: return "TutorialSolveWhisper";
        case StoryStep::Day1Briefing: return "Day1Briefing";
        case StoryStep::Day1Investigation: return "Day1Investigation";
    }
    return "Unknown";
}
