#include "Enemy.h"
#include "Game.h"
#include "Player.h"


const float Enemy::Gravity(2.0f);
const int Enemy::JumpHeight(50);


Enemy::Enemy(SDL_Renderer* const renderer
	, Sprite* idleLeftSprite, Sprite* idleRightSprite
	, Sprite* walkLeftSprite, Sprite* walkRightSprite
	, Sprite* attackLeftSprite, Sprite* attackRightSprite
	, Sprite* hitLeftSprite, Sprite* hitRightSprite
	, Sprite* fallLeftSprite, Sprite* fallRightSprite
	, const string& name_
	, float posX, float posY
	, int health
	, const Uint32 attackTimeout
	, float speed_
	, float patrolRange_
	, float patrolVecX_
	, float vision_
	, float minDistX
	, float minDistY
)
	: GameObject(name_, GT_Enemy, health, Left, speed_)
	, idleLeft(idleLeftSprite)
	, idleRight(idleRightSprite)
	, walkLeft(walkLeftSprite)
	, walkRight(walkRightSprite)
	, attackLeft(attackLeftSprite)
	, attackRight(attackRightSprite)
	, hitLeft(hitLeftSprite)
	, hitRight(hitRightSprite)
	, fallLeft(fallLeftSprite)
	, fallRight(fallRightSprite)
	, current(walkLeft)
	, state(ES_Patrolling)
	, attackTimer(0)
	, idleTimer(0)
	, recoveryTimer(0)
	, hitCount(0)
	, jumpState(JS_Ground)
	, KnockDownHitCount(3)
	, patrolRange(patrolRange_)
	, patrolVecX(patrolVecX_)
	, vision(vision_)
	, AttackTimeOut(attackTimeout)
	, MinDistX(minDistX)
	, MinDistY(minDistY)
{
	position.x = posX, position.y = posY, position.w = (float)walkLeft->Position().w;
	position.h = (float)walkLeft->Position().h;
	AdjustZToGameDepth();
}


Enemy::~Enemy()
{
	current = NULL;
	util::Delete(idleLeft);
	util::Delete(idleRight);
	util::Delete(walkLeft);
	util::Delete(walkRight);
	util::Delete(attackLeft);
	util::Delete(attackRight);
	util::Delete(hitLeft);
	util::Delete(hitRight);
	util::Delete(fallLeft);
	util::Delete(fallRight);
	logPrintf("Enemy object released");
}


void Enemy::Update()
{
	switch(state)
	{
	case ES_KnockedDown:
		OnKnockDown();
		return;

	//Recovery (when hit)
	case ES_Hit:
		if(SDL_GetTicks() > recoveryTimer)
			OnRecovery();
		break;

	case ES_Patrolling:
		OnPatrol();
		break;

	case ES_Chasing:
		OnChase();
		break;

	case ES_Attacking:
		OnAttack();
		break;

	case ES_Idle:
		OnIdle();
		break;
	}

	//Translate/animate
	Translate(xVel != 0 || yVel != 0 || state == ES_Attacking);
	Propagate();
	current->Update();
}


void Enemy::Draw(SDL_Renderer* const renderer) const
{
	current->Draw(renderer);
}


void Enemy::Walk(Directions dir)
{
	GameObject::SetDirection(dir);
	if (GetDirection() == Right) current = walkRight;
	else if(GetDirection() == Left) current = walkLeft;
}


void Enemy::Stop()
{
	xVel = yVel = 0;
	current = GetDirection() == Left? idleLeft: idleRight;
}


