#ifndef INVENTORYCATALOG_H
#define INVENTORYCATALOG_H

#include <string>

struct InventoryEntryDefinition;

class InventoryCatalog
{
public:
    static bool Load(const std::string &path);
    static const InventoryEntryDefinition *Find(const std::string &id);
    static void Clear();
};

#endif
