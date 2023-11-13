//#include <SDL2/SDL.h>
#include <SDL.h>
#include <stdio.h>

int main(int argc, char* args[]) {
  SDL_Window* window = NULL;
  SDL_Surface* screenSurface = NULL;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
	fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
	return 1;
  }

  SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        SDL_Log("Erro ao obter o modo de exibição atual: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

  window = SDL_CreateWindow(
				"hello_sdl2",
				SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				displayMode.w, displayMode.h,
				SDL_WINDOW_FULLSCREEN
				);
  if (window == NULL) {
	fprintf(stderr, "could not create window: %s\n", SDL_GetError());
	return 1;
  }

 SDL_Renderer* renderer = NULL;
	renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

 SDL_SetRenderDrawColor( renderer, 255, 0, 255,  0);

	// Clear winow
	SDL_RenderClear( renderer );

SDL_Delay(500);

	Uint32 aa = SDL_GetTicks();
	int n = 0;
int i = 0;
	while ((SDL_GetTicks() - aa) < 5000) {

 SDL_SetRenderDrawColor( renderer, 255, 0, 255,  0);

	// Clear winow
	SDL_RenderClear( renderer );

SDL_Rect r;
	r.x = 100 + n;
	r.y = 300;
	r.w = 200;
	r.h = 200;

 SDL_SetRenderDrawColor( renderer, 0, 0, 255, 0);

	// Render rect
	SDL_RenderFillRect( renderer, &r );

	// Render the rect to the screen
	SDL_RenderPresent(renderer);

	n += 10;

//	SDL_LogDebug(0, "TESTEEEEEEEEEEEEE %i\n", i);
	SDL_Log("TESTEEEEEEEEEEEEE %i\n", i);   // use "adb logcat" to see this
i++;
  SDL_Delay(20);

	}
 
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}