#include "Player.h"
#include "Util.h"



const float Player::Gravity(2.0f);
const int Player::JumpHeight(50);
const float Player::WalkVel(0.5f);


Player::Player(SDL_Renderer* const renderer)
	: GameObject(10, Right)	
	, walkRight(NULL)
	, walkLeft(NULL)
	, current(NULL)
	, xVel(0)
	, yVel(0)
	, jumpState(JS_Ground)
{
	position.x  = 100, position.y = 400, position.w = 76, position.h = 120; 
	moveBounds.x = 0, moveBounds.y = 370;
	moveBounds.w = 800 - position.w, moveBounds.h = position.h;

	walkRight = Sprite::FromFile("resources/walkright.png", renderer, 76, 120, 5, 1);
	walkLeft = Sprite::FromFile("resources/walkleft.png", renderer, 76, 120, 5, 1);

	if(walkRight) SetDirection(Right);
}



void Player::Update()
{
	//Jump start..
	//Shoot up (y) and sway horizontally a bit (x)
	if(jumpState == JS_Jumped)
	{
		yVel += Gravity/(float)JumpHeight;
		if(position.y > jumpLocation.y - JumpHeight) 
			Translate(false);
		else 
			jumpState = JS_Landing;
	}

	//Landing (in the air)..
	else if(jumpState == JS_Landing)
	{
		//Not landed yet..
		if(position.y < jumpLocation.y)
		{
			yVel += Gravity;
			xVel += GetDirection() == Right? 0.1f: -0.1f;
			Translate(false);
		}
		//On the ground now...
		else 
		{
			jumpState = JS_Ground;
			xVel = 0, yVel = 0;
			position.y = jumpLocation.y;
			Translate(false);
		}
	}

	//Propagate to the underlying currently active sprite
	current->Pos().x = position.x;
	current->Pos().y = position.y;
	current->Update();
}


void Player::Draw(SDL_Renderer* const renderer) const
{
	current->Draw(renderer);
}


Player::~Player()
{
	util::Delete(walkRight);
	util::Delete(walkLeft);

	logPrintf("Player object released");
}


void Player::SetDirection(Directions dir)
{
	GameObject::SetDirection(dir);
	if (GetDirection() == Right) current = walkRight;
	else if(GetDirection() == Left) current = walkLeft;
}


void Player::Jump(float xForce, float yForce)
{
	//Can only jump whilst on the ground
	if(jumpState != JS_Ground) return;

	jumpLocation.x = position.x;
	jumpLocation.y = position.y;
	xVel = GetDirection() == Right? xForce: -xForce;
	yVel = -yForce;
	jumpState = JS_Jumped;
}


void Player::Stop()
{
	position.x -= xVel;
	position.y -= yVel;
	xVel = yVel = 0;
	current->SetStill();
}


void Player::GoUp()
{
	if(jumpState != JS_Ground) return;

	if (position.y >= moveBounds.y) 
		yVel = -WalkVel;
	else 
		yVel = 0;
	Translate();
}


void Player::GoDown()
{
	if(jumpState != JS_Ground) return;
	
	if (position.y <= moveBounds.y + moveBounds.h) 
		yVel = WalkVel;
	else 
		yVel = 0;
	Translate();        
}


void Player::GoRight()
{
	if(jumpState != JS_Ground) return;

	if (position.x <= moveBounds.x + moveBounds.w) 
		xVel = WalkVel;
	else 
		xVel = 0;

	SetDirection(Right);
	Translate();
}


void Player::GoLeft()
{
	if(jumpState != JS_Ground) return;
	
	if (position.x >= moveBounds.x) 
		xVel = -WalkVel;
	else 
		xVel = 0;

	SetDirection(Left);
	Translate();
}


void Player::Translate(bool anim)
{
	current->SetAnimation(anim);
	position.x += xVel;
	position.y += yVel;
	//logPrintf("Translate: Pos {%d, %d}", position.x, position.y); 
}

