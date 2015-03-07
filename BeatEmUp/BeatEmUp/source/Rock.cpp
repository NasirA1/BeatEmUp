#include "Rock.h"
#include "Game.h"
#include "Player.h"


const int RANGE = 2 * GAME.ClientWidth();



Rock::Rock(const string& file, SDL_Renderer* const renderer)
 : GameObject(GT_Enemy, 1, Left) 
 , texture(NULL)
{
	util::SDLSurfaceFromFile surf(file, true);
	texture = SDL_CreateTextureFromSurface(renderer, surf.surface);
	
	speed = 10.0f;
	position.x = (float)RANGE;
	position.w = (float)surf.surface->w;
	position.h = (float)surf.surface->h;
	position.y = (float)GAME.player->Position().y + 20;
	AdjustZToGameDepth();
}



void Rock::Update()
{
	if(GetDirection() == Left)
	{
		SetAngle(GetAngle() - speed);
		xVel = -speed;
	}
	else
	{
		SetAngle(GetAngle() + speed);
		xVel = speed;
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

	AdjustZToGameDepth();
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
	, float posX, float posY, float roamMinX_, float roamMaxX_,  bool backgroundObj)
	: GameObject(GT_Background)	
	, walkRight(walkRightSprite)
	, walkLeft(walkLeftSprite)
	, current(NULL)
	, roamMinX(roamMinX_)
	, roamMaxX(roamMaxX_)
	, backgroundObject(backgroundObj)
{
	position.x = posX, position.y = posY, position.w = (float)walkLeft->Position().w;
	position.h = (float)walkLeft->Position().h;
	speed = 2.0f;

	if(backgroundObject)
		position.z = -9;
	else
		AdjustZToGameDepth();
}



void Roamer::Update()
{
	if(position.x <= roamMinX)
	{
		SetDirection(Right);
		xVel = speed;
	}
	else if(position.x >= roamMaxX)
	{
		SetDirection(Left);
		xVel = -speed;
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
	if(!backgroundObject)
		AdjustZToGameDepth();
}

