#pragma once
#include "GameObject.h"
#include <string>



class BackgroundLayer : public GameObject
{	

public:
	BackgroundLayer(const std::string& filename, SDL_Renderer& renderer, int _screenWidth, int _screenHeight, float xVel);
	virtual void Update() override;
	virtual void Draw(SDL_Renderer& renderer) const override;
	virtual ~BackgroundLayer();


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
	Background(int clientWidth, int clientHeight, SDL_Renderer& renderer
		, const std::string& fileLayer1, const std::string& fileLayer2, const std::string& fileLayer3);
	virtual void Update() override;
	virtual void Draw(SDL_Renderer& renderer) const override;
	virtual ~Background();

	//Getters/Setters
	__forceinline bool IsScrolling() const { return scroll; }
	__forceinline void Stop() { scroll = false; }
	__forceinline void SetScroll(Direction dir) { SetDirection(dir); scroll = true; }
	__forceinline void SetScroll(bool enabled) { scroll = enabled; }

	__forceinline const BackgroundLayer& Bg1() const { return *bg1; }
	__forceinline const BackgroundLayer& Bg2() const { return *bg2; }
	__forceinline const BackgroundLayer& Bg3() const { return *bg3; }


private:
  bool scroll;
  GameObjectList layers;
	BackgroundLayer* bg1;
	BackgroundLayer* bg2;
	BackgroundLayer* bg3;
};
