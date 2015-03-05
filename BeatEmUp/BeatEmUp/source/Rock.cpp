#include "Rock.h"
#include "Game.h"
#include "Player.h"


const int RANGE = 2 * GAME.ClientWidth();
const double ROTATION_RATE = 10.0000;
const float VELOCITY = 10.0f;



Rock::Rock(const string& file, SDL_Renderer* const renderer)
 : GameObject(1, Left) 
 , texture(NULL)
{
	util::SDLSurfaceFromFile surf(file, true);
	texture = SDL_CreateTextureFromSurface(renderer, surf.surface);
	
	position.x = (float)RANGE;
	position.w = (float)surf.surface->w;
	position.h = (float)surf.surface->h;
	//position.y = (float)GAME.MidSectionY((int)position.h) - 10.0f;
	position.y = (float)GAME.player->Position().y + 20;
}



void Rock::Update()
{
	if(GetDirection() == Left)
	{
		SetAngle(GetAngle() - ROTATION_RATE);
		xVel = -VELOCITY;
	}
	else
	{
		SetAngle(GetAngle() + ROTATION_RATE);
		xVel = VELOCITY;
	}

	if(position.x >= RANGE)
	{
		SetDirection(Left);
	}
	else if(position.x < -RANGE)
	{
		SetDirection(Right);
	}

	position.x += xVel;
}



void Rock::Draw(SDL_Renderer* const renderer) const
{
	SDL_Rect nPos;
	util::Convert(position, nPos);
	SDL_RenderCopyEx(renderer, texture, NULL, &nPos, GetAngle(), NULL, SDL_FLIP_NONE);
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



Roamer::Roamer(SDL_Renderer* const renderer, Sprite* walkLeftSprite, Sprite* walkRightSprite
	, float posX, float posY, float roamMinX_, float roamMaxX_)
	: GameObject()	
	, walkRight(walkRightSprite)
	, walkLeft(walkLeftSprite)
	, current(NULL)
	, roamMinX(roamMinX_)
	, roamMaxX(roamMaxX_)
{
	position.x = posX, position.y = posY, position.w = (float)walkLeft->Position().w;
	position.h = (float)walkLeft->Position().h; 
}



void Roamer::Update()
{
	if(position.x <= roamMinX)
	{
		SetDirection(Right);
		xVel = 2;
	}
	else if(position.x >= roamMaxX)
	{
		SetDirection(Left);
		xVel = -2;
	}

	Translate(true);

	//Propagate to the underlying currently active sprite
	current->Position().x = position.x;
	current->Position().y = position.y;
	current->Update();
}


void Roamer::Draw(SDL_Renderer* const renderer) const
{
	current->Draw(renderer);
}


Roamer::~Roamer()
{
	util::Delete(walkRight);
	util::Delete(walkLeft);

	logPrintf("Roamer object released");
}


void Roamer::SetDirection(Directions dir)
{
	GameObject::SetDirection(dir);
	if (GetDirection() == Right) current = walkRight;
	else if(GetDirection() == Left) current = walkLeft;
}


void Roamer::Stop()
{
	//position.x -= xVel;
	//position.y -= yVel;
	//xVel = yVel = 0;
	//current->SetStill();
}


void Roamer::Translate(bool anim)
{
	current->SetAnimation(anim);
	position.x += xVel;
	position.y += yVel;
	//logPrintf("Translate: Pos {%d, %d}", position.x, position.y); 
}

