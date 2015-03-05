#include "Player.h"
#include "Util.h"
#include "Game.h"
#include "Rock.h"


const float Player::Gravity(2.0f);
const int Player::JumpHeight(50);
const float Player::WalkVel(0.5f);


Player::Player(SDL_Renderer* const renderer)
	: GameObject(10, Right)	
	, walkRight(NULL)
	, walkLeft(NULL)
	, current(NULL)
	, jumpState(JS_Ground)
{
	position.x  = 100.0f , position.w = 76.0f, position.h = 120.0f;
	position.y = (float)GAME.MidSectionY((int)position.h);
	walkRight = Sprite::FromFile("resources/walkright.png", renderer, 76, 120, 5, 1, 0xFF, 0x40, 0x40);
	walkLeft = Sprite::FromFile("resources/walkleft.png", renderer, 76, 120, 5, 1, 0xFF, 0x40, 0x40);
	if(walkRight) SetDirection(Right);
}



void Player::Update()
{
	//Jump rotation...
	if(jumpState == JS_Jumped || jumpState == JS_Landing)
			SetAngle(GetAngle() + (GetDirection()==Right? 13: -13));
	else SetAngle(0);

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
			xVel += GetDirection() == Right? 0.15f: -0.15f;
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
	current->Position().x = position.x;
	current->Position().y = position.y;
	current->Update();

	//Collision detection
	if(CollidesWith(GAME.rock))
	{
		//got hit by rock!
		Jump(0, 30);
	}
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


void Player::SetAngle(double theta)
{
	GameObject::SetAngle(theta);
	current->SetAngle(theta);
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

	if (position.y >= GAME.MoveBounds.y) 
		yVel = -WalkVel;
	else 
		yVel = 0;
	Translate();
}


void Player::GoDown()
{
	if(jumpState != JS_Ground) return;
	
	if (position.y <= GAME.MoveBounds.bottom()) 
		yVel = WalkVel;
	else 
		yVel = 0;
	Translate();        
}


void Player::GoRight()
{
	if(jumpState != JS_Ground) return;

	if (position.x <= GAME.MoveBounds.right() - position.w) 
		xVel = WalkVel;
	else 
		xVel = 0;

	SetDirection(Right);
	Translate();
}


void Player::GoLeft()
{
	if(jumpState != JS_Ground) return;
	
	if (position.x >= GAME.MoveBounds.x) 
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
