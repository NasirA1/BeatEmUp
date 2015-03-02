#include "Sprite.h"
#include "Util.h"



Sprite::Sprite(SDL_Surface* const spriteSheet, SDL_Renderer* const renderer, 
	int frameWidth, int frameHeight, int frameSpeed_, int stillFrame_)
	: GameObject()
	, sheet(NULL)
	, frameSpeed(frameSpeed_)
	, stillFrame(stillFrame_)
	, currentFrame(0)
	, animationRunning(false)
{
	if(!spriteSheet || !renderer) return;

	memset(&pos, 0, sizeof(SDL_Rect));
	pos.x = 100, pos.y = 400;
	pos.w = frameWidth, pos.h = frameHeight;
	framesPerRow = (int)SDL_floor((double)spriteSheet->w / frameWidth);
	rowCount = (int)SDL_floor((double)spriteSheet->h / frameHeight);
	frameCount = framesPerRow * rowCount;
	sheet = SDL_CreateTextureFromSurface(renderer, spriteSheet);

	logPrintf("spritesheet loaded (%d,%d) %d frames", spriteSheet->w, spriteSheet->h, frameCount);
}


void Sprite::Update()
{
	if (!animationRunning)
	{
		currentFrame = stillFrame;
		return;
	}

	// update to the next frame if it is time
	if (counter == (frameSpeed - 1))
		currentFrame = (currentFrame + 1) % frameCount;

	// update the counter
	counter = (counter + 1) % frameSpeed;
}


void Sprite::Draw(SDL_Renderer* const renderer) const
{
	int row = currentFrame / framesPerRow;
	int col = currentFrame % framesPerRow;
	SDL_Rect src = { col * pos.w, row * pos.h, pos.w, pos.h };
	SDL_RenderCopy(renderer, sheet, &src, &pos); 
}


Sprite::~Sprite()
{
	if(sheet)
	{
		SDL_DestroyTexture(sheet);
		sheet = NULL;
	}

	logPrintf("Sprite object released");
}


