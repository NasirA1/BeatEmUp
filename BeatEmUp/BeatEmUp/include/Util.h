#pragma once
#include <stdio.h>
#include <ctime>
#include <string>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <memory>
#include <functional>


#ifdef _DEBUG
#define logPrintf(fmt, ...)	fprintf(stderr, fmt "\n", __VA_ARGS__)
#else
#define logPrintf()
#endif

#define __WHEEL	util::Random::Instance()



namespace util
{

	//Allows you to write:
	//unique_ptr2<Foo> foo(customcreate(), [](Foo* f) { customdeleter(f); });
	//unique_ptr2<FILE> file(fopen("file.txt", "r"), [](FILE* f) { fclose(f); });
	template<typename T>
	using unique_ptr2 = std::unique_ptr<T, std::function<void(T*)>>;


	//General-purpose Singleton mixin
	template <typename T>
	class Singleton
	{
	public:
		static T& Instance()
		{
			static T instance;
			return instance;
		}
	};


	//Float rectangle (useful for more precise calculations)
	//e.g. move by 2.5 pixels
	struct RectF
	{
		float x, y, w, h;
		float z;
		RectF(float x_ = 0.0f, float y_ = 0.0f, float w_ = 0.0f, float h_ = 0.0f, float z_ = 0.0f)
		 : x(x_), y(y_), w(w_), h(h_), z(z_){}

		__forceinline float left() const { return x; } 
		__forceinline float right() const { return x + w; }
		__forceinline float top() const { return y; }
		__forceinline float bottom() const { return y + h; }
	};

	
	//Converts a float rectangle to an int rectangle (useful in rendering)
	inline void Convert(const RectF& rectF, SDL_Rect& rect)
	{
		rect.x = (int)rectF.x;
		rect.y = (int)rectF.y;
		rect.w = (int)rectF.w;
		rect.h = (int)rectF.h;
	}

	
	struct VectF
	{
		float x, y;
		VectF(float x_ = 0.0f, float y_ = 0.0f) : x(x_), y(y_){}
	};


	static inline float GetDistance(const RectF& a, const RectF& b)
	{
		double dx = a.x - b.x;
		double dy = a.y - b.y;
		double dist = SDL_sqrt(dx * dx + dy * dy);
		return (float)dist;
	}


	//Singleton pseudo-random number generator
	struct Random : public Singleton<Random>
	{
		Random()
		{
			srand((unsigned int)time(nullptr));
		}

		//Note: max is EXCLUSIVE
		__forceinline unsigned long Next(const int min, const int max) const
		{
			int r = rand();
			return min + r % (max - min);
		}

		//Note: max is EXCLUSIVE
		__forceinline float Next(const float min, const float max) const
		{
			float r = (float)rand() / (float)RAND_MAX;
			return min + r * (max - min);
		}

		__forceinline bool TakeAChance() const
		{
			return Next(0, 2) > 0;
		}
	};


	static inline Uint32 get_pixel32( SDL_Surface *surface, int x, int y )
	{
		//Convert the pixels to 32 bit
		Uint32 *pixels = (Uint32 *)surface->pixels;
		//Get the requested pixel
		return pixels[ ( y * surface->w ) + x ];
	}


	static inline void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel )
	{
		//Convert the pixels to 32 bit
		Uint32 *pixels = (Uint32 *)surface->pixels;
		//Set the pixel
		pixels[ ( y * surface->w ) + x ] = pixel;
	}


	static inline bool IntersectsPixel(const SDL_Rect& r1, const SDL_Rect& r2
		, const SDL_Colour* const data1, const SDL_Colour* const data2)
	{
		int top = SDL_max(r1.y, r2.y);
		int bottom = SDL_min(r1.y + r1.h, r2.y + r2.h);
		int left = SDL_max(r1.x, r2.x);
		int right = SDL_min(r1.x + r1.w, r2.x + r2.w);

		for(int y = top; y < bottom; y++)
		{
			for(int x = left; x < right; x++)
			{
				SDL_Colour c1 = data1[ (x - r1.x) + (y - r1.y) * r1.w ];
				SDL_Colour c2 = data2[ (x - r2.x) + (y - r2.y) * r2.w ];

				//if the two colours are not transparent
				if(c1.a != 0 && c2.a != 0) return true;
			}
		}

		return false;
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


	struct TTFont
	{
		TTF_Font* font;

		TTFont(const std::string& fileName, size_t size)
			: font(nullptr)
		{
			font = TTF_OpenFont(fileName.c_str(), size);
			if(!font)
			{
				logPrintf("Unable to open {%s}  ERROR: %s", fileName.c_str(), TTF_GetError());
			}
		}


		~TTFont()
		{
			if(font)
			{
				TTF_CloseFont(font);
				font = nullptr;
				logPrintf("TTFont object released");
			}
		}
	};


	struct TextTexture
	{
		SDL_Surface* const surface; 
		SDL_Texture* const texture;

		TextTexture(SDL_Renderer& renderer, const TTFont& font, const std::string& text, const SDL_Colour& colour)
			: surface(TTF_RenderText_Solid(font.font, text.c_str(), colour))
			, texture(SDL_CreateTextureFromSurface(&renderer, surface))
		{
			if(!surface)
			{
				logPrintf( "TextTexture ERROR: %s", TTF_GetError() );
			}

			if(!texture)
			{
				logPrintf( "TextTexture ERROR: %s", SDL_GetError() );
			}
		}

		~TextTexture()
		{
			if(texture)
			{
				SDL_DestroyTexture(texture);
				const_cast<SDL_Texture*>(texture) = nullptr;
			}
			if(surface)
			{
				SDL_FreeSurface(surface);
				const_cast<SDL_Surface*>(surface) = nullptr;
			}
		}
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


}//endnamespace

