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



Enemy::Enemy(SDL_Renderer* const renderer
	, Sprite* walkLeftSprite, Sprite* walkRightSprite
	, Sprite* punchLeftSprite, Sprite* punchRightSprite
	, Sprite* hitLeftSprite, Sprite* hitRightSprite
	, Sprite* fallLeftSprite, Sprite* fallRightSprite
	, float posX, float posY)
	: GameObject(GT_Enemy, 50)	
	, walkLeft(walkLeftSprite)
	, walkRight(walkRightSprite)
	, punchLeft(punchLeftSprite)
	, punchRight(punchRightSprite)
	, hitLeft(hitLeftSprite)
	, hitRight(hitRightSprite)
	, fallLeft(fallLeftSprite)
	, fallRight(fallRightSprite)
	, current(NULL)
	, state(ES_Patrolling)
	, punchTimer(0)
	, idleTimer(0)
	, recoveryTimer(0)
	, hitCount(0)
	, jumpState(JS_Ground)
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



const Uint8 KnockDownHitCount = 15;
void Enemy::OnPlayerAttack()
{
	if(state != ES_Attacking && state != ES_KnockedDown)
	{
		Stop();
		current = GetDirection() == Left? hitLeft: hitRight;
		state = ES_Hit;
		hitCount++;
		SetHealth(GetHealth() - 1);
	
		if(GetHealth() > 0 && hitCount < KnockDownHitCount){
			recoveryTimer = SDL_GetTicks() + 200;
		}
		else
		{
			hitCount = 0;
			current = GetDirection() == Left? fallLeft: fallRight;
			current->SetCurrentFrame(0);
			state = ES_KnockedDown;
			recoveryTimer = 0;
			Jump(8.0f, 10.0f);
		}
	}
}


const float Enemy::Gravity(2.0f);
const int Enemy::JumpHeight(50);


void Enemy::Jump(float xAccel, float yAccel)
{
	jumpLocation.x = position.x;
	jumpLocation.y = position.y;
	xVel = GetDirection() == Right? -xAccel: xAccel;
	yVel = -yAccel;
	jumpState = JS_Jumped;
}


void Enemy::HandleKnockedDown()
{
	//Jump start..
	//Shoot up (yVel acceleration)...
	if(jumpState == JS_Jumped)
	{
		yVel += Gravity/(float)JumpHeight;
		if(position.y <= jumpLocation.y - JumpHeight) 
			jumpState = JS_Landing;
	}
	//Landing (in the air)..
	else if(jumpState == JS_Landing)
	{
		//Not landed yet..
		if(position.y < jumpLocation.y)
		{
			yVel += Gravity;
			xVel += GetDirection() == Right? 0.15f: -0.15f;
		}
		//Landed. On the ground now...
		else 
		{
			jumpState = JS_Ground;
			xVel = 0, yVel = 0;
			position.y = jumpLocation.y;
			current->SetCurrentFrame(1);
			MIXER.Play(Mixer::SE_Thud);
		}
	}
	//On-the-ground logic...
	else if(jumpState == JS_Ground)
	{
		//Getting up...
		if(GetHealth() > 0)
		{
			if(current->GetCurrentFrame() == 1)
			{
				if(recoveryTimer == 0) {
					recoveryTimer = SDL_GetTicks() + 2000;
				} else if (SDL_GetTicks() > recoveryTimer) {
					current->SetCurrentFrame(2);
					recoveryTimer = SDL_GetTicks() + 500;
				}
			}
			//Half up...
			else if(current->GetCurrentFrame() == 2)
			{
				//full up.. go to idle..
				if(SDL_GetTicks() > recoveryTimer) {
					Stop();
					state = ES_Idle;
					recoveryTimer = 0;
				}
			}
		}
		//Enemy is dead.. 
		//TODO: Garbage collect
		else
		{
			state = ES_Dead;
			MIXER.Play(Mixer::SE_DragonRoar);
		}
	}

	Translate();
	current->Position().x = position.x;
	current->Position().y = position.y;
	current->Update();
}


const float MaxDistX = 50.0f;
const float MaxDistY = 0.0f;
void Enemy::Update()
{
	//Knocked down.. get up or die...
	if(state == ES_KnockedDown)
	{
		HandleKnockedDown();
		return;
	}

	//Recovery (when hit)
	if(state == ES_Hit && SDL_GetTicks() > recoveryTimer)
	{
		Stop();
		state = ES_Idle;
		recoveryTimer = 0;
		hitCount = 0;
	}

	//Chase player
	float distX = position.x - GAME.player->Position().x;
	float distY = position.y - GAME.player->Position().y;
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

		//When close enough, attack
		if(SDL_abs((int)distX) <= (int)MaxDistX 
			&& SDL_abs((int)distY) <= (int)MaxDistY)
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

	//Translate/animate
	Translate(xVel != 0 || yVel != 0 || state == ES_Attacking);
	//Propagate to the underlying currently active sprite
	current->Position().x = position.x;
	current->Position().y = position.y;
	current->Update();
}


void Enemy::Attack()
{
	state = ES_Attacking;
	punchTimer = SDL_GetTicks();
}


void Enemy::Draw(SDL_Renderer* const renderer) const
{
	current->Draw(renderer);
}


Enemy::~Enemy()
{
	current = NULL;
	util::Delete(walkLeft);
	util::Delete(walkRight);
	util::Delete(punchLeft);
	util::Delete(punchRight);
	util::Delete(hitLeft);
	util::Delete(hitRight);
	util::Delete(fallLeft);
	util::Delete(fallRight);
	logPrintf("Enemy object released");
}


void Enemy::SetDirection(Directions dir)
{
	GameObject::SetDirection(dir);
	if (GetDirection() == Right) current = walkRight;
	else if(GetDirection() == Left) current = walkLeft;
}


void Enemy::Stop()
{
	xVel = yVel = 0;
	current = GetDirection() == Left? walkLeft: walkRight;
	current->SetStill();
}


void Enemy::Translate()
{
	//Handle BG scrolling
	if(GAME.bg->IsScrolling())
	{
		position.x += GAME.bg->GetDirection() == Left?
			-GAME.bg->GetSpeed(): GAME.bg->GetSpeed();
	}

	//Translate
	position.x += xVel;
	position.y += yVel;

	//Z rules dont apply to jumping
	if(jumpState == JS_Ground)
	{
		position.y = position.y < GAME.MoveBounds.top()? GAME.MoveBounds.top(): position.y;
		AdjustZToGameDepth();
	}

	//logPrintf("Enemy Translate: Pos {%d, %d}", (int)position.x, (int)position.y);
}


void Enemy::Translate(bool anim)
{
	current->SetAnimation(anim);
	Translate();
}


