#pragma once
#include "GameObject.h"
#include "Sprite.h"
#include <queue>
#include "Util.h"



//You can write :
//deleted_unique_ptr<Foo> foo(new Foo(), [](Foo* f) { customdeleter(f); });


class Rock : public GameObject
{
public:
	Rock(const string& file, SDL_Renderer& renderer);
	virtual ~Rock();
	virtual void Update() override;
	virtual void Draw(SDL_Renderer& renderer) const override;

public:
	static const float Range;

private:
	unique_ptr2<SDL_Texture> texture;
};



enum class EnemyState
{
	Entry,
	Idle,
	Patrolling,
	Visiting,
	Chasing,
	Attacking,
	Hit,
	KnockedDown,
	Dead
};


class Enemy : public GameObject
{
public:
	Enemy(SDL_Renderer& renderer
	, Sprite::ptr idleLeftSprite, Sprite::ptr idleRightSprite
	, Sprite::ptr walkLeftSprite, Sprite::ptr walkRightSprite
	, Sprite::ptr punchLeftSprite, Sprite::ptr punchRightSprite
	, Sprite::ptr hitLeftSprite, Sprite::ptr hitRightSprite
	, Sprite::ptr fallLeftSprite, Sprite::ptr fallRightSprite
	, const string& name_
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
	virtual void Draw(SDL_Renderer& renderer) const override;
	virtual ~Enemy();

	void OnHit();

	__forceinline bool IsDead() const { return state == EnemyState::Dead; }
	__forceinline bool IsAttackable() const {
		return state !=	EnemyState::KnockedDown && state != EnemyState::Dead;
	}
	

	//Chasing - alternate path info
	queue<SDL_Point> visitPath;

protected:
	virtual void Propagate();

private:
	void Translate();
	void Translate(bool anim);
	void Walk(Direction dir);
	void Visit();
	void VisitAltPlayer();
	void Stop();
	void Attack();
	void Jump(float xAccel, float yAccel);
	void OnKnockDown();
	void OnPatrol();
	void OnRecovery();
	void OnChase();
	void OnAttack();
	void OnIdle();
	void OnVisit(EnemyState destState);

protected:
	Sprite::ptr idleRight;
	Sprite::ptr idleLeft;
	Sprite::ptr walkRight;
	Sprite::ptr walkLeft;
	Sprite::ptr hitLeft;
	Sprite::ptr hitRight;
	Sprite::ptr fallLeft;
	Sprite::ptr fallRight;
	Sprite::ptr attackRight;
	Sprite::ptr attackLeft;
	Sprite* current;

	EnemyState state;
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

	JumpState jumpState;
	VectF jumpLocation;
	static const float Gravity;
	static const int JumpHeight;
};


class Andore : public Enemy
{
public:
	Andore(SDL_Renderer& renderer, float posX, float posY);
	virtual ~Andore();

private:
	void OnPunchSprite(const Sprite& sender, const Sprite::FramePlayedEventArgs& e);
};


class Joker : public Enemy
{
public:
	Joker(SDL_Renderer& renderer, float posX, float posY);
	virtual ~Joker();

protected:
	virtual void Propagate() override;

private:
	void OnStickSprite(const Sprite& sender, const Sprite::FramePlayedEventArgs& e);
};


class Axl : public Enemy
{
public:
	Axl(SDL_Renderer& renderer, float posX, float posY);
	virtual ~Axl();

private:
	void OnPunchSprite(const Sprite& sender, const Sprite::FramePlayedEventArgs& e);
};
