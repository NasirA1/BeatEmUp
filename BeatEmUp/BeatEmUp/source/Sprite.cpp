#include "Sprite.h"
#include "Util.h"



Sprite::Sprite(SDL_Surface* const spriteSheet, SDL_Renderer& renderer, 
	int frameWidth, int frameHeight, int frameSpeed_, int stillFrame_, bool playReverse)
	: GameObject("", GT_Sprite)
	, sheet(nullptr)
	, frameSpeed(frameSpeed_)
	, stillFrame(stillFrame_)
	, fromIndex(0)
	, toIndex(0)
	, currentFrame(0)
	, animationRunning(false)
	, loop(true)
	, reverse(playReverse)
{
	if(!spriteSheet) return;

	position.x = 100, position.y = 400;
	position.w = (float)frameWidth, position.h = (float)frameHeight;
	framesPerRow = (int)SDL_floor((double)spriteSheet->w / frameWidth);
	rowCount = (int)SDL_floor((double)spriteSheet->h / frameHeight);
	frameCount = framesPerRow * rowCount;
	sheet = unique_ptr2<SDL_Texture>(
		SDL_CreateTextureFromSurface(&renderer, spriteSheet), 
		[](auto p) { SDL_DestroyTexture(p); }
	);

	fromIndex = 0;
	toIndex = frameCount - 1;
	logPrintf("spritesheet loaded (%d,%d) %d frames", spriteSheet->w, spriteSheet->h, frameCount);
}


void Sprite::PlayFrames(int fromFrame, int toFrame, bool loop_) 
{
	fromIndex = currentFrame = fromFrame; 
	toIndex = toFrame;
	SetLoop(loop_);
	SetAnimation(true);
}


void Sprite::Update()
{
	if (!animationRunning)
	{
		//currentFrame = stillFrame;
		return;
	}

	// update to the next frame if it is time
	if (counter == (frameSpeed - 1)) 
	{
		if(reverse)
		{
			currentFrame = (currentFrame - 1) % (toIndex + 1);        
			if (currentFrame < 0) currentFrame = toIndex;
			//logPrintf("currentFrame: %d, toIndex: %d", currentFrame, toIndex);
		}
		else
		{
			currentFrame = (currentFrame + 1) % (toIndex + 1);
			if(currentFrame < fromIndex) currentFrame = fromIndex;
		}

		//notify listeners (if any)
		FramePlayed.notify(*this, FramePlayedEventArgs(currentFrame));
	}
	// update the counter
	counter = (counter + 1) % frameSpeed;
}


void Sprite::Draw(SDL_Renderer& renderer) const
{
	int row = currentFrame / framesPerRow;
	int col = currentFrame % framesPerRow;
	SDL_Rect src = { (int)((float)col * position.w), (int)((float)row * position.h)
		, (int)position.w, (int)position.h };

	SDL_Rect nPos;
	util::Convert(position, nPos);
	SDL_RenderCopyEx(&renderer, sheet.get(), &src, &nPos, GetAngle(), nullptr, SDL_FLIP_NONE);

	if(!loop && IsAnimationRunning())
	{
		if( (!reverse && currentFrame == toIndex) || (reverse && currentFrame == fromIndex) )
			const_cast<Sprite*>(this)->SetAnimation(false);
	}
}


Sprite::~Sprite()
{
	logPrintf("Sprite object released");
}
