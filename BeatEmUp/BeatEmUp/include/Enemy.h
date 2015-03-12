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



class Andore : public GameObject
{
public:
	Andore(SDL_Renderer* const renderer, Sprite* walkLeftSprite, Sprite* walkRightSprite
		, Sprite* punchLeftSprite, Sprite* punchRightSprite
		, float posX, float posY);
	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual ~Andore();
	virtual void SetDirection(Directions dir) override;
	void Stop();
	void Attack();

	enum EState
	{
		ES_Patrolling,
		ES_Chasing,
		ES_Attacking,
		ES_Idle
	};

private:
	void Translate(bool anim = true);


private:
	Sprite* walkRight;
	Sprite* walkLeft;
	Sprite* current;

	Sprite* punchRight;
	Sprite* punchLeft;
	EState state;
	Uint32 punchTimer;
	Uint32 idleTimer;
};
