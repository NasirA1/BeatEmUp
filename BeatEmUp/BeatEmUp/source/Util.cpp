#include "Util.h"
#include <string>

using namespace std;

namespace util
{

	SDLSurfaceFromFile::SDLSurfaceFromFile(const std::string& path , bool trans, Uint8 ckeyr, Uint8 ckeyg, Uint8 ckeyb)
		: file(path)
		, surface(IMG_Load(file.c_str()))
		, transparent(trans)
		, colKeyR(ckeyr), colKeyG(ckeyg), colKeyB(ckeyb)
	{
		if(!surface)
		{
			logPrintf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
		}
		else
		{
			//Color key image
			if(trans)
			{
				logPrintf("Color Key -> R: %d,  G: %d,  B: %d", colKeyR, colKeyG, colKeyB);
				SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, colKeyR, colKeyG, colKeyB));
			}
		}
	}


	SDLSurfaceFromFile::~SDLSurfaceFromFile()
	{
		if(surface)
		{
			SDL_FreeSurface(surface);
			surface = NULL;
			logPrintf("%s surface released", file.c_str());
		}
	}


	LTimer::LTimer()
	{
		//Initialize the variables
		mStartTicks = 0;
		mPausedTicks = 0;

		mPaused = false;
		mStarted = false;
	}

	
	void LTimer::start()
	{
		//Start the timer
		mStarted = true;

		//Unpause the timer
		mPaused = false;

		//Get the current clock time
		mStartTicks = SDL_GetTicks();
		mPausedTicks = 0;
	}

	
	void LTimer::stop()
	{
		//Stop the timer
		mStarted = false;

		//Unpause the timer
		mPaused = false;

		//Clear tick variables
		mStartTicks = 0;
		mPausedTicks = 0;
	}

	
	void LTimer::pause()
	{
		//If the timer is running and isn't already paused
		if( mStarted && !mPaused )
		{
			//Pause the timer
			mPaused = true;

			//Calculate the paused ticks
			mPausedTicks = SDL_GetTicks() - mStartTicks;
			mStartTicks = 0;
		}
	}

	
	void LTimer::unpause()
	{
		//If the timer is running and paused
		if( mStarted && mPaused )
		{
			//Unpause the timer
			mPaused = false;

			//Reset the starting ticks
			mStartTicks = SDL_GetTicks() - mPausedTicks;

			//Reset the paused ticks
			mPausedTicks = 0;
		}
	}

	
	Uint32 LTimer::getTicks()
	{
		//The actual timer time
		Uint32 time = 0;

		//If the timer is running
		if( mStarted )
		{
			//If the timer is paused
			if( mPaused )
			{
				//Return the number of ticks when the timer was paused
				time = mPausedTicks;
			}
			else
			{
				//Return the current time minus the start time
				time = SDL_GetTicks() - mStartTicks;
			}
		}

		return time;
	}

	
	bool LTimer::isStarted()
	{
		//Timer is running and paused or unpaused
		return mStarted;
	}

	bool LTimer::isPaused()
	{
		//Timer is running and paused
		return mPaused && mStarted;
	}


}//endnamespace
