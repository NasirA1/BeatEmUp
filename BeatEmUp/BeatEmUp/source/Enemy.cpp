#include "Enemy.h"
#include "Game.h"
#include "Player.h"


const float Enemy::Gravity(2.0f);
const int Enemy::JumpHeight(50);


Enemy::Enemy(SDL_Renderer& renderer
	, Sprite::ptr idleLeftSprite, Sprite::ptr idleRightSprite
	, Sprite::ptr walkLeftSprite, Sprite::ptr walkRightSprite
	, Sprite::ptr attackLeftSprite, Sprite::ptr attackRightSprite
	, Sprite::ptr hitLeftSprite, Sprite::ptr hitRightSprite
	, Sprite::ptr fallLeftSprite, Sprite::ptr fallRightSprite
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
	: GameObject(name_, GT_Enemy, health, Direction::Left, speed_)
	, idleLeft(std::move(idleLeftSprite))
	, idleRight(std::move(idleRightSprite))
	, walkLeft(std::move(walkLeftSprite))
	, walkRight(std::move(walkRightSprite))
	, attackLeft(std::move(attackLeftSprite))
	, attackRight(std::move(attackRightSprite))
	, hitLeft(std::move(hitLeftSprite))
	, hitRight(std::move(hitRightSprite))
	, fallLeft(std::move(fallLeftSprite))
	, fallRight(std::move(fallRightSprite))
	, current(walkLeft.get())
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
	current = nullptr;
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

	case ES_Visiting:
		OnVisit(ES_Chasing);
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


void Enemy::Draw(SDL_Renderer& renderer) const
{
	current->Draw(renderer);
}


void Enemy::Walk(Direction dir)
{
	GameObject::SetDirection(dir);
	if (GetDirection() == Direction::Right) current = walkRight.get();
	else if(GetDirection() == Direction::Left) current = walkLeft.get();
}


void Enemy::Visit()
{
	if(visitPath.empty()) return;

	if(visitPath.front().x < (int)position.x) Walk(Direction::Left);
	else Walk(Direction::Right);
	state = ES_Visiting;
}


//destState = State of the object once it has visited destination node
void Enemy::OnVisit(EState destState)
{
	int distX = (int)position.x - visitPath.front().x;
	int distY = (int)position.y - visitPath.front().y;
	//logPrintf("(int)position.x %d visitPath.front().x %d distX %d distY %d", 
	//	(int)position.x, visitPath.front().x, distX, distY);
	if(distX > 0) {
		Walk(Direction::Left);
		xVel = -speedX;
		//zero-out rounding error(s)
		if(distX == 1) xVel = 0;
	}
	else if(distX < 0) {
		Walk(Direction::Right);
		xVel = speedX;
		//zero-out rounding error(s)
		if(distX == -1) xVel = 0;
	}
	else {
		xVel = 0.0f;
	}

	if(distY > 0) {
		yVel = -speedY;
	}
	else if(distY < 0) {
		yVel = speedX;
	}
	else
	{
		yVel = 0.0f;
	}

	if(xVel == 0.0f && yVel == 0.0f) {
		visitPath.pop();
		//all nodes visited
		if(visitPath.empty()) {
			state = destState;
		}
	}
}


void Enemy::Stop()
{
	xVel = yVel = 0;
	current = GetDirection() == Direction::Left? idleLeft.get(): idleRight.get();
}


void Enemy::Translate()
{
	//Handle BG scrolling
	if(GAME.bg->IsScrolling())
	{
		position.x += GAME.bg->GetDirection() == Direction::Left?
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
		current = GetDirection() == Direction::Left? hitLeft.get(): hitRight.get();
		state = ES_Hit;
		hitCount++;
		SetHealth(GetHealth() - 1);
	
		if(GetHealth() > 0 && hitCount < KnockDownHitCount){
			recoveryTimer = SDL_GetTicks() + 400;
		}
		else
		{
			hitCount = 0;
			current = GetDirection() == Direction::Left? fallLeft.get(): fallRight.get();
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
	xVel = GetDirection() == Direction::Right? -xAccel: xAccel;
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
			xVel += GetDirection() == Direction::Right? 0.15f: -0.15f;
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
		else
		{
			state = ES_Dead;
			MIXER.Play(Mixer::SE_DragonRoar);
			auto it = find(GAME.enemies.begin(), GAME.enemies.end(), this);
			logPrintf("%s[%x] is dead", GetName().c_str(), (unsigned int)*it);
			GAME.enemies.erase(it);
		}
	}

	Translate();
	current->Position().x = position.x;
	current->Position().y = position.y;
	current->Update();
}


void Enemy::VisitAltPlayer()
{
	const bool PlayerOnTheLeft = GAME.player->Position().x < position.x;
	const int MinDistY = __WHEEL.Next(10, 50);
	const static int MinDistX = 100;
//logPrintf("PlayerOnTheLeft? %d", PlayerOnTheLeft);
	int y = 0;
	if(__WHEEL.TakeAChance())
		y = ((int)position.bottom() + MinDistY <= (int)GAME.MoveBounds.bottom()? (int)position.y + MinDistY: (int)GAME.MoveBounds.bottom());
	else
		y = ((int)position.top() - MinDistY >= (int)GAME.MoveBounds.top()? (int)position.y - MinDistY: (int)GAME.MoveBounds.top());

	SDL_Point p1 = {(int)GAME.player->Position().x + (PlayerOnTheLeft? -MinDistX: MinDistX), y };
	SDL_Point p2 = {(int)p1.x + (PlayerOnTheLeft? -MinDistY: MinDistY), (int)GAME.player->Position().y};
	visitPath.push(p1);
	visitPath.push(p2);
	Visit();
}


void Enemy::OnPatrol()
{
	if(!GAME.player->IsDead())
	{
		float distanceToPlayer = util::GetDistance(GAME.player->Position(), position);
		//logPrintf("vision %f dist %f", vision, distanceToPlayer);
		if(distanceToPlayer <= vision) {
			if(__WHEEL.TakeAChance()) /*50-50 chance*/ {
				//direct (straight-line path to player)
				state = ES_Chasing; 
			}
			else{
				//Alternative (u-turn) path to player
				VisitAltPlayer();
				return;
			}
		}
	}

	//logPrintf("dist %f", patrolVecX)
	if(patrolVecX >= patrolRange) Walk(Direction::Left), patrolVecX = 0;
	else if(patrolVecX < -patrolRange) Walk(Direction::Right), patrolVecX = 0;
	if(GetDirection() == Direction::Left) xVel = -speedX;
	else xVel = speedX;
	patrolVecX += xVel;
}


void Enemy::OnChase()
{
	float distX = position.x - GAME.player->Position().x;
	float distY = position.bottom() - (GAME.player->Position().bottom() - 10);

	if(GAME.player->GetState() != Player::PS_Jumping) {
		if(distY > MinDistY) yVel = -speedY;
		else if(distY < -MinDistY) yVel = speedY;
	}
	else { 
		yVel = 0.0f;
	}

	#pragma region Teleporting enemy
	//teleport
	//if(distX <= 3 * MinDistX)
	//{
	//	Enemy* neighbour = GetNearestNeighbour< Enemy >(GAME.enemies);
	//	if(neighbour && neighbour->IsAttackable()) 
	//	{
	//		if(__WHEEL.TakeAChance())
	//		{
	//			if(GAME.player->Position().x <= position.x)
	//				position.x += -200;
	//			else
	//				position.x += 200;
	//		}
	//	}
	//}
	#pragma endregion


	if(distX > MinDistX) {
		Walk(Direction::Left);
		xVel = -speedX;
	}
	else if(distX < -MinDistX) {
		Walk(Direction::Right);
		xVel = speedX;
	}

	//When close enough, attack
	if(SDL_abs((int)distX) <= (int)(position.left() 
		< GAME.player->Position().left()? MinDistX+10.0f: MinDistX) 
		&& SDL_abs((int)distY) <= (int)MinDistY) 
	{
		Stop();
		Direction myOpposite = GetDirection()==Direction::Left? Direction::Right: Direction::Left;
		if(!GAME.player->IsPunching(myOpposite) && !GAME.player->IsKicking(myOpposite))
			Attack();
		
		if(GAME.player->IsDead()) {
			state = ES_Patrolling;
			current = GetDirection() == Direction::Right? walkRight.get(): walkLeft.get();
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
	current = GetDirection() == Direction::Left? attackLeft.get(): attackRight.get();
	current->Rewind();
}


void Enemy::OnIdle()
{
	if(!idleTimer)
	{
		Stop();
		//idleTimer = SDL_GetTicks() + __WHEEL.Next(1000, 3000);
		idleTimer = SDL_GetTicks() + __WHEEL.Next(100, 1000);
	}
	else
	{
		if(SDL_GetTicks() >= idleTimer)
		{
			state = ES_Chasing;
			Enemy* neighbour = GameObject::GetNearestNeighbour(GAME.enemies); 
			if(neighbour && neighbour->state == ES_Chasing)
			{
				VisitAltPlayer();
			}
			idleTimer = 0;
		}
		else
		{
			//Face player all the time when on idle
			SetDirection(position.x > GAME.player->Position().x? Direction::Left: Direction::Right);
			Stop();
		}
	}
}



Andore::Andore(SDL_Renderer& renderer_, float posX, float posY)
	: Enemy(renderer_, 
		Sprite::FromFile("resources/andore_idleleft.png", renderer_, 84, 115, 10, 0),
		Sprite::FromFile("resources/andore_idleright.png", renderer_, 88, 117, 10, 0), 
		Sprite::FromFile("resources/andore_walkleft.png", renderer_, 88, 117, 10, 0, true),
		Sprite::FromFile("resources/andore_walkright.png", renderer_, 88, 117, 10, 5), 
		Sprite::FromFile("resources/andore_punchleft.png", renderer_, 115, 112, 10, 1),
		Sprite::FromFile("resources/andore_punchright.png", renderer_, 115, 112, 10, 1), 
		Sprite::FromFile("resources/andore_hitleft.png", renderer_, 70, 124, 5, 0), 
		Sprite::FromFile("resources/andore_hitright.png", renderer_, 70, 124, 5, 0), 
		Sprite::FromFile("resources/andore_fallleft.png", renderer_, 150, 120, 1, 0), 
		Sprite::FromFile("resources/andore_fallright.png", renderer_, 150, 120, 1, 0), 
		"Andore", posX, posY, 30, 300, 1.5f, 200.0f, 0.0f, 350.0f, 40.0f, 0.0f)
{
	attackLeft->FramePlayed.attach(*this, &Andore::OnPunchSprite);
	attackRight->FramePlayed.attach(*this, &Andore::OnPunchSprite);
}


void Andore::OnPunchSprite(const Sprite& sender, const Sprite::FramePlayedEventArgs& e)
{
	if(e.FrameIndex == 1)
	{
		if(!GAME.player->IsDown() && CollidedWith(*GAME.player, 0, 0))
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
	attackLeft->FramePlayed.detach(*this, &Andore::OnPunchSprite);
	attackRight->FramePlayed.detach(*this, &Andore::OnPunchSprite);
	logPrintf("Andore released");
}



Axl::Axl(SDL_Renderer& renderer_, float posX, float posY)
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
	attackLeft->FramePlayed.attach(*this, &Axl::OnPunchSprite);
	attackRight->FramePlayed.attach(*this, &Axl::OnPunchSprite);
}


void Axl::OnPunchSprite(const Sprite& sender, const Sprite::FramePlayedEventArgs& e)
{
	if(e.FrameIndex == 1)
	{
		if(!GAME.player->IsDown() && CollidedWith(*GAME.player, 0, 0))
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
	attackLeft->FramePlayed.detach(*this, &Axl::OnPunchSprite);
	attackRight->FramePlayed.detach(*this, &Axl::OnPunchSprite);
	logPrintf("Axl released");
}



Joker::Joker(SDL_Renderer& renderer_, float posX, float posY)
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
	attackLeft->FramePlayed.attach(*this, &Joker::OnStickSprite);
	attackRight->FramePlayed.attach(*this, &Joker::OnStickSprite);
}


Joker::~Joker()
{
	attackLeft->FramePlayed.detach(*this, &Joker::OnStickSprite);
	attackRight->FramePlayed.detach(*this, &Joker::OnStickSprite);
	logPrintf("Joker released");
}


void Joker::OnStickSprite(const Sprite& sender, const Sprite::FramePlayedEventArgs& e)
{
	if(e.FrameIndex == 2)
	{
		if(!GAME.player->IsDown()) 
		{
			bool collision = CollidedWith(*GAME.player, -60, 0, 15);
			if(collision) {
				MIXER.Play(Mixer::SE_PunchHit);
				GAME.player->OnHit();
			}
			else {
				MIXER.Play(Mixer::SE_Punch);
			}
		}
		else {
			MIXER.Play(Mixer::SE_Punch);
		}
	}
}


void Joker::Propagate()
{
	if(state == ES_Attacking) 
	{
		if(GAME.player->Position().x < position.x)
			current->Position().x = position.x - 60;
		else
			current->Position().x = position.x - 0;		
		current->Position().y = position.y - (current->Position().h - idleLeft->Position().h);
	}
	else
	{
		Enemy::Propagate();
	}
}



#pragma region Rock

const float Rock::Range(10.0f * (float)GAME.ClientWidth());


Rock::Rock(const string& file, SDL_Renderer& renderer)
 : GameObject("Rock", GT_Enemy, 1, Direction::Left, 10.0f) 
 , texture(nullptr)
{
	util::SDLSurfaceFromFile surf(file, true);

	texture = unique_ptr2<SDL_Texture>(
		SDL_CreateTextureFromSurface(&renderer, surf.surface)
		, [](auto p) { SDL_DestroyTexture(p); }
	);

	position.x = Range;
	position.w = (float)surf.surface->w;
	position.h = (float)surf.surface->h;
	position.y = (float)GAME.RandomYWithinMoveBounds((int)position.h);
	AdjustZToGameDepth();
}


void Rock::Update()
{
	if(GetDirection() == Direction::Left)
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
		SetDirection(Direction::Left);
	}
	else if(position.x < -Range)
	{
		position.y = (float)GAME.RandomYWithinMoveBounds((int)position.h);
		SetDirection(Direction::Right);
	}

	position.x += xVel;
	AdjustZToGameDepth();


	//collision detection
	if(!GAME.player->IsDown() && CollidedWith(*GAME.player))
	{
		GAME.player->Stop();
		GAME.player->OnHit();
		GAME.player->KnockedDown();
		MIXER.Play(Mixer::SE_Grunt);
	}
}


void Rock::Draw(SDL_Renderer& renderer) const
{
	SDL_Rect nPos;
	util::Convert(position, nPos);
	SDL_RenderCopyEx(&renderer, texture.get(), nullptr, &nPos, GetAngle(), nullptr, SDL_FLIP_NONE);
}


Rock::~Rock()
{
	logPrintf("Rock object released");
}

#pragma endregion

