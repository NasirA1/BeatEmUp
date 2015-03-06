#pragma once
#include <SDL.h>
#include <vector>
#include "Util.h"


using namespace std;
using namespace util;

enum Directions
{
	Left, Right, Up, Down
};


class Game;

class GameObject
{
protected:
	GameObject(int health_ = 1, Directions initialDirection = Right) 
		: health(health_)
		, direction(initialDirection)
		, xVel(0)
		, yVel(0)
		, angle(0)
	{}

public:
	virtual ~GameObject(){}
	virtual void Update() = 0;
	virtual void Draw(SDL_Renderer* const renderer) const = 0;
	virtual void SetAngle(double theta) { angle = theta; }
	__forceinline virtual void SetDirection(Directions dir) { direction = dir; }

	
	//Accessors
	__forceinline RectF& Position() { return position; }
	__forceinline Directions GetDirection() const { return direction; }
	__forceinline double GetAngle() const { return angle; }
	__forceinline bool IsDead() const { return health <= 0; }
	__forceinline float XVel() const { return xVel; }
	__forceinline float YVel() const { return yVel; }
	__forceinline int GetHealth() const { return health; }

	
	//Mutators
	__forceinline void SetHealth(int value) { health = value < 0? 0: value; }


	//Rectangle-based collision detection
	bool CollidesWith(const GameObject* const other) const;
	void AdjustZToGameDepth();

	//Functor for sorting Game objects by depth (z axis) 
	friend struct GameObjectSortByDepth;


protected:
	RectF position;
	float xVel, yVel;
	int health;

private:
	Directions direction;
	double angle; //rotation angle
};


class GameObjectList : public vector<GameObject*>
{
public:
	void Update(bool removeDead = true);
	void Draw(SDL_Renderer* const renderer) const;
	~GameObjectList();
};


//Functor for sorting Game objects by depth (z axis) 
struct GameObjectSortByDepth {
	__forceinline bool operator()(GameObject* const a, GameObject* const b) const {
		return a->position.z < b->position.z;
	}
};


#pragma region Test Derived Class
#if 0

class Ball : public GameObject
{
	SDL_Rect r;
	SDL_Colour c;
	int dx, dy;

public:
	Ball(SDL_Point loc, size_t size, Uint8 _r, Uint8 _g, Uint8 _b)
	{
		r.x = loc.x;
		r.y = loc.y;
		r.w = size;
		r.h = size;
		dx = dy = 1;
		c.r = _r;
		c.g = _g;
		c.b = _b;
		c.a = 0xff;
	}

	virtual ~Ball()
	{
		logPrintf("Ball object destroyed");
	}

	void Update() override
	{
		int velocity = Random::Instance().Next(1, 10);

		if (r.x >= 800 - r.w) dx = -velocity;
		if (r.x <= 0) dx = velocity;
		if (r.y >= 600 - r.h) dy = -velocity;
		if (r.y <= 0) dy = velocity;

		r.x += dx;
		r.y += dy;
		SDL_Delay(1);
	}

	void Draw(SDL_Renderer* const renderer) const override
	{
		SDL_SetRenderDrawColor( renderer, c.r, c.g, c.b, c.a );
		SDL_RenderFillRect( renderer, &r );
	}

	static void AddRandomBalls(GameObjectList& list, size_t count)
	{
		for (int i = 0; i < count; ++i)
		{
			int diameter = Random::Instance().Next(10, 100);
			int r = Random::Instance().Next(0, 255);
			int g = Random::Instance().Next(0, 255);
			int b = Random::Instance().Next(0, 255);
			list.push_back(new Ball(GetRandomLocation(), diameter, r, g, b));
		}
	}

	static SDL_Point GetRandomLocation()
	{
		SDL_Point p = {Random::Instance().Next(0, 800), Random::Instance().Next(0, 600)};
		return p;
	}
};

#endif
#pragma endregion


