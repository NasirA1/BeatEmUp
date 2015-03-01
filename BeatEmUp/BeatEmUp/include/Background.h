#pragma once
#include "GameObject.h"
#include <string>



class BackgroundLayer : public GameObject
{	

public:
	BackgroundLayer(const std::string& filename, SDL_Renderer* const renderer, int _screenWidth, int _screenHeight, float xVel);
	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual ~BackgroundLayer();


public:
	float XVel;


private:
	SDL_Texture* texture;
	RectF pos1;
	RectF pos2;
	int screenWidth;
	int screenHeight;
};



class Background : public GameObject
{	

public:
	Background(int clientWidth, int clientHeight, SDL_Renderer* const renderer);
	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual ~Background();

	//Getters/Setters
	__forceinline bool IsScrolling() const { return scroll; }
	__forceinline void Stop() { scroll = false; }
	__forceinline void SetScroll(Directions dir) { SetDirection(dir); scroll = true; }
	__forceinline void SetScroll(bool enabled) { scroll = enabled; }

private:
  bool scroll;
  GameObjectList layers;
};
