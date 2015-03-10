#pragma once
#include "Sprite.h"


class Player : public GameObject
{
public:
	Player(SDL_Renderer* const renderer);
	virtual ~Player();
	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual void SetDirection(Directions dir) override;
	virtual void SetAngle(double theta) override;

enum JumpState
{
	JS_Ground,
	JS_Jumped,
	JS_Landing
};

public:
	void Stop();
	void GoUp();
	void GoDown();
	void GoRight();
	void GoLeft();
	
	__forceinline bool isMoving() const 
	{ 
		return xVel == 0.0f && yVel == 0.0f; //current->IsAnimationRunning(); 
	}
	
	void Jump(float xForce, float yForce);

private:
	void Translate(bool anim = true);


private:
	Sprite* walkRight;
	Sprite* walkLeft;
	Sprite* stanceRight;
	Sprite* stanceLeft;
	Sprite* current;

	JumpState jumpState;
	VectF jumpLocation;

	static const float Gravity;
	static const int JumpHeight;
};
