#include "InventoryCatalog.h"

#include "InventoryEntryDefinition.h"
#include "json.hpp"

#include <fstream>
#include <iostream>
#include <unordered_map>

using json = nlohmann::json;

namespace {
std::unordered_map<std::string, InventoryEntryDefinition> definitions;

bool ParseKind(const std::string &value, InventoryEntryKind &kind) {
    if (value == "item") kind = InventoryEntryKind::Item;
    else if (value == "evidence") kind = InventoryEntryKind::Evidence;
    else if (value == "dialogueFolder") kind = InventoryEntryKind::DialogueFolder;
    else return false;
    return true;
}

bool ParseCategory(const std::string &value, MindPlaceCategory &category) {
    if (value == "people") category = MindPlaceCategory::People;
    else if (value == "documents") category = MindPlaceCategory::Documents;
    else if (value == "evidence") category = MindPlaceCategory::Evidence;
    else return false;
    return true;
}

bool FileExists(const std::string &path) {
    std::ifstream file(path);
    return file.good();
}
}

bool InventoryCatalog::Load(const std::string &path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Inventory catalog not found: " << path << '\n';
        return false;
    }
    try {
        json root; file >> root;
        if (!root.contains("entries") || !root["entries"].is_array()) throw std::runtime_error("missing entries array");
        std::unordered_map<std::string, InventoryEntryDefinition> loaded;
        bool valid = true;
        for (const auto &value : root["entries"]) {
            InventoryEntryDefinition def;
            def.id = value.value("id", "");
            def.name = value.value("name", "");
            def.description = value.value("description", "");
            def.mindPlaceImage = value.value("mindPlaceImage", "");
            def.detailImage = value.value("detailImage", "");
            def.usableOnClueBoard = value.value("usableOnClueBoard", false);
            def.characterId = value.value("characterId", "");
            const bool rowValid = !def.id.empty() && !def.name.empty() && !def.mindPlaceImage.empty()
                && ParseKind(value.value("kind", ""), def.kind)
                && ParseCategory(value.value("category", ""), def.category)
                && FileExists(def.mindPlaceImage)
                && (def.detailImage.empty() || FileExists(def.detailImage))
                && (def.kind != InventoryEntryKind::DialogueFolder || !def.characterId.empty())
                && loaded.count(def.id) == 0;
            if (!rowValid) {
                std::cerr << "Invalid inventory entry: " << (def.id.empty() ? "<missing id>" : def.id) << '\n';
                valid = false;
                continue;
            }
            loaded.emplace(def.id, def);
        }
        definitions.swap(loaded);
        return valid;
    } catch (const std::exception &error) {
        std::cerr << "Invalid inventory catalog " << path << ": " << error.what() << '\n';
        return false;
    }
}

const InventoryEntryDefinition *InventoryCatalog::Find(const std::string &id) {
    const auto it = definitions.find(id);
    return it == definitions.end() ? nullptr : &it->second;
}

void InventoryCatalog::Clear() { definitions.clear(); }
