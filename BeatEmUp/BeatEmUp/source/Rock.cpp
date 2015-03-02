#include "Rock.h"


Rock::Rock(const string& file, SDL_Renderer* const renderer)
 : GameObject() 
 , texture(NULL)
{
	util::SDLSurfaceFromFile surf(file, true);
	texture = SDL_CreateTextureFromSurface(renderer, surf.surface);
	
	position.x = 800 * 5;
	position.y = 410;
	position.w = surf.surface->w;
	position.h = surf.surface->h;
}


void Rock::Update()
{
	angle-=10;
	position.x-=10;

	//remove when out of view
	if(position.right() < 0)
	{
		SetHealth(0);
		//position.x = 900;
	}
}


void Rock::Draw(SDL_Renderer* const renderer) const
{
	SDL_Rect nPos;
	util::Convert(position, nPos);
	SDL_RenderCopyEx(renderer, texture, NULL, &nPos, angle, NULL, SDL_FLIP_NONE);
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



Knight::Knight(SDL_Renderer* const renderer)
	: GameObject(1, Left)	
	, walkRight(NULL)
	, walkLeft(NULL)
	, current(NULL)
{
	position.x  = 1000, position.y = 400, position.w = 128, position.h = 128; 
	moveBounds.x = 0, moveBounds.y = 370;
	moveBounds.w = 800 - position.w, moveBounds.h = position.h;

	walkRight = Sprite::FromFile("resources/knightwalk_right.png", renderer, 128, 128, 4, 15);
	walkLeft = Sprite::FromFile("resources/knightwalk_left.png", renderer, 128, 128, 4, 3);

	if(walkRight) SetDirection(Left);
}



void Knight::Update()
{
	//xVel = -1;
	//Translate(true);

	//Propagate to the underlying currently active sprite
	current->Pos().x = position.x;
	current->Pos().y = position.y;
	current->Update();
}


void Knight::Draw(SDL_Renderer* const renderer) const
{
	current->Draw(renderer);
}


Knight::~Knight()
{
	util::Delete(walkRight);
	util::Delete(walkLeft);

	logPrintf("Knight object released");
}


void Knight::SetDirection(Directions dir)
{
	GameObject::SetDirection(dir);
	if (GetDirection() == Right) current = walkRight;
	else if(GetDirection() == Left) current = walkLeft;
}


void Knight::Stop()
{
	//position.x -= xVel;
	//position.y -= yVel;
	//xVel = yVel = 0;
	//current->SetStill();
}

int distance2(int a, int b)
{
	return a - b;
}

void Knight::Follow(const Player& p)
{
	int distX = distance2(position.x, p.Position().x);
	int distY = distance2(position.y, p.Position().y);
	

	if( distX > 50)
	{
		xVel=-5;
		SetDirection(Left);
		Translate(true);
	}
	else if(distX < -50)
	{
		xVel=5;
		SetDirection(Right);
		Translate(true);
	}
	else
	{
		xVel=0;
		if(position.x < p.Position().x) SetDirection(Right);
		else SetDirection(Left);
		Translate(p.isMoving());
	}

	//if( distY > 20)
	//{
	//	yVel=-1;
	//	Translate(true);
	//}
	//else if(distY <= -20)
	//{
	//	yVel=1;
	//	Translate(true);
	//}
	//else
	//{
	//	yVel=0;
	//	Translate(true);
	//}
}


void Knight::Translate(bool anim)
{
	current->SetAnimation(anim);
	position.x += xVel;
	position.y += yVel;
	//logPrintf("Translate: Pos {%d, %d}", position.x, position.y); 
}

