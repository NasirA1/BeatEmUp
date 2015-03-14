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
	
	enum EState
	{
		ES_Patrolling,
		ES_Chasing,
		ES_Attacking,
		ES_Hit,
		ES_KnockedDown,
		ES_Idle
		/*No ES_Dead state
		health of 0 indicates death
		*/
	};


private:
	void Translate(bool anim);
	void Translate();
	void HandleKnockedDown();


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
