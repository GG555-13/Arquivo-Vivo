#include "Game.h"
#include "StageState.h"
#include "TitleState.h"
#include "StageManager.h"

int main(int argc, char** argv) {
    Game& game = Game::GetInstance("Guilherme de Oliveira Mendes - 180113496", 1200, 900);

    StageManager::Initialize();
    
    game.Push(new TitleState());
    
    game.Run();
    return 0;
}