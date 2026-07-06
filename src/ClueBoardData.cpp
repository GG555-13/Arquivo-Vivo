#include "ClueBoardData.h"
#include "InventoryCatalog.h"
#include "json.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

std::map<std::string, Vec2> ClueBoardData::placedPositions;

namespace {
std::vector<ClueBoardSlot> slots;
Rect placementArea;
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
        std::vector<ClueBoardSlot> loaded;
        bool valid = true;
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
        return valid;
    } catch (const std::exception &error) {
        std::cerr << "Invalid clue board layout " << path << ": " << error.what() << '\n';
        return false;
    }
}

const std::vector<ClueBoardSlot> &ClueBoardData::GetSlots() { return slots; }
const Rect &ClueBoardData::GetPlacementArea() { return placementArea; }
void ClueBoardData::ClearLayout() { slots.clear(); placedPositions.clear(); placementArea = Rect(); }
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
