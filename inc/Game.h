#ifndef GAME_H
#define GAME_H

#include "Window.h"
#include "PlayState.h"
#include "FiniteStateMachine.h"

class Game
{
public:

   Game();
   ~Game() = default;

   Game(const Game&) = delete;
   Game& operator=(const Game&) = delete;

   Game(Game&&) = delete;
   Game& operator=(Game&&) = delete;

   bool  initialize(const std::string& title);
   void  executeGameLoop();

#ifdef __EMSCRIPTEN__
   void  updateWindowDimensions(int width, int height);
#endif

private:

   std::shared_ptr<Window>                 mWindow;

   std::shared_ptr<PlayState>              mPlayState;

   std::shared_ptr<FiniteStateMachine>     mFSM;
};

#endif
