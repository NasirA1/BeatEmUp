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


enum PState
{
	PS_Idle,
	PS_Walking,
	PS_Jumping,
	PS_Punching,
	PS_Kicking,
	PS_Hit,
	PS_KnockedDown,
	PS_Dead
};

public:
	void Stop();
	void GoUp();
	void GoDown();
	void GoRight();
	void GoLeft();
	void Jump();
	void Jump(float xAccel, float yAccel);
	void Punch();
	void OnEnemyAttack();
	
	__forceinline bool isMoving() const { return !(xVel == 0.0f && yVel == 0.0f); }
	__forceinline PState GetState() const { return pState; }
	

private:
	void Translate(bool anim);
	void PunchSprites_FramePlayed(const Sprite* const sender, const Sprite::FramePlayedEventArgs* const e);
	void OnKnockDown();

private:
	Sprite* walkRight;
	Sprite* walkLeft;
	Sprite* stanceRight;
	Sprite* stanceLeft;
	Sprite* current;
	Sprite* punchLeft;
	Sprite* punchRight;
	Sprite* hitLeft;
	Sprite* hitRight;
	Sprite* fallLeft;
	Sprite* fallRight;

	//player state
	PState pState;
	Uint32 punchTimeout;
	Uint32 recoveryTimer;
	Uint8 hitCount;

	//jumping
	enum JumpState
	{
		JS_Ground,
		JS_Jumped,
		JS_Landing
	};
	JumpState jumpState;
	VectF jumpLocation;
	static const float Gravity;
	static const int JumpHeight;
};
