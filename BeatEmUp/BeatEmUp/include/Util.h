#pragma once
#include <stdio.h>
#include <ctime>
#include <string>
#include <SDL_image.h>

#ifdef _DEBUG
#define logPrintf(fmt, ...)	fprintf(stderr, fmt "\n", __VA_ARGS__)
#else
#define logPrintf()
#endif


namespace util
{
	struct RectF
	{
		float x, y, w, h;
		RectF(float x_ = 0.0f, float y_ = 0.0f, float w_ = 0.0f, float h_ = 0.0f)
		 : x(x_), y(y_), w(w_), h(h_){}

		__forceinline float left() const { return x; } 
		__forceinline float right() const { return x + w; }
		__forceinline float top() const { return y; }
		__forceinline float bottom() const { return y + h; }
	};

	
	inline void Convert(const RectF& rectF, SDL_Rect& rect)
	{
		rect.x = rectF.x;
		rect.y = rectF.y;
		rect.w = rectF.w;
		rect.h = rectF.h;
	}

	
	struct VectF
	{
		float x, y;
		VectF(float x_ = 0.0f, float y_ = 0.0f) : x(x_), y(y_){}
	};


	struct Random
	{
		Random()
		{
			srand(time(NULL));
		}

		__forceinline int Next(const int min, const int max) const
		{
			return rand() % max + min;
		}

		static const Random& Instance()
		{
			static const Random instance;
			return instance;
		}
	};


	inline Uint32 get_pixel32( SDL_Surface *surface, int x, int y )
	{
		//Convert the pixels to 32 bit
		Uint32 *pixels = (Uint32 *)surface->pixels;
		//Get the requested pixel
		return pixels[ ( y * surface->w ) + x ];
	}


	inline void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel )
	{
		//Convert the pixels to 32 bit
		Uint32 *pixels = (Uint32 *)surface->pixels;
		//Set the pixel
		pixels[ ( y * surface->w ) + x ] = pixel;
	}

	
	//RAII for SDL_Surface objects
	struct SDLSurfaceFromFile
	{
		const std::string file;
		SDL_Surface* surface;
		const bool transparent;
		const Uint8 colKeyR, colKeyG, colKeyB;

		SDLSurfaceFromFile(const std::string& path, bool trans = false
			, Uint8 ckeyr = 0x00, Uint8 ckeyg = 0x00, Uint8 ckeyb = 0x00);
		~SDLSurfaceFromFile();
	};


	//The application time based timer
	class LTimer
	{
	public:
		//Initializes variables
		LTimer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		Uint32 getTicks();

		//Checks the status of the timer
		bool isStarted();
		bool isPaused();

	private:
		//The clock time when the timer started
		Uint32 mStartTicks;

		//The ticks stored when the timer was paused
		Uint32 mPausedTicks;

		//The timer status
		bool mPaused;
		bool mStarted;
	};


	template <class T> void Release(T t)
	{
		if(t)
		{
			t->Release();
			t = NULL;
		}
	}

	template <class T> void Delete(T t)
	{
		if(t)
		{
			delete t;
			t = NULL;
		}
	}

}//endnamespace

