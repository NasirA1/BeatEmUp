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

private:
	SDL_Texture* texture;
};



class Roamer : public GameObject
{
public:
	Roamer(SDL_Renderer* const renderer, Sprite* walkLeftSprite, Sprite* walkRightSprite
		, float posX, float posY, float roamMinX_, float roamMaxX_);
	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual ~Roamer();
	virtual void SetDirection(Directions dir) override;
	void Stop();

private:
	void Translate(bool anim = true);


private:
	Sprite* walkRight;
	Sprite* walkLeft;
	Sprite* current;
	float roamMinX;
	float roamMaxX;
};
