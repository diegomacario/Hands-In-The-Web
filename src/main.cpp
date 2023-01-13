#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include "Game.h"

#ifdef __EMSCRIPTEN__
Game game;

void gameLoop()
{
   game.executeGameLoop();
}

extern "C" void EMSCRIPTEN_KEEPALIVE updateWindowDimensions(int width, int height)
{
   game.updateWindowDimensions(width, height);
}

bool pointerLocked = false;
bool touchControlsEnabled = false;

EM_BOOL on_pointerlockchange(int eventType, const EmscriptenPointerlockChangeEvent* event, void* userData)
{
   if (event->isActive == 0)
   {
      pointerLocked = false;
   }
   else
   {
      pointerLocked = true;
   }

   return 0;
}

EM_BOOL on_canvasclicked(int eventType, const EmscriptenMouseEvent* event, void* userData)
{
   touchControlsEnabled = false;

   return 0;
}

EM_BOOL enable_touch_controls_callback(int eventType, const EmscriptenTouchEvent* event, void* userData)
{
   if (!touchControlsEnabled)
   {
      touchControlsEnabled = true;
      game.enableTouchControlOfCamera(true);
   }

   return 0;
}
#endif

int main()
{
#ifndef __EMSCRIPTEN__
   Game game;
#endif

   if (!game.initialize("Hands In The Web"))
   {
      std::cout << "Error - main - Failed to initialize the game" << "\n";
      return -1;
   }

#ifdef __EMSCRIPTEN__
   emscripten_set_click_callback("#canvas", NULL, 0, on_canvasclicked);

   emscripten_set_pointerlockchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, 0, on_pointerlockchange);

   emscripten_set_touchstart_callback("#canvas", NULL, 0, enable_touch_controls_callback);

   emscripten_set_main_loop(gameLoop, 0, true);
#else
   game.executeGameLoop();
#endif

   return 0;
}
