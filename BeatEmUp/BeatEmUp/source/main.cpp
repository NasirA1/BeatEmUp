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
	getchar();
#endif
	return 0;
}


#pragma region Balls Demo
#if 0
class Game : public SDLApp
{
public:
	Game();
	virtual ~Game();
	virtual bool Init() override;
	virtual void ProcessEvent(const SDL_Event& e) override;
	virtual void Update() override;
	virtual void Render() override;

private:
	GameObjectList gameObjects;
};


Game::Game() 
	: SDLApp(SCREEN_WIDTH
	, SCREEN_HEIGHT
	, "Balls Demo")
{
}


Game::~Game()
{
	logPrintf("Game object destroyed");
}


bool Game::Init()
{
	if(!SDLApp::Init())
		return false;

	Ball::AddRandomBalls(gameObjects, 20);

	return true;
}


void Game::ProcessEvent(const SDL_Event& e)
{
	switch(e.key.keysym.sym)
	{
	case SDLK_ESCAPE:
		quit_ = true;
		break;
	}
}


void Game::Update()
{
	gameObjects.Update();
}


void Game::Render()
{
	//Clear screen
	SDL_RenderClear( renderer_ );

	//Render texture to screen
	//SDL_RenderCopy( renderer_, gTexture, NULL, NULL );
	gameObjects.Draw( renderer_ );

	//Update screen
	SDL_RenderPresent( renderer_ );
}

#endif
#pragma endregion
