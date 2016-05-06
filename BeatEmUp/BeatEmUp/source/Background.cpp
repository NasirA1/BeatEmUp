#include "Background.h"
#include "Util.h"
#include <sstream>

using namespace util;


Background::Background(int clientWidth, int clientHeight, SDL_Renderer& renderer 
	, const std::string& fileLayer1, const std::string& fileLayer2, const std::string& fileLayer3)
	: GameObject("", GT_Background, 1, Direction::Left, 3.0f)
	, scroll(false)
	, bg1(new BackgroundLayer(fileLayer1, renderer, clientWidth, clientHeight, 0.25f))
	, bg2(new BackgroundLayer(fileLayer2, renderer, clientWidth, clientHeight, 2.0f))
	, bg3(new BackgroundLayer(fileLayer3, renderer, clientWidth, clientHeight, 3.0f))
{
	//speedX = 3.0f; //speed of the outermost layer
	position.z = -99999; //bg z-order
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


void Background::Draw(SDL_Renderer& renderer) const
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


BackgroundLayer::BackgroundLayer(const std::string& filename, SDL_Renderer& renderer,
	int _screenWidth, int _screenHeight, float xVel)
	: GameObject("", GT_Background, 1, Direction::Left)
{
	SDLSurfaceFromFile fileSurface(filename);
	texture = SDL_CreateTextureFromSurface(&renderer, fileSurface.surface);

	if( !texture )
	{
		logPrintf( "Unable to create texture from %s! SDL Error: %s\n", filename.c_str(), SDL_GetError() );
	}
	else
	{
		GameObject::xVel = xVel;
		screenWidth = _screenWidth, screenHeight = _screenHeight;
		pos1.w = (float)fileSurface.surface->w, pos1.h = (float)fileSurface.surface->h;
		pos1.x = 0.0f, pos1.y = 0.0f;
		
		pos2.w = (float)fileSurface.surface->w, pos2.h = (float)fileSurface.surface->h;
		pos2.x = pos2.w, pos2.y = 0.0f;		
	}
}


BackgroundLayer::~BackgroundLayer()
{
	if(texture)
	{
		SDL_DestroyTexture(texture);
		texture = nullptr;
	}

	logPrintf("BackgroundLayer released");
}


void BackgroundLayer::Update()
{
	if (GetDirection() == Direction::Left) pos1.x -= xVel, pos2.x -= xVel;
	else if (GetDirection() == Direction::Right) pos1.x += xVel, pos2.x += xVel;

	//std::stringstream ss;
	//ss << "pos1 {"<< pos1.x << ","<< pos1.y << ","<< pos1.w << "," << pos1.h << "}" << "  " 
	//	 << "pos2 {"<< pos2.x << ","<< pos2.y << ","<< pos2.w << "," << pos2.h << "}" << "  ";
	//logPrintf("%s", ss.str().c_str());

	if(GetDirection() == Direction::Left)
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
	else if(GetDirection() == Direction::Right)
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


void BackgroundLayer::Draw(SDL_Renderer& renderer) const
{
	SDL_Rect nPos1, nPos2;
	util::Convert(pos1, nPos1);
	util::Convert(pos2, nPos2);
	
	SDL_RenderCopy( &renderer, texture, nullptr, &nPos1 );
	SDL_RenderCopy( &renderer, texture, nullptr, &nPos2 );
}
