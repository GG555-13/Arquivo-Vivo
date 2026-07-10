#ifndef CLUEBOARDDATA_H
#define CLUEBOARDDATA_H

#include "Rect.h"
#include "Vec2.h"
#include "SDL_include.h"
#include <cstddef>
#include <map>
#include <string>
#include <vector>

struct ClueBoardSlot {
    std::string entryId;
    Vec2 trayPosition;
};

struct ClueBoardQuestion {
    std::string prompt;
    std::vector<std::string> acceptedAnswers;
    int maxLength = 32;
    Vec2 promptPosition;
    Vec2 inputPosition;
};

struct ClueBoardQuestionStyle {
    std::string fontFile = "recursos/font/SpecialElite-Regular.ttf";
    int promptFontSize = 22;
    int answerFontSize = 22;
    int fieldWidth = 240;
    SDL_Color textColor = {45, 35, 30, 255};
    std::string incorrectDialogueFile;
};

struct ClueBoardData {
    static bool LoadLayout(const std::string &path);
    static const std::vector<ClueBoardSlot> &GetSlots();
    static const Rect &GetPlacementArea();
    static const std::vector<ClueBoardQuestion> &GetQuestions();
    static const ClueBoardQuestionStyle &GetQuestionStyle();
    static void ClearLayout();
    static void ClearProgress();
    static bool GetPlacedPosition(const std::string &entryId, Vec2 &position);
    static void SetPlacedPosition(const std::string &entryId, const Vec2 &position);
    static void ClearPlacedPosition(const std::string &entryId);
    static bool GetSolvedAnswer(std::size_t questionIndex, std::string &answer);
    static void SetSolvedAnswer(std::size_t questionIndex, const std::string &answer);

private:
    static std::map<std::string, Vec2> placedPositions;
    static std::map<std::size_t, std::string> solvedAnswers;
};

#endif
