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
	void Punch();

	enum State
	{
		ST_Patrolling,
		ST_Chasing,
		ST_Punching
	};

private:
	void Translate(bool anim = true);


private:
	Sprite* walkRight;
	Sprite* walkLeft;
	Sprite* current;

	Sprite* punchRight;
	Sprite* punchLeft;
	State state;
	Uint32 punchTimer;
};
