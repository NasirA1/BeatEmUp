#include "Rock.h"
#include "Util.h"

Rock::Rock(const string& file, SDL_Renderer* const renderer)
	: GameObject(1)
	, texture(NULL)
	, angle(0.0)
{
	util::SDLSurfaceFromFile surf(file, true);
	texture = SDL_CreateTextureFromSurface(renderer, surf.surface);
	
	position.x = 800;
	position.y = 410;
	position.w = surf.surface->w;
	position.h = surf.surface->h;
}


void Rock::Update()
{
	angle-=10;
	position.x-=5;

	//remove when out of view
	if(position.x + position.w < 0)
	{
		SetHealth(0);
	}
}


void Rock::Draw(SDL_Renderer* const renderer) const
{
	SDL_Rect nPos;
	util::Convert(position, nPos);	
	SDL_RenderCopyEx(renderer, texture, NULL, &nPos, angle, NULL, SDL_FLIP_NONE);
}


Rock::~Rock()
{
	if(texture)
	{
		SDL_DestroyTexture(texture);
		texture = NULL;
	}

	logPrintf("Rock object released");
}

