#ifndef INVENTORYENTRYDEFINITION_H
#define INVENTORYENTRYDEFINITION_H

#include <string>

enum class InventoryEntryKind
{
    Item,
    Evidence,
    DialogueFolder
};

enum class MindPlaceCategory
{
    People,
    Documents,
    Evidence
};

struct InventoryEntryDefinition
{
    std::string id;
    InventoryEntryKind kind = InventoryEntryKind::Item;
    std::string name;
    std::string description;
    MindPlaceCategory category = MindPlaceCategory::Documents;
    std::string mindPlaceImage;
    std::string detailImage;
    bool usableOnClueBoard = false;
    std::string characterId;
};

#endif
