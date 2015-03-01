#include "Rock.h"


Rock::Rock(const string& file, SDL_Renderer* const renderer)
 : GameObject(1) 
 , texture(NULL)
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
	position.x-=10;

	//remove when out of view
	if(position.right() < 0)
	{
		SetHealth(0);
		//position.x = 900;
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

	logPrintf("Enemy object released");
}
