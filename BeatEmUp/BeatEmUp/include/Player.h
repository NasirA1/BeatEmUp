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

enum PState
{
	PS_Stance,
	PS_Walking,
	PS_Jumping,
	PS_Punching,
	PS_Kicking,
	PS_Hit,
	PS_Recovering
};

public:
	void Stop();
	void GoUp();
	void GoDown();
	void GoRight();
	void GoLeft();
	void Jump(float xForce, float yForce);
	void Punch();
	
	__forceinline bool isMoving() const { return xVel == 0.0f && yVel == 0.0f; }
	__forceinline PState GetState() const { return pState; }
	

private:
	void Translate(bool anim = true);


private:
	Sprite* walkRight;
	Sprite* walkLeft;
	Sprite* stanceRight;
	Sprite* stanceLeft;
	Sprite* current;
	Sprite* punchLeft;
	Sprite* punchRight;

	//player state
	PState pState;
	Uint32 punchTimeout;

	//jumping
	JumpState jumpState;
	VectF jumpLocation;
	static const float Gravity;
	static const int JumpHeight;
};
