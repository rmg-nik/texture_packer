#include <SDL3/SDL.h>

int main(int, char **) {

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow("test", 800, 600, SDL_WINDOW_RESIZABLE);

  SDL_Event event;

  bool is_running = true;
  while (is_running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        is_running = false;
        break;
      }
      SDL_Delay(1);
    }
  }
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
