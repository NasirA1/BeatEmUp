#pragma once
#include "GameObject.h"
#include "Sprite.h"

class Rock : public GameObject
{
public:
	Rock(const string& file, SDL_Renderer* const renderer);
	virtual ~Rock(void);
	virtual void Update(Game& world) override;
	virtual void Draw(SDL_Renderer* const renderer) const override;


private:
	SDL_Texture* texture;
};



class Knight : public GameObject
{
public:
	Knight(SDL_Renderer* const renderer);
	virtual void Update(Game& world) override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual ~Knight();
	virtual void SetDirection(Directions dir) override;
	void Stop();

private:
	void Translate(bool anim = true);


private:
	Sprite* walkRight;
	Sprite* walkLeft;
	Sprite* current;
};
