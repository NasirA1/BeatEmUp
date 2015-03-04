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

	position.x = 100, position.y = 400;
	position.w = (float)frameWidth, position.h = (float)frameHeight;
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
	SDL_Rect src = { (int)((float)col * position.w), (int)((float)row * position.h)
		, (int)position.w, (int)position.h };

	SDL_Rect nPos;
	util::Convert(position, nPos);
	SDL_RenderCopyEx(renderer, sheet, &src, &nPos, GetAngle(), NULL, SDL_FLIP_NONE);
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

