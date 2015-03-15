#pragma once
#include "GameObject.h"
#include "Sprite.h"




class Rock : public GameObject
{
public:
	Rock(const string& file, SDL_Renderer* const renderer);
	virtual ~Rock(void);
	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;

public:
	static const float Range;

private:
	SDL_Texture* texture;
};



class Enemy : public GameObject
{
public:
	Enemy(SDL_Renderer* const renderer
	, Sprite* walkLeftSprite, Sprite* walkRightSprite
	, Sprite* punchLeftSprite, Sprite* punchRightSprite
	, Sprite* hitLeftSprite, Sprite* hitRightSprite
	, Sprite* fallLeftSprite, Sprite* fallRightSprite
	, float posX, float posY);

	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual ~Enemy();
	virtual void SetDirection(Directions dir) override;
	void Stop();
	void Attack();
	void OnPlayerAttack();
	void Jump(float xAccel, float yAccel);

	__forceinline bool IsAttackable()
	{
		return state !=	ES_KnockedDown && state != ES_Dead;
	}
	
	enum EState
	{
		ES_Idle,
		ES_Patrolling,
		ES_Chasing,
		ES_Attacking,
		ES_Hit,
		ES_KnockedDown,
		ES_Dead
	};


private:
	void Translate(bool anim);
	void Translate();
	void OnKnockDown();
	void OnPatrol();
	void OnRecovery();
	void OnChase();
	void OnPunch();
	void OnIdle();
	void OnPunchSprite(const Sprite* const sender, const Sprite::FramePlayedEventArgs* const e);


private:
	Sprite* walkRight;
	Sprite* walkLeft;
	Sprite* current;
	Sprite* hitLeft;
	Sprite* hitRight;
	Sprite* fallLeft;
	Sprite* fallRight;

	Sprite* punchRight;
	Sprite* punchLeft;
	EState state;
	Uint32 punchTimer;
	Uint32 idleTimer;
	Uint32 recoveryTimer;
	Uint8 hitCount;
	const Uint8 KnockDownHitCount;

	float patrolRange;
	float patrolVecX;
	float vision;

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
