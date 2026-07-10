#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <unordered_map>
#include <string>

enum class StoryStep {
    TutorialTalkToBoss,
    TutorialOpenBoard,
    TutorialSolveBoard,
    TutorialSolveWhisper,
    Day1Briefing,
    Day1Investigation
};

class GameData {
public:
    static std::unordered_map<std::string, bool> flags;
    static void SetFlag(const std::string& flagName, bool value);
    static bool GetFlag(const std::string& flagName);
    static bool playerVictory;
    static StoryStep GetStoryStep();
    static bool AdvanceStory(StoryStep expected, StoryStep next);
    static const char* GetStoryStepName();

private:
    static StoryStep storyStep;
};

#endif
