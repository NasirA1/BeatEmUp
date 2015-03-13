#include "Enemy.h"
#include "Game.h"
#include "Player.h"



#pragma region Rock

const float Rock::Range(10.0f * (float)GAME.ClientWidth());


Rock::Rock(const string& file, SDL_Renderer* const renderer)
 : GameObject(GT_Enemy, 1, Left) 
 , texture(NULL)
{
	util::SDLSurfaceFromFile surf(file, true);
	texture = SDL_CreateTextureFromSurface(renderer, surf.surface);
	
	speed = 10.0f;
	position.x = Range;
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

	if(position.x >= Range)
	{
		SetDirection(Left);
	}
	else if(position.x < -Range)
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

#pragma endregion



Andore::Andore(SDL_Renderer* const renderer, Sprite* walkLeftSprite, Sprite* walkRightSprite
	, Sprite* punchLeftSprite, Sprite* punchRightSprite, Sprite* hitLeftSprite, Sprite* hitRightSprite
	, float posX, float posY)
	: GameObject(GT_Enemy, 100)	
	, walkLeft(walkLeftSprite)
	, walkRight(walkRightSprite)
	, punchLeft(punchLeftSprite)
	, punchRight(punchRightSprite)
	, hitLeft(hitLeftSprite)
	, hitRight(hitRightSprite)
	, current(NULL)
	, state(ES_Patrolling)
	, punchTimer(0)
	, idleTimer(0)
	, recoveryTimer(0)
{
	position.x = posX, position.y = posY, position.w = (float)walkLeft->Position().w;
	position.h = (float)walkLeft->Position().h;
	speed = 1.0f;
	AdjustZToGameDepth();

	//Start chasing now
	//TODO: add patrolling logic later
	state = ES_Chasing;
	speed = 1.0f;
}



void Andore::OnPlayerAttack()
{
	if(state != ES_Attacking)
	{
		Stop();
		current = GetDirection() == Left? hitLeft: hitRight;
		state = ES_Hit;
		SetHealth(GetHealth() - 1);
		if(GetHealth() > 0)
			recoveryTimer = SDL_GetTicks() + 200;
		else
			MIXER.Play(Mixer::SE_DragonRoar); //death
	}
}


const float MaxDistX = 50.0f;
const float MaxDistY = 0.0f;

void Andore::Update()
{
	//Chase player
	float distX = position.x - GAME.player->Position().x;
	float distY = position.y - GAME.player->Position().y;

	//when hit, check if recovered and resume chasing player
	if(state == ES_Hit && SDL_GetTicks() > recoveryTimer)
	{
		Stop();
		state = ES_Idle;
		recoveryTimer = 0;
		//Face player
		//SetDirection(position.x > GAME.player->Position().x? Left: Right);
	}

	if(state == ES_Chasing)
	{
		if(distY > MaxDistY) yVel = -speed;
		else if(distY < -MaxDistY) yVel = speed;
		else yVel = 0.0f;

		if(distX > MaxDistX)
		{
			SetDirection(Left);
			xVel = -speed;
		}
		else if(distX < -MaxDistX)
		{
			SetDirection(Right);
			xVel = speed;
		}

		if(SDL_abs(distX) <= MaxDistX 
			&& SDL_abs(distY) <= MaxDistY)
		{
			Stop();
			Attack();
		}
	}
	else if(state == ES_Attacking)
	{
		current = GetDirection() == Left? punchLeft: punchRight;

		if(SDL_GetTicks() - punchTimer > 300)
		{
			state = ES_Idle;
			punchTimer = 0;
		}
	}
	else if(state == ES_Idle)
	{
		if(!idleTimer)
		{
			Stop();
			idleTimer = SDL_GetTicks() + Random::Instance().Next(1000, 3000);
		}
		else
		{
			if(SDL_GetTicks() >= idleTimer || GAME.player->isMoving())
			{
				state = ES_Chasing;
				idleTimer = 0;
			}
			else
			{
				//Face player all the time when on idle
				SetDirection(position.x > GAME.player->Position().x? Left: Right);
			}
		}
	}

	//Handle BG scrolling
	if(GAME.bg->IsScrolling())
	{
		position.x += GAME.bg->GetDirection() == Left?
			-GAME.bg->GetSpeed(): GAME.bg->GetSpeed();
	}

	//Translate/animate
	Translate(xVel != 0 || yVel != 0 || state == ES_Attacking);

	//Propagate to the underlying currently active sprite
	current->Position().x = position.x;
	current->Position().y = position.y;
	current->Update();
}


void Andore::Attack()
{
	state = ES_Attacking;
	punchTimer = SDL_GetTicks();
}


void Andore::Draw(SDL_Renderer* const renderer) const
{
	current->Draw(renderer);
}


Andore::~Andore()
{
	util::Delete(walkLeft);
	util::Delete(walkRight);
	util::Delete(punchLeft);
	util::Delete(punchRight);
	util::Delete(hitLeft);
	util::Delete(hitRight);
	logPrintf("Andore object released");
}


void Andore::SetDirection(Directions dir)
{
	GameObject::SetDirection(dir);
	if (GetDirection() == Right) current = walkRight;
	else if(GetDirection() == Left) current = walkLeft;
}


void Andore::Stop()
{
	xVel = yVel = 0;
	current = GetDirection() == Left? walkLeft: walkRight;
	current->SetStill();
}


void Andore::Translate(bool anim)
{
	current->SetAnimation(anim);
	position.x += xVel;
	position.y += yVel;
	position.y = position.y < GAME.MoveBounds.top()? GAME.MoveBounds.top(): position.y;
	//logPrintf("Andore Translate: Pos {%d, %d}", (int)position.x, (int)position.y);
	AdjustZToGameDepth();
}


