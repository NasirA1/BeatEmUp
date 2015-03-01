#pragma once
#include "GameObject.h"
#include <string>


class Rock : public GameObject
{
public:
	Rock(const std::string& filename, SDL_Renderer* const renderer);
	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual ~Rock();


private:
	SDL_Texture* texture;
	double angle; //rotation angle
};
