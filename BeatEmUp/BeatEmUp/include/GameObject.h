#pragma once
#include <SDL.h>
#include <vector>
#include <string>
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
public:
	enum Type
	{
		GT_Background,
		GT_Sprite,
		GT_Object,
		GT_Player,
		GT_Enemy
	};

protected:
	GameObject(const string& name_, Type type_, int health_ = 1, Directions initialDirection = Right, float speed_ = 1.0f)
		: name(name_) 
		, type(type_)
		, health(health_)
		, direction(initialDirection)
		, xVel(0.0f)
		, yVel(0.0f)
		, angle(0.0000)
		, speedX(speed_)
		, speedY(speed_ / 2.0f)
		, gc(false)
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
	__forceinline float XVel() const { return xVel; }
	__forceinline float YVel() const { return yVel; }
	__forceinline float GetSpeedX() const { return speedX; }
	__forceinline float GetSpeedY() const { return speedY; }
	__forceinline int GetHealth() const { return health; }
	__forceinline Type GetType() const { return type; }
	__forceinline string GetName() const { return name; }
	__forceinline bool IsMarkedForGC() const { return gc; }
	__forceinline void MarkForGC() { gc = true; }
	
	//Mutators
	__forceinline void SetHealth(int value) { health = value; }
	__forceinline void SetSpeed(float spx, float spy) { speedX = spx; speedY = spy; }


	//Rectangle-based collision detection
	bool CollidedWith(const GameObject* const other, const int penThresholdX = 25
		, const int penThresholdY = 25, const int penThresholdZ = 25) const;
	void AdjustZToGameDepth();


	//Functor for sorting Game objects by depth (z axis)
	//Used for painting
	friend struct GameObjectSortByDepth;

protected:
	RectF position;
	float xVel, yVel;
	float speedX, speedY;

private:
	string name;
	Directions direction;
	double angle; //rotation angle
	int health;
	Type type;
	bool gc;
};


class GameObjectList : public vector<GameObject*>
{
public:
	void Update();
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
		: GameObject("", GT_Object)
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

