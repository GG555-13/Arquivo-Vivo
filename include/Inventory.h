#ifndef INVENTORY_H
#define INVENTORY_H

#include <string>
#include <vector>

class Inventory
{
public:
    static bool Add(const std::string &entryId);
    static bool Remove(const std::string &entryId);
    static bool Has(const std::string &entryId);
    static const std::vector<std::string> &GetEntries();
    static bool HasPendingNotifications();
    static std::string PopPendingNotification();
    static void Clear();
    static void AddDialogueHistory(const std::string &characterId, const std::string &text);
    static std::string GetDialogueHistory(const std::string &characterId);
};

#endif
