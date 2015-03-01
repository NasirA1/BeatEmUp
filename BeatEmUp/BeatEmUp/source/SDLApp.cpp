#include "SDLApp.h"
#include "Util.h"
#include <sstream>
#include <string>


#define FPS_NORMAL			60
#define FPS_SLOWMOTION	5

//Useful for testing/debugging
//const int SCREEN_FPS = FPS_SLOWMOTION;
const int SCREEN_FPS = FPS_NORMAL;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;


using namespace util;


SDLApp::SDLApp(const int clientWidth, const int clientHeight, const std::string& appTitle)
	: clientWidth_(clientWidth)
	, clientHeight_(clientHeight)
	, appTitle_(appTitle)
	, renderer_(NULL) 
	, window_(NULL)
	, fps_(0.0f)
	, quit_(false)
	, fullScreen_(false)
{

}


SDLApp::~SDLApp()
{
	if(renderer_)
	{
		SDL_DestroyRenderer(renderer_);
		renderer_ = NULL;
	}

	if(window_)
	{
		SDL_DestroyWindow( window_ );
		window_ = NULL;
	}

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}


bool SDLApp::Init()
{
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		logPrintf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return false;
	}

	window_ = SDL_CreateWindow( appTitle_.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		clientWidth_, clientHeight_, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOWPOS_CENTERED );
	if( !window_ )
	{
		logPrintf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
		return false;
	}

	//Set texture filtering to linear
	if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
	{
		logPrintf( "Warning: Linear texture filtering not enabled!" );
	}

	renderer_ = SDL_CreateRenderer( window_, -1, SDL_RENDERER_ACCELERATED );
	if( !renderer_ )
	{
		logPrintf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
		return false;
	}

	//Initialize renderer color
	SDL_SetRenderDrawColor( renderer_, 0xFF, 0xFF, 0xFF, 0xFF );
	
	logPrintf("Init successful.");
	return true;
}


void SDLApp::ToggleFullScreen()
{
	fullScreen_ = !fullScreen_;
	SDL_SetWindowFullscreen(window_, fullScreen_? SDL_WINDOW_FULLSCREEN /*SDL_WINDOW_FULLSCREEN_DESKTOP*/ : 0);
}


int SDLApp::Run()
{
	SDL_Event e;
	LTimer fpsTimer;
	LTimer capTimer;
	std::stringstream timeText;
	int countedFrames = 0;

	fpsTimer.start();

	while(!quit_)
	{
		capTimer.start();

		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit
			if( e.type == SDL_QUIT )
			{
				quit_ = true;
			}
			
			ProcessEvent(e);
		}

		//Calculate and correct fps
		float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
		if( avgFPS > 2000000 )
		{
			avgFPS = 0;
		}

		//Set text to be rendered
		if(true)
		{
			timeText.str("");
			timeText << "Average Frames Per Second " << avgFPS; 
			//logPrintf("%s", timeText.str().c_str());
		}

		Update();
		Render();

		++countedFrames;
		//If frame finished early
		int frameTicks = capTimer.getTicks();
		if( frameTicks < SCREEN_TICKS_PER_FRAME )
		{
			//Wait remaining time
			SDL_Delay( SCREEN_TICKS_PER_FRAME - frameTicks );
		}
	}

	return 0;
}

