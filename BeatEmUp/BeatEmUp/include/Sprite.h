#pragma once
#include "GameObject.h"
#include "Mixer.h"
#include "CppEvent.h"


class Sprite : public GameObject
{

public:
	Sprite(SDL_Surface* const spriteSheet, SDL_Renderer* const renderer, 
		int frameWidth, int frameHeight, int frameSpeed_, int stillFrame_, bool playReverse = false);
	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;
	virtual ~Sprite();

	//Getters
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
	__forceinline void SetLoop(bool enabled) { loop = enabled; }
	void PlayFrames(int fromFrame, int toFrame, bool loop_);

	//Event dispatch
	struct FramePlayedEventArgs : public EventArgs
	{
		FramePlayedEventArgs(const int frameIndex) 
			: FrameIndex(frameIndex){}
		const int FrameIndex;
	};
	CppEvent<const Sprite* const, const FramePlayedEventArgs* const> FramePlayed;


	static inline Sprite* FromFile(string filename, SDL_Renderer* const renderer, 
		int frameWidth, int frameHeight, int frameSpeed, int stillFrame, bool playReverse = false
		, Uint8 colKeyR = 0x00, Uint8 colKeyG = 0x00, Uint8 colKeyB = 0x00)
	{
		util::SDLSurfaceFromFile surface(filename, true, colKeyR, colKeyG, colKeyB);
		return new Sprite(surface.surface, renderer, frameWidth, frameHeight, frameSpeed, stillFrame, playReverse);
	}


private:
	SDL_Texture* sheet;
	int framesPerRow;
	int rowCount;
	int currentFrame;
	int frameCount;
	int frameSpeed;
	int counter;
	bool animationRunning;
	int stillFrame;
	int fromIndex;
	int toIndex;
	bool loop;
	bool reverse;
};
