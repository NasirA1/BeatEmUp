#pragma once
#include "GameObject.h"


class Sprite : public GameObject
{
private:
	SDL_Texture* sheet;

private:
	int framesPerRow;
	int rowCount;
	int currentFrame;
	int frameCount;
	int frameSpeed;
	int counter;
	bool animationRunning;
	int stillFrame;
	SDL_Rect pos;

public:
	Sprite(SDL_Surface* const spriteSheet, SDL_Renderer* const renderer, 
		int frameWidth, int frameHeight, int frameSpeed_, int stillFrame_);
	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual ~Sprite();

	//Getters
	__forceinline SDL_Rect& Pos() { return pos; }
	__forceinline bool IsAnimationRunning() const { return animationRunning; }
	__forceinline int GetCurrentFrame() const { return currentFrame; }
	__forceinline int GetStillFrame() const { return stillFrame; }
	__forceinline int GetFrameSpeed() const { return frameSpeed; }
	__forceinline int GetFrameCount() const { return frameCount; }

	//Setters
	__forceinline void SetAnimation(bool enabled) { animationRunning = enabled; }
	__forceinline void SetCurrentFrame(int index) { currentFrame = index; }
	__forceinline void SetFrameSpeed(int fps) { frameSpeed = fps; }
	__forceinline void SetStill() { currentFrame = stillFrame; animationRunning = false; }

	static inline Sprite* FromFile(string filename, SDL_Renderer* const renderer, 
		int frameWidth, int frameHeight, int frameSpeed, int stillFrame)
	{
		util::SDLSurfaceFromFile surface(filename, true, 0xFF, 0x40, 0x40);
		return new Sprite(surface.surface, renderer, frameWidth, frameHeight, frameSpeed, stillFrame);
	}

};

