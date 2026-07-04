#include "ClueBoardData.h"
#include "InventoryCatalog.h"
#include "json.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

std::vector<Vec2> ClueBoardData::cluePositions;
std::unordered_map<std::string, Vec2> ClueBoardData::entryPositions;
std::vector<std::string> ClueBoardData::lockedEntries;

namespace { std::vector<ClueBoardSlot> slots; }

std::vector<Vec2> ClueBoardData::GetDefaultPositions() {
    std::vector<Vec2> defaults;
    defaults.push_back(Vec2(260.0f, 200.0f));
    defaults.push_back(Vec2(410.0f, 200.0f));
    defaults.push_back(Vec2(560.0f, 200.0f));
    defaults.push_back(Vec2(335.0f, 340.0f));
    return defaults;
}

bool ClueBoardData::LoadLayout(const std::string &path) {
    std::ifstream file(path);
    if (!file) { std::cerr << "Clue board layout not found: " << path << '\n'; return false; }
    try {
        nlohmann::json root; file >> root;
        if (!root.contains("slots") || !root["slots"].is_array()) throw std::runtime_error("missing slots array");
        std::vector<ClueBoardSlot> loaded;
        bool valid = true;
        for (const auto &value : root["slots"]) {
            ClueBoardSlot slot;
            slot.entryId = value.value("entryId", "");
            slot.snapRadius = value.value("snapRadius", 0.0f);
            if (value.contains("target")) {
                slot.targetPosition = Vec2(value["target"].value("x", 0.0f), value["target"].value("y", 0.0f));
            }
            const bool duplicate = std::find_if(loaded.begin(), loaded.end(), [&](const ClueBoardSlot &item) {
                return item.entryId == slot.entryId;
            }) != loaded.end();
            if (slot.entryId.empty() || slot.snapRadius <= 0.0f || !InventoryCatalog::Find(slot.entryId) || duplicate) {
                std::cerr << "Invalid clue board slot: " << (slot.entryId.empty() ? "<missing id>" : slot.entryId) << '\n';
                valid = false;
                continue;
            }
            loaded.push_back(slot);
        }
        slots.swap(loaded);
        return valid;
    } catch (const std::exception &error) {
        std::cerr << "Invalid clue board layout " << path << ": " << error.what() << '\n';
        return false;
    }
}

const std::vector<ClueBoardSlot> &ClueBoardData::GetSlots() { return slots; }
void ClueBoardData::ClearLayout() { slots.clear(); entryPositions.clear(); lockedEntries.clear(); cluePositions.clear(); }
bool ClueBoardData::IsLocked(const std::string &entryId) {
    return std::find(lockedEntries.begin(), lockedEntries.end(), entryId) != lockedEntries.end();
}
void ClueBoardData::Lock(const std::string &entryId) {
    if (!IsLocked(entryId)) lockedEntries.push_back(entryId);
}
