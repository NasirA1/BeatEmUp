#include "Roamer.h"




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
