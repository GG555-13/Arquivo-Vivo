#include "Game.h"
#include "StageState.h"
#include "TitleState.h"
#include "StageManager.h"
#include "Inventory.h"
#include "InventoryCatalog.h"
#include "ClueBoardData.h"

int main(int argc, char** argv) {
    Game& game = Game::GetInstance("Guilherme de Oliveira Mendes - 180113496", 1200, 900);

    StageManager::Initialize();
    Inventory::Clear();
    InventoryCatalog::Load("recursos/dados/inventory_entries.json");
    ClueBoardData::LoadLayout("recursos/dados/tutorial_board.json");
    
    game.Push(new TitleState());
    
    game.Run();
    return 0;
}
