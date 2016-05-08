#pragma once
#include "Sprite.h"



enum class PlayerState
{
	Idle,
	Walking,
	Jumping,
	Punching,
	Kicking,
	Hit,
	KnockedDown,
	Dead
};


class Player : public GameObject
{
public:
	Player(SDL_Renderer& renderer);
	virtual ~Player();
	virtual void Update() override;
	virtual void Draw(SDL_Renderer& renderer) const override;
	virtual void SetDirection(Direction dir) override;
	virtual void SetAngle(double theta) override;

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
	void KnockedDown();
	bool CantMove() const;
	bool IsDown() const;

	__forceinline bool isMoving() const { return !(xVel == 0.0f && yVel == 0.0f); }
	__forceinline bool IsDead() const { return pState == PlayerState::Dead; }
	__forceinline PlayerState GetState() const { return pState; }
	__forceinline bool IsPunching(Direction dir) { return pState == PlayerState::Punching && GetDirection() == dir;}
	__forceinline bool IsKicking(Direction dir) { return pState == PlayerState::Kicking && GetDirection() == dir;}


private:
	void Translate(bool anim);
	void HandleJump();
	void OnPunchSprite(const Sprite& sender, const Sprite::FramePlayedEventArgs& e);
	void OnKickSprite(const Sprite& sender, const Sprite::FramePlayedEventArgs& e);
	void OnKnockDown();


//fields
private:
	Sprite::ptr walkRight;
	Sprite::ptr walkLeft;
	Sprite::ptr idleRight;
	Sprite::ptr idleLeft;
	Sprite::ptr punchLeft;
	Sprite::ptr punchRight;
	Sprite::ptr hitLeft;
	Sprite::ptr hitRight;
	Sprite::ptr fallLeft;
	Sprite::ptr fallRight;
	Sprite::ptr kickLeft;
	Sprite::ptr kickRight;
	Sprite* current;

	PlayerState pState;
	Uint32 punchTimeout;
	Uint32 kickTimeout;
	Uint32 recoveryTimer;
	Uint8 hitCount;
  const Uint8 KnockDownHitCount;
	JumpState jumpState;
	VectF jumpLocation;
	static const float Gravity;
	static const int JumpHeight;
};
