#include "Background.h"
#include "Util.h"
#include <sstream>

using namespace util;


Background::Background(int clientWidth, int clientHeight, SDL_Renderer* const renderer)
	: GameObject(1, Left)
	, scroll(false)
	, bg1(new BackgroundLayer("resources/bg1.gif", renderer, clientWidth, clientHeight, 0.25f))
	, bg2(new BackgroundLayer("resources/bg2.gif", renderer, clientWidth, clientHeight, 2.0f))
	, bg3(new BackgroundLayer("resources/bg3.gif", renderer, clientWidth, clientHeight, 3.0f))
{
	layers.push_back(bg1);
	layers.push_back(bg2);
	layers.push_back(bg3);
}


void Background::Update()
{
	if (!scroll)
	{
		return;
	}

	for(GameObjectList::iterator it = layers.begin(); it != layers.end(); ++it)
	{
		BackgroundLayer& layer = dynamic_cast<BackgroundLayer&>(**it); 
		layer.SetDirection(GetDirection());
		layer.Update();
	}
}


void Background::Draw(SDL_Renderer* const renderer) const
{
	for(GameObjectList::const_iterator it = layers.begin(); it != layers.end(); ++it)
	{
		(*it)->Draw(renderer);
	}
}


Background::~Background()
{
	//Memory freed via GameObjectList .dtor call
	logPrintf("Background object released");
}


BackgroundLayer::BackgroundLayer(const std::string& filename, SDL_Renderer* const renderer,
	int _screenWidth, int _screenHeight, float xVel)
	: GameObject(1, Left)
{
	SDLSurfaceFromFile fileSurface(filename);
	texture = SDL_CreateTextureFromSurface(renderer, fileSurface.surface);

	if( !texture )
	{
		logPrintf( "Unable to create texture from %s! SDL Error: %s\n", filename.c_str(), SDL_GetError() );
	}
	else
	{
		GameObject::xVel = xVel;
		screenWidth = _screenWidth, screenHeight = _screenHeight;
		pos1.w = fileSurface.surface->w, pos1.h = fileSurface.surface->h;
		pos1.x = 0, pos1.y = 0;
		
		pos2.w = fileSurface.surface->w, pos2.h = fileSurface.surface->h;
		pos2.x = pos2.w, pos2.y = 0;		
	}
}


BackgroundLayer::~BackgroundLayer()
{
	if(texture)
	{
		SDL_DestroyTexture(texture);
		texture = NULL;
	}

	logPrintf("BackgroundLayer released");
}


void BackgroundLayer::Update()
{
	if (GetDirection() == Left) pos1.x -= xVel, pos2.x -= xVel;
	else if (GetDirection() == Right) pos1.x += xVel, pos2.x += xVel;

	//std::stringstream ss;
	//ss << "pos1 {"<< pos1.x << ","<< pos1.y << ","<< pos1.w << "," << pos1.h << "}" << "  " 
	//	 << "pos2 {"<< pos2.x << ","<< pos2.y << ","<< pos2.w << "," << pos2.h << "}" << "  ";
	//logPrintf("%s", ss.str().c_str());

	if(GetDirection() == Left)
	{
		if(pos1.x <= -pos1.w)
		{
			pos1.x = pos2.x + pos2.w;
		}

		if(pos2.x <= -pos2.w)
		{
			pos2.x = pos1.x + pos1.w;
		}
	}
	else if(GetDirection() == Right)
	{
		if(pos1.x >= pos1.w)
		{
			pos1.x = pos2.x - pos2.w;
		}

		if(pos2.x >= pos2.w)
		{
			pos2.x = pos1.x - pos1.w;
		}
	}
}


void BackgroundLayer::Draw(SDL_Renderer* const renderer) const
{
	SDL_Rect nPos1, nPos2;
	util::Convert(pos1, nPos1);
	util::Convert(pos2, nPos2);
	
	SDL_RenderCopy( renderer, texture, NULL, &nPos1 );
	SDL_RenderCopy( renderer, texture, NULL, &nPos2 );
}
