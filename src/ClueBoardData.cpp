#include "ClueBoardData.h"
#include "InventoryCatalog.h"
#include "json.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

std::map<std::string, Vec2> ClueBoardData::placedPositions;
std::map<std::size_t, std::string> ClueBoardData::solvedAnswers;

namespace {
std::vector<ClueBoardSlot> slots;
Rect placementArea;
std::vector<ClueBoardQuestion> questions;
ClueBoardQuestionStyle questionStyle;

SDL_Color ReadColor(const nlohmann::json &value, SDL_Color fallback) {
    if (!value.is_object()) return fallback;
    return {
        static_cast<Uint8>(value.value("r", static_cast<int>(fallback.r))),
        static_cast<Uint8>(value.value("g", static_cast<int>(fallback.g))),
        static_cast<Uint8>(value.value("b", static_cast<int>(fallback.b))),
        static_cast<Uint8>(value.value("a", static_cast<int>(fallback.a)))
    };
}
}

bool ClueBoardData::LoadLayout(const std::string &path) {
    std::ifstream file(path);
    if (!file) { std::cerr << "Clue board layout not found: " << path << '\n'; return false; }
    try {
        nlohmann::json root; file >> root;
        if (!root.contains("slots") || !root["slots"].is_array()) throw std::runtime_error("missing slots array");
        if (!root.contains("placementArea") || !root["placementArea"].is_object()) throw std::runtime_error("missing placementArea");
        const auto &area = root["placementArea"];
        Rect loadedArea(area.value("x", 0.0f), area.value("y", 0.0f),
                        area.value("w", 0.0f), area.value("h", 0.0f));
        if (loadedArea.w <= 0.0f || loadedArea.h <= 0.0f) throw std::runtime_error("invalid placementArea");
        bool valid = true;
        ClueBoardQuestionStyle loadedStyle;
        if (root.contains("questionPresentation") && root["questionPresentation"].is_object()) {
            const auto &style = root["questionPresentation"];
            loadedStyle.fontFile = style.value("font", loadedStyle.fontFile);
            loadedStyle.promptFontSize = style.value("promptFontSize", loadedStyle.promptFontSize);
            loadedStyle.answerFontSize = style.value("answerFontSize", loadedStyle.answerFontSize);
            loadedStyle.fieldWidth = style.value("fieldWidth", loadedStyle.fieldWidth);
            loadedStyle.incorrectDialogueFile = style.value("incorrectDialogue", loadedStyle.incorrectDialogueFile);
            if (style.contains("textColor")) loadedStyle.textColor = ReadColor(style["textColor"], loadedStyle.textColor);
        }
        std::vector<ClueBoardQuestion> loadedQuestions;
        if (root.contains("questions") && root["questions"].is_array()) {
            for (const auto &value : root["questions"]) {
                if (!value.is_object()) {
                    std::cerr << "Invalid clue board question; skipping\n";
                    valid = false;
                    continue;
                }
                ClueBoardQuestion question;
                question.prompt = value.value("prompt", "");
                question.maxLength = value.value("maxLength", 32);
                if (value.contains("promptPosition")) {
                    question.promptPosition = Vec2(value["promptPosition"].value("x", 0.0f), value["promptPosition"].value("y", 0.0f));
                }
                if (value.contains("inputPosition")) {
                    question.inputPosition = Vec2(value["inputPosition"].value("x", 0.0f), value["inputPosition"].value("y", 0.0f));
                }
                if (value.contains("acceptedAnswers") && value["acceptedAnswers"].is_array()) {
                    for (const auto &answer : value["acceptedAnswers"]) {
                        if (answer.is_string() && !answer.get<std::string>().empty()) question.acceptedAnswers.push_back(answer.get<std::string>());
                    }
                }
                if (question.prompt.empty() || question.acceptedAnswers.empty() || question.maxLength <= 0) {
                    std::cerr << "Invalid clue board question; skipping\n";
                    valid = false;
                    continue;
                }
                loadedQuestions.push_back(question);
            }
        }
        std::vector<ClueBoardSlot> loaded;
        for (const auto &value : root["slots"]) {
            ClueBoardSlot slot;
            slot.entryId = value.value("entryId", "");
            if (value.contains("tray")) {
                slot.trayPosition = Vec2(value["tray"].value("x", 0.0f), value["tray"].value("y", 0.0f));
            }
            const bool duplicate = std::find_if(loaded.begin(), loaded.end(), [&](const ClueBoardSlot &item) {
                return item.entryId == slot.entryId;
            }) != loaded.end();
            if (slot.entryId.empty() || slot.trayPosition.x <= 0.0f || slot.trayPosition.y <= 0.0f ||
                !InventoryCatalog::Find(slot.entryId) || duplicate) {
                std::cerr << "Invalid clue board slot: " << (slot.entryId.empty() ? "<missing id>" : slot.entryId) << '\n';
                valid = false;
                continue;
            }
            loaded.push_back(slot);
        }
        slots.swap(loaded);
        placementArea = loadedArea;
        questions.swap(loadedQuestions);
        questionStyle = loadedStyle;
        return valid;
    } catch (const std::exception &error) {
        std::cerr << "Invalid clue board layout " << path << ": " << error.what() << '\n';
        return false;
    }
}

const std::vector<ClueBoardSlot> &ClueBoardData::GetSlots() { return slots; }
const Rect &ClueBoardData::GetPlacementArea() { return placementArea; }
const std::vector<ClueBoardQuestion> &ClueBoardData::GetQuestions() { return questions; }
const ClueBoardQuestionStyle &ClueBoardData::GetQuestionStyle() { return questionStyle; }
void ClueBoardData::ClearLayout() { slots.clear(); questions.clear(); placedPositions.clear(); solvedAnswers.clear(); placementArea = Rect(); questionStyle = ClueBoardQuestionStyle(); }
void ClueBoardData::ClearProgress() { placedPositions.clear(); solvedAnswers.clear(); }
bool ClueBoardData::GetPlacedPosition(const std::string &entryId, Vec2 &position) {
    const auto found = placedPositions.find(entryId);
    if (found == placedPositions.end()) return false;
    position = found->second;
    return true;
}
void ClueBoardData::SetPlacedPosition(const std::string &entryId, const Vec2 &position) {
    placedPositions[entryId] = position;
}
void ClueBoardData::ClearPlacedPosition(const std::string &entryId) { placedPositions.erase(entryId); }
bool ClueBoardData::GetSolvedAnswer(std::size_t questionIndex, std::string &answer) {
    const auto found = solvedAnswers.find(questionIndex);
    if (found == solvedAnswers.end()) return false;
    answer = found->second;
    return true;
}
void ClueBoardData::SetSolvedAnswer(std::size_t questionIndex, const std::string &answer) {
    solvedAnswers[questionIndex] = answer;
}
