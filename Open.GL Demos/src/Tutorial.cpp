#include "Tutorial.h"
#include "SDL.h"

std::string getBasePath()
{
	char *bp = SDL_GetBasePath();
	std::string basePath(bp);
	SDL_free(bp);

	return basePath;
}