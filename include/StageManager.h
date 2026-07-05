#ifndef STAGEMANAGER_H
#define STAGEMANAGER_H

#include <unordered_map>
#include <string>
#include <StageConfig.h>

class StageManager {
public:
    static void Initialize();
    static StageConfig GetStage(const std::string& stageId);

private:
    static std::unordered_map<std::string, StageConfig> stages;
};

#endif