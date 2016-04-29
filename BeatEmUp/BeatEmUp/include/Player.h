#pragma once
#include "Sprite.h"
#include <memory>

class Player : public GameObject
{
public:
	Player(SDL_Renderer* const renderer);
	virtual ~Player();
	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual void SetDirection(Direction dir) override;
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
	void Kick();
	void OnHit(Uint8 damage = 1);
	
	__forceinline bool isMoving() const { return !(xVel == 0.0f && yVel == 0.0f); }
	__forceinline bool IsDead() const { return pState == PS_Dead; }
	__forceinline PState GetState() const { return pState; }
	__forceinline bool IsPunching(Direction dir) { return pState == PS_Punching && GetDirection() == dir;}
	__forceinline bool IsKicking(Direction dir) { return pState == PS_Kicking && GetDirection() == dir;}


private:
	void Translate(bool anim);
	void HandleJump();
	void OnPunchSprite(const Sprite* const sender, const Sprite::FramePlayedEventArgs* const e);
	void OnKickSprite(const Sprite* const sender, const Sprite::FramePlayedEventArgs* const e);

public:
	void KnockedDown();
	bool CantMove() const;
	bool IsDown() const;

private:
	void OnKnockDown();

private:
	Sprite* walkRight;
	Sprite* walkLeft;
	Sprite* idleRight;
	Sprite* idleLeft;
	Sprite* current;
	Sprite* punchLeft;
	Sprite* punchRight;
	Sprite* hitLeft;
	Sprite* hitRight;
	Sprite* fallLeft;
	Sprite* fallRight;
	Sprite* kickLeft;
	Sprite* kickRight;

	//player state
	PState pState;
	Uint32 punchTimeout;
	Uint32 kickTimeout;
	Uint32 recoveryTimer;
	Uint8 hitCount;
  const Uint8 KnockDownHitCount;
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
