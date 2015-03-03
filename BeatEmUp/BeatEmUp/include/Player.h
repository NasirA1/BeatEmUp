#pragma once
#include "Sprite.h"


class Player : public GameObject
{
public:
	Player(SDL_Renderer* const renderer);
	virtual void Update(Game& world) override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual ~Player();
	virtual void SetDirection(Directions dir) override;


enum JumpState
{
	JS_Ground,
	JS_Jumped,
	JS_Landing
};

public:
	void Stop();
	void GoUp(Game& world);
	void GoDown(Game& world);
	void GoRight(Game& world);
	void GoLeft(Game& world);
	
	__forceinline bool isMoving() const 
	{ 
		return current->IsAnimationRunning(); 
	}
	
	void Jump(float xForce, float yForce);

private:
	void Translate(bool anim = true);


private:
	Sprite* walkRight;
	Sprite* walkLeft;
	Sprite* current;

	JumpState jumpState;
	VectF jumpLocation;
	static const float Gravity;
	static const int JumpHeight;
	static const float WalkVel;
};
