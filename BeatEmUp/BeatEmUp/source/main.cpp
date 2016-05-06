#include "Game.h"
#include <stdio.h>


int main( int argc, char* args[] )
{

	{
		if(Game::Instance().Init())
		{
			Game::Instance().Run();
		}
	}
	
#ifdef _DEBUG
	//getchar();
#endif
	return 0;
}



#pragma region Balls Demo
#if 0
#include "GameObject.h"
#include "SDLApp.h"
#include "util.h"
class Demo : public SDLApp, public util::Singleton<Demo>
{
public:
	Demo();
	virtual ~Demo();
	virtual bool Init() override;
	virtual void ProcessEvent(const SDL_Event& e) override;
	virtual void Update() override;
	virtual void Render() override;

private:
	GameObjectList gameObjects;
};


Demo::Demo() 
	: SDLApp(800, 600, "Balls Demo")
{
}


Demo::~Demo()
{
	logPrintf("Demo object destroyed");
}


bool Demo::Init()
{
	if(!SDLApp::Init())
		return false;

	Ball::AddRandomBalls(gameObjects, 20);

	return true;
}


void Demo::ProcessEvent(const SDL_Event& e)
{
	switch(e.key.keysym.sym)
	{
	case SDLK_ESCAPE:
		quit_ = true;
		break;
	}
}


void Demo::Update()
{
	gameObjects.Update();
}


void Demo::Render()
{
	//Clear screen
	SDL_RenderClear( renderer_ );

	//Render texture to screen
	//SDL_RenderCopy( renderer_, gTexture, nullptr, nullptr );
	gameObjects.Draw( renderer_ );

	//Update screen
	SDL_RenderPresent( renderer_ );
}

#endif
#pragma endregion
