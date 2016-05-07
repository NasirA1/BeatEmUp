#include "Roamer.h"



Roamer::Roamer(SDL_Renderer& renderer, Sprite::ptr walkLeftSprite, Sprite::ptr walkRightSprite
	, float posX, float posY, float roamMinX_, float roamMaxX_,  bool backgroundObj)
	: GameObject("", GT_Background)	
	, walkRight(std::move(walkRightSprite))
	, walkLeft(std::move(walkLeftSprite))
	, current(nullptr)
	, roamMinX(roamMinX_)
	, roamMaxX(roamMaxX_)
	, backgroundObject(backgroundObj)
{
	position.x = posX, position.y = posY, position.w = (float)walkLeft->Position().w;
	position.h = (float)walkLeft->Position().h;
	SetSpeed(2.0f, 1.0f);

	if(backgroundObject)
		position.z = -9;
	else
		AdjustZToGameDepth();
}


void Roamer::Update()
{
	if(position.x <= roamMinX)
	{
		SetDirection(Direction::Right);
		xVel = speedX;
	}
	else if(position.x >= roamMaxX)
	{
		SetDirection(Direction::Left);
		xVel = -speedX;
	}

	Translate(true);

	//Propagate to the underlying currently active sprite
	current->Position().x = position.x;
	current->Position().y = position.y;
	current->Update();
}


void Roamer::Draw(SDL_Renderer& renderer) const
{
	current->Draw(renderer);
}


Roamer::~Roamer()
{
	logPrintf("Roamer object released");
}


void Roamer::SetDirection(Direction dir)
{
	GameObject::SetDirection(dir);
	if (GetDirection() == Direction::Right) current = walkRight.get();
	else if(GetDirection() == Direction::Left) current = walkLeft.get();
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
