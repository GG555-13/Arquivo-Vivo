#include "Inventory.h"
#include "InventoryCatalog.h"
#include "InventoryEntryDefinition.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>

namespace {
std::vector<std::string> entries;
std::vector<std::string> pendingNotifications;
std::unordered_map<std::string, std::vector<std::string>> histories;
}

bool Inventory::Add(const std::string &entryId) {
    if (entryId.empty() || !InventoryCatalog::Find(entryId)) {
        std::cerr << "Cannot acquire unknown inventory entry: " << (entryId.empty() ? "<empty>" : entryId) << '\n';
        return false;
    }
    if (Has(entryId)) return false;
    entries.push_back(entryId);
    const InventoryEntryDefinition *definition = InventoryCatalog::Find(entryId);
    if (definition && definition->kind != InventoryEntryKind::DialogueFolder) {
        pendingNotifications.push_back(entryId);
    }
    return true;
}

bool Inventory::Has(const std::string &entryId) {
    return std::find(entries.begin(), entries.end(), entryId) != entries.end();
}

const std::vector<std::string> &Inventory::GetEntries() { return entries; }

bool Inventory::HasPendingNotifications() {
    return !pendingNotifications.empty();
}

std::string Inventory::PopPendingNotification() {
    if (pendingNotifications.empty()) return "";
    const std::string entryId = pendingNotifications.front();
    pendingNotifications.erase(pendingNotifications.begin());
    return entryId;
}

void Inventory::Clear() {
    entries.clear();
    pendingNotifications.clear();
    histories.clear();
}

void Inventory::AddDialogueHistory(const std::string &characterId, const std::string &text) {
    if (!characterId.empty() && !text.empty()) histories[characterId].push_back(text);
}

std::string Inventory::GetDialogueHistory(const std::string &characterId) {
    const auto it = histories.find(characterId);
    if (it == histories.end()) return "Nenhuma conversa registrada.";
    std::string result;
    for (const std::string &line : it->second) {
        if (!result.empty()) result += "\n\n";
        result += line;
    }
    return result;
}
