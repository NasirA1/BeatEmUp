#pragma once
#include <SDL.h>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include "Util.h"


using namespace std;
using namespace util;



enum class Direction
{
	Left, Right, Up, Down
};


enum class JumpState
{
	Ground,
	Jumped,
	Landing
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

	typedef void(*Deleter) (GameObject*);
	using ptr = std::unique_ptr<GameObject, Deleter>;


protected:
	GameObject(const string& name_, Type type_, int health_ = 1, Direction initialDirection = Direction::Right, float speed_ = 1.0f)
		: name(name_) 
		, type(type_)
		, health(health_)
		, direction(initialDirection)
		, xVel(0.0f)
		, yVel(0.0f)
		, angle(0.0000)
		, speedX(speed_)
		, speedY(speed_ / 2.0f)
	{}

public:
	virtual ~GameObject() = default;
	virtual void Update() = 0;
	virtual void Draw(SDL_Renderer& renderer) const = 0;
	virtual void SetAngle(double theta) { angle = theta; }
	__forceinline virtual void SetDirection(Direction dir) { direction = dir; }

	
	//Accessors
	__forceinline RectF& Position() { return position; }
	__forceinline Direction GetDirection() const { return direction; }
	__forceinline double GetAngle() const { return angle; }
	__forceinline float XVel() const { return xVel; }
	__forceinline float YVel() const { return yVel; }
	__forceinline float GetSpeedX() const { return speedX; }
	__forceinline float GetSpeedY() const { return speedY; }
	__forceinline int GetHealth() const { return health; }
	__forceinline Type GetType() const { return type; }
	__forceinline string GetName() const { return name; }


	//Mutators
	__forceinline void SetHealth(int value) { health = value; }
	__forceinline void SetSpeed(float spx, float spy) { speedX = spx; speedY = spy; }


	//Rectangle-based collision detection
	bool CollidedWith(const GameObject& other, const int penThresholdX = 25, const int penThresholdY = 25, const int penThresholdZ = 25) const;
	bool CollidedWith(const RectF& other, const int penThresholdX = 25, const int penThresholdY = 25, const int penThresholdZ = 25) const;
	void AdjustZToGameDepth();

	
	template<class GameObjectType>
	GameObjectType* GetNearestNeighbour(const vector<GameObjectType*>& neighbours) const
	{
		for(const auto neighbour : neighbours)
		{
			if( neighbour != this &&
				(position.right() >= neighbour->position.left()) &&
				(position.left() <= neighbour->position.right())
			)
			{
				logPrintf("%s: %s in range", this->GetName().c_str(), neighbour->GetName().c_str());
				return neighbour;
			}
		}

		return nullptr;
	}


	//Functor for sorting Game objects by depth (z axis)
	//Used for painting
	friend struct GameObjectSortByDepth;

protected:
	RectF position;
	float xVel, yVel;
	float speedX, speedY;

private:
	string name;
	Direction direction;
	double angle; //rotation angle
	int health;
	Type type;
};


//Custom deleter lambdas
namespace GameObjectDeleters
{
	static auto Delete = [](GameObject* obj) { delete obj; };
	static auto NoDelete = [](GameObject*) { };
}



//Functor for sorting Game objects by depth (z axis) 
struct GameObjectSortByDepth {
	__forceinline bool operator()(const GameObject::ptr& a, const GameObject::ptr& b) const {
		return a->position.z < b->position.z;
	}
};



//Container for all the drawable game objects
//Owned objects memory is managed automatically
//Non-owned objects can also be added
struct World
{
	World(const size_t initialCapacity = 50)
	{
		gameObjects_.reserve(initialCapacity);
	}


	size_t Count() const { return gameObjects_.size(); }


	//Non-owner - delete will not be called on the object being added
	void AddGameObject(const GameObject& object)
	{
		gameObjects_.emplace_back(const_cast<GameObject*>(&object), GameObjectDeleters::NoDelete);
	}



	//Owner - delete will be called on the object being created/added
	//if successful, a pointer to the created object will be returned
	//otherwise, nullptr will be returned
	template<class T>
	T* AddGameObject()
	{
		try
		{
			GameObject::ptr object(new T, GameObjectDeleters::Delete);
			GameObject* p = object.get();
			if (p) gameObjects_.push_back(std::move(object));
			return p;
		}
		catch (...)
		{
			return nullptr;
		}
	}


	//Owner - delete will be called on the object being created/added
	//if successful, a pointer to the created object will be returned
	//otherwise, nullptr will be returned
	template <class T, class... Args>
	T* AddGameObject(Args&&... args)
	{
		try
		{
			GameObject::ptr object(new T(std::forward<Args>(args)...), GameObjectDeleters::Delete);
			T* p = dynamic_cast<T*>(object.get());
			if (p) gameObjects_.push_back(std::move(object));
			return p;
		}
		catch (...)
		{
			return nullptr;
		}
	}


	void Draw(SDL_Renderer& renderer)
	{
		//Sort by depth, then draw
		std::sort(gameObjects_.begin(), gameObjects_.end(), GameObjectSortByDepth());
		std::for_each(gameObjects_.begin(), gameObjects_.end(), [&](const auto& obj) { obj->Draw(renderer); });
	}


	void Update()
	{
		for (auto& object : gameObjects_)
			object->Update();
	}


private:
	vector<GameObject::ptr> gameObjects_;
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

	void Draw(SDL_Renderer& renderer) const override
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

