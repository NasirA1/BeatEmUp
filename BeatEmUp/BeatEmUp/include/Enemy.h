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
	, Sprite* idleLeftSprite, Sprite* idleRightSprite
	, Sprite* walkLeftSprite, Sprite* walkRightSprite
	, Sprite* punchLeftSprite, Sprite* punchRightSprite
	, Sprite* hitLeftSprite, Sprite* hitRightSprite
	, Sprite* fallLeftSprite, Sprite* fallRightSprite
	, float posX, float posY
	, int health
	, const Uint32 attackTimeOut
	, float speed_
	, float patrolRange_
	, float patrolVecX_
	, float vision_
	, const float minDistX, const float minDistY
);

	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual ~Enemy();

	void OnHit();

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


protected:
	virtual void Propagate();

private:
	void Translate();
	void Translate(bool anim);
	void Walk(Directions dir);
	void Stop();
	void Attack();
	void Jump(float xAccel, float yAccel);
	void OnKnockDown();
	void OnPatrol();
	void OnRecovery();
	void OnChase();
	void OnAttack();
	void OnIdle();


protected:
	Sprite* idleRight;
	Sprite* idleLeft;
	Sprite* walkRight;
	Sprite* walkLeft;
	Sprite* current;
	Sprite* hitLeft;
	Sprite* hitRight;
	Sprite* fallLeft;
	Sprite* fallRight;

	Sprite* attackRight;
	Sprite* attackLeft;
	EState state;
	Uint32 attackTimer;
	Uint32 idleTimer;
	Uint32 recoveryTimer;
	Uint8 hitCount;
	
	const Uint8 KnockDownHitCount;
	const Uint32 AttackTimeOut;

	float patrolRange;
	float patrolVecX;
	float vision;

	const float MinDistX;
	const float MinDistY;


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


class Andore : public Enemy
{
public:
	Andore(SDL_Renderer* const renderer, float posX, float posY);
	virtual ~Andore();

private:
	void OnPunchSprite(const Sprite* const sender, const Sprite::FramePlayedEventArgs* const e);
};


class Joker : public Enemy
{
public:
	Joker(SDL_Renderer* const renderer, float posX, float posY);
	virtual ~Joker();

protected:
	virtual void Propagate() override;

private:
	void OnStickSprite(const Sprite* const sender, const Sprite::FramePlayedEventArgs* const e);
};