void Enemy::Translate()
{
	//Handle BG scrolling
	if(GAME.bg->IsScrolling())
	{
		position.x += GAME.bg->GetDirection() == Left?
			-GAME.bg->GetSpeedX(): GAME.bg->GetSpeedX();
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


void Enemy::Propagate()
{
	//Propagate to the underlying currently active sprite
	current->Position().x = position.x;
	current->Position().y = position.y;
}


void Enemy::Translate(bool anim)
{
	current->SetAnimation(anim);
	Translate();
}


void Enemy::OnHit()
{
	if(state != ES_Attacking && state != ES_KnockedDown)
	{
		Stop();
		current = GetDirection() == Left? hitLeft: hitRight;
		state = ES_Hit;
		hitCount++;
		SetHealth(GetHealth() - 1);
	
		if(GetHealth() > 0 && hitCount < KnockDownHitCount){
			recoveryTimer = SDL_GetTicks() + 400;
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


void Enemy::Jump(float xAccel, float yAccel)
{
	jumpLocation.x = position.x;
	jumpLocation.y = position.y;
	xVel = GetDirection() == Right? -xAccel: xAccel;
	yVel = -yAccel;
	jumpState = JS_Jumped;
}


void Enemy::OnKnockDown()
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


void Enemy::OnPatrol()
{
	if(!GAME.player->IsDead())
	{
		float distanceToPlayer = util::GetDistance(GAME.player->Position(), position);
		//logPrintf("vision %f dist %f", vision, distanceToPlayer);
		if(distanceToPlayer <= vision) {
			state = ES_Chasing;
		}
	}

	//logPrintf("dist %f", patrolVecX)
	if(patrolVecX >= patrolRange) Walk(Left), patrolVecX = 0;
	else if(patrolVecX < -patrolRange) Walk(Right), patrolVecX = 0;
	if(GetDirection() == Left) xVel = -speedX;
	else xVel = speedX;
	patrolVecX += xVel;
}


void Enemy::OnChase()
{
	float distX = position.x - GAME.player->Position().x;
	float distY = position.bottom() - (GAME.player->Position().bottom() - 10);

	if(GAME.player->GetState() != Player::PS_Jumping)
	{
		if(distY > MinDistY) yVel = -speedY;
		else if(distY < -MinDistY) yVel = speedY;
	}
	else { yVel = 0.0f; }

	if(distX > MinDistX)
	{
		Walk(Left);
		xVel = -speedX;
	}
	else if(distX < -MinDistX)
	{
		Walk(Right);
		xVel = speedX;
	}

	//When close enough, attack
	if(SDL_abs((int)distX) <= (int)(position.left() 
		< GAME.player->Position().left()? 2*MinDistX: MinDistX) 
		&& SDL_abs((int)distY) <= (int)MinDistY)
	{
		Stop();
		Directions myOpposite = GetDirection()==Left? Right: Left;
		if(!GAME.player->IsPunching(myOpposite) && !GAME.player->IsKicking(myOpposite))
			Attack();
		
		if(GAME.player->IsDead())
		{
			state = ES_Patrolling;
			current = GetDirection() == Right? walkRight: walkLeft;
		}
	}
}


void Enemy::OnRecovery()
{
	Stop();
	state = ES_Idle;
	recoveryTimer = 0;
	hitCount = 0;
}


void Enemy::OnAttack()
{
	if(SDL_GetTicks() - attackTimer > AttackTimeOut)
	{
		state = ES_Idle;
		attackTimer = 0;
	}
}


void Enemy::Attack()
{
	state = ES_Attacking;
	attackTimer = SDL_GetTicks();
	current = GetDirection() == Left? attackLeft: attackRight;
	current->Rewind();
}


void Enemy::OnIdle()
{
	if(!idleTimer)
	{
		Stop();
		idleTimer = SDL_GetTicks() + WHEEL_OF_FORTUNE.Next(1000, 3000);
	}
	else
	{
		if(SDL_GetTicks() >= idleTimer)
		{
			state = ES_Chasing;
			idleTimer = 0;
		}
		else
		{
			//Face player all the time when on idle
			SetDirection(position.x > GAME.player->Position().x? Left: Right);
			Stop();
		}
	}
}



Andore::Andore(SDL_Renderer* const renderer_, float posX, float posY)
	: Enemy(renderer_, 
		Sprite::FromFile("resources/andore_idleleft.png", renderer_, 84, 115, 10, 0),
		Sprite::FromFile("resources/andore_idleright.png", renderer_, 88, 117, 10, 0), 
		Sprite::FromFile("resources/andore_walkleft.png", renderer_, 84, 115, 10, 5),
		Sprite::FromFile("resources/andore_walkright.png", renderer_, 88, 117, 10, 5), 
		Sprite::FromFile("resources/andore_punchleft.png", renderer_, 115, 112, 10, 1),
		Sprite::FromFile("resources/andore_punchright.png", renderer_, 115, 112, 10, 1), 
		Sprite::FromFile("resources/andore_hitleft.png", renderer_, 70, 124, 5, 0), 
		Sprite::FromFile("resources/andore_hitright.png", renderer_, 70, 124, 5, 0), 
		Sprite::FromFile("resources/andore_fallleft.png", renderer_, 150, 120, 1, 0), 
		Sprite::FromFile("resources/andore_fallright.png", renderer_, 150, 120, 1, 0), 
		"Andore", posX, posY, 30, 300, 1.0f, 200.0f, 0.0f, 350.0f, 40.0f, 0.0f)
{
	attackLeft->FramePlayed.attach(this, &Andore::OnPunchSprite);
	attackRight->FramePlayed.attach(this, &Andore::OnPunchSprite);
}


void Andore::OnPunchSprite(const Sprite* const sender, const Sprite::FramePlayedEventArgs* const e)
{
	if(e->FrameIndex == 1)
	{
		if(CollidedWith(GAME.player, 0, 0))
		{
			MIXER.Play(Mixer::SE_PunchHit);
			GAME.player->OnHit();
		}
		else
		{
			MIXER.Play(Mixer::SE_Punch);
		}
	}
}


Andore::~Andore()
{
	attackLeft->FramePlayed.detach(this, &Andore::OnPunchSprite);
	attackRight->FramePlayed.detach(this, &Andore::OnPunchSprite);
	logPrintf("Andore released");
}



Axl::Axl(SDL_Renderer* const renderer_, float posX, float posY)
	: Enemy(renderer_, 
		Sprite::FromFile("resources/axl_idleleft.png", renderer_, 85, 112, 10, 0),
		Sprite::FromFile("resources/axl_idleright.png", renderer_, 85, 112, 10, 0), 
		Sprite::FromFile("resources/axl_walkleft.png", renderer_, 85, 112, 10, 0, true),
		Sprite::FromFile("resources/axl_walkright.png", renderer_, 85, 112, 10, 5), 
		Sprite::FromFile("resources/axl_kickleft.png", renderer_, 110, 112, 10, 0, true), 
		Sprite::FromFile("resources/axl_kickright.png", renderer_, 110, 112, 10, 1), 
		Sprite::FromFile("resources/axl_hitleft.png", renderer_, 85, 112, 5, 0), 
		Sprite::FromFile("resources/axl_hitright.png", renderer_, 85, 112, 5, 0), 
		Sprite::FromFile("resources/axl_fallleft.png", renderer_, 150, 120, 1, 0, true), 
		Sprite::FromFile("resources/axl_fallright.png", renderer_, 150, 120, 1, 0), 
		"Axl", posX, posY, 20, 300, 2.0f, 400.0f, 0.0f, 250.0f, 30.0f, 0.0f)
{
	attackLeft->FramePlayed.attach(this, &Axl::OnPunchSprite);
	attackRight->FramePlayed.attach(this, &Axl::OnPunchSprite);
}


void Axl::OnPunchSprite(const Sprite* const sender, const Sprite::FramePlayedEventArgs* const e)
{
	if(e->FrameIndex == 1)
	{
		if(CollidedWith(GAME.player, 0, 0))
		{
			MIXER.Play(Mixer::SE_Kick);
			GAME.player->OnHit();
		}
		else
		{
			MIXER.Play(Mixer::SE_Punch);
		}
	}
}


Axl::~Axl()
{
	attackLeft->FramePlayed.detach(this, &Axl::OnPunchSprite);
	attackRight->FramePlayed.detach(this, &Axl::OnPunchSprite);
	logPrintf("Andore released");
}



Joker::Joker(SDL_Renderer* const renderer_, float posX, float posY)
	: Enemy(renderer_, 
		Sprite::FromFile("resources/joker_idleleft.png", renderer_, 60, 97, 10, 0),
		Sprite::FromFile("resources/joker_idleright.png", renderer_, 60, 97, 10, 0), 
		Sprite::FromFile("resources/joker_walkleft.png", renderer_, 60, 97, 10, 0, true),
		Sprite::FromFile("resources/joker_walkright.png", renderer_, 60, 97, 10, 0), 
		Sprite::FromFile("resources/joker_attackleft.png", renderer_, 130, 130, 10, 3),
		Sprite::FromFile("resources/joker_attackright.png", renderer_, 130, 130, 10, 3), 
		Sprite::FromFile("resources/joker_hitleft.png", renderer_, 50, 90, 5, 0), 
		Sprite::FromFile("resources/joker_hitright.png", renderer_, 50, 90, 5, 0), 
		Sprite::FromFile("resources/joker_fallleft.png", renderer_, 90, 90, 1, 0), 
		Sprite::FromFile("resources/joker_fallright.png", renderer_, 90, 90, 1, 0), 
		"Joker", posX, posY, 10, 550, 1.0f, 200.0f, 0.0f, 250.0f, 90.0f, 0.0f)
{
	attackLeft->FramePlayed.attach(this, &Joker::OnStickSprite);
	attackRight->FramePlayed.attach(this, &Joker::OnStickSprite);
}


Joker::~Joker()
{
	attackLeft->FramePlayed.detach(this, &Joker::OnStickSprite);
	attackRight->FramePlayed.detach(this, &Joker::OnStickSprite);
	logPrintf("Joker released");
}


void Joker::OnStickSprite(const Sprite* const sender, const Sprite::FramePlayedEventArgs* const e)
{
	if(e->FrameIndex == 2)
	{
		if(CollidedWith(GAME.player, -40, 0, 15))
		{
			MIXER.Play(Mixer::SE_PunchHit);
			GAME.player->OnHit();
		}
		else
		{
			MIXER.Play(Mixer::SE_Punch);
		}
	}
}


void Joker::Propagate()
{
	if(state == ES_Attacking) 
	{
		if(GetDirection()==Left)
			current->Position().x = position.x - (current->Position().w - idleLeft->Position().w);
		current->Position().y = position.y - (current->Position().h - idleLeft->Position().h);
	}
	else
	{
		Enemy::Propagate();
	}
}



#pragma region Rock

const float Rock::Range(10.0f * (float)GAME.ClientWidth());


Rock::Rock(const string& file, SDL_Renderer* const renderer)
 : GameObject("Rock", GT_Enemy, 1, Left, 10.0f) 
 , texture(NULL)
{
	util::SDLSurfaceFromFile surf(file, true);
	texture = SDL_CreateTextureFromSurface(renderer, surf.surface);

	position.x = Range;
	position.w = (float)surf.surface->w;
	position.h = (float)surf.surface->h;
	position.y = (float)GAME.RandomYWithinMoveBounds((int)position.h);
	AdjustZToGameDepth();
}


void Rock::Update()
{
	if(GetDirection() == Left)
	{
		SetAngle(GetAngle() - speedX);
		xVel = -speedX;
	}
	else
	{
		SetAngle(GetAngle() + speedX);
		xVel = speedX;
	}

	if(position.x >= Range)
	{
		position.y = (float)GAME.RandomYWithinMoveBounds((int)position.h);
		SetDirection(Left);
	}
	else if(position.x < -Range)
	{
		position.y = (float)GAME.RandomYWithinMoveBounds((int)position.h);
		SetDirection(Right);
	}

	position.x += xVel;
	AdjustZToGameDepth();


	//collision detection
	if(GAME.player->IsAttackable() && CollidedWith(GAME.player))
	{
		GAME.player->Stop();
		GAME.player->OnHit();
		GAME.player->KnockedDown();
		MIXER.Play(Mixer::SE_Grunt);
	}
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

