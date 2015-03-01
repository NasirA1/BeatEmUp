#pragma once
#include "GameObject.h"


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

