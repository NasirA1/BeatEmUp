#include "Game.h"
#include <sstream>
#include <algorithm>
#include "Mixer.h"


Game::Game() 
	: SDLApp(SCREEN_WIDTH
	, SCREEN_HEIGHT
	, "Nasir's Beat 'em Up Game")
	, MoveBounds(0.0f, 370.0f, (float)SCREEN_WIDTH, 120.0f)
	, player(nullptr)
	, tbFps(nullptr), tbPlayerPos(nullptr), tbEnemyPos(nullptr)
	, bg(nullptr)
	, world(nullptr)
	, leftDown(false)
	, rightDown(false)
	, upDown(false)
	, downDown(false)
	, currentLevel(0LU)
	, MaxLevel(10LU)
{
}


Game::~Game()
{
	logPrintf("Game object released");
}


bool Game::Init()
{
	if(!SDLApp::Init())
		return false;

	//Create and add player
	//currently only one character (baddude) supported
	player = make_unique<Player>(renderer());
	tbFps = make_unique<TextBlock>("FPS: 00.000000", 16, 0.0f, 0.0f, renderer());
	tbPlayerPos = make_unique<TextBlock>("Pos {}", 16, 0.0f, tbFps->Position().bottom() + 1, renderer());
	tbEnemyPos = make_unique<TextBlock>("Enemy Pos {}", 16, 0.0f, tbPlayerPos->Position().bottom() + 1, renderer());

	//Load level1
	LoadNextLevel();

	MIXER.Instance();
	//MIXER.Play(Mixer::ST_Track1);
	return true;
}


void Game::ProcessEvent(const SDL_Event& e)
{
	//Temporary
	//For now we just disable player keys
	//TODO Add proper keyboard control (e.g. properly handling ESC/F1/other keys
	//when player is dead)
	if(player->IsDead())
	{
		leftDown = rightDown = upDown = downDown = false;
		return;
	}

	if(e.key.state == SDL_PRESSED && !e.key.repeat)
	{
		switch(e.key.keysym.sym)
		{
		case SDLK_F1:
			ToggleFullScreen();
			break;
		case SDLK_ESCAPE:
			quit_ = true;
			break;
			/* Player Keys */
		case SDLK_LEFT:
			leftDown = true;
			bg->SetScroll(Direction::Right);
			break;
		case SDLK_RIGHT:
			rightDown = true;
			bg->SetScroll(Direction::Left);
			break;
		case SDLK_UP:
			upDown = true;
			bg->SetScroll(player->GetDirection() == Direction::Right? Direction::Left : Direction::Right);
			break;
		case SDLK_DOWN:
			downDown = true;
			bg->SetScroll(player->GetDirection() == Direction::Right? Direction::Left : Direction::Right);
			break;
		case SDLK_SPACE:
			player->Jump();
			break;
		case SDLK_a:
			player->Punch();
			break;
		case SDLK_s:
			player->Kick();
			break;
		}
	}
	else if(e.key.state == SDL_RELEASED && !e.key.repeat)
	{
		switch(e.key.keysym.sym)
		{
		case SDLK_LEFT:
			leftDown = false;
			Stop();
			break;
		case SDLK_RIGHT:
			rightDown = false;
			Stop();
			break;
		case SDLK_UP:
			upDown = false;
			Stop();
			break;
		case SDLK_DOWN:
			downDown = false;
			Stop();
			break;
		}
	}
}


bool Game::LevelComplete() const
{
	//all enemies destroyed!
	return enemies.size() <= 0;
}


bool Game::LoadNextLevel()
{
	if(currentLevel < MaxLevel) 
		currentLevel++;
	else 
		return false;
	logPrintf("Loading Level{%lu}", currentLevel);

	switch(currentLevel)
	{
	//currently ALL 10 levels the same (for testing purposes)
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		{
			enemies.clear();
			world.reset(new World);

			//Add background
			bg = world->AddGameObject<Background>(clientWidth_, clientHeight_, renderer(), "resources/bg1.gif", "resources/bg2.gif", "resources/bg3.gif");

			//Add some 'roamers'
			world->AddGameObject<Roamer>(renderer(), 
				Sprite::FromFile("resources/skater_left.png", renderer(), 71, 90, 11, 0),
				Sprite::FromFile("resources/skater_right.png", renderer(), 71, 90, 11, 0), 
				-200.0f, 390.0f, -200.0f, 1000.0f, true);

			world->AddGameObject<Roamer>(renderer(), 
				Sprite::FromFile("resources/knightwalk_left.png", renderer(), 128, 128, 4, 15),
				Sprite::FromFile("resources/knightwalk_right.png", renderer(), 128, 128, 4, 3), 
				5000.0f, 480.0f, -5000.0f, 5000.0f, false);

			//Add a rock
			world->AddGameObject<Rock>("resources/rock.png", renderer());

			//Add some enemies
			enemies.push_back(world->AddGameObject<Andore>(renderer(), 1200.0f, 450.0f));
			enemies.push_back(world->AddGameObject<Andore>(renderer(), 1200.0f, 450.0f));
			enemies.push_back(world->AddGameObject<Andore>(renderer(), 2400.0f, 450.0f));
			enemies.push_back(world->AddGameObject<Joker>(renderer(), 1000.0f, 400.0f));
			enemies.push_back(world->AddGameObject<Axl>(renderer(), 800.0f, 400.0f));
			enemies.push_back(world->AddGameObject<Andore>(renderer(), 700.0f, 380.0f));
			enemies.push_back(world->AddGameObject<Axl>(renderer(), -200.0f, 400.0f));
			enemies.push_back(world->AddGameObject<Joker>(renderer(), 1100.0f, 400.0f));
			enemies.push_back(world->AddGameObject<Axl>(renderer(), 500.0f, 400.0f));

			//Add non-owned objects so then can be drawn
			world->AddGameObject(*tbFps);
			world->AddGameObject(*tbPlayerPos);
			world->AddGameObject(*tbEnemyPos);
			world->AddGameObject(*player);
		}
		break;

	//other levels
	//{
	//}
	//break;

	default:
		logPrintf("*** MUST NEVER GET HERE ***");
		return false;
	}

	logPrintf("Level{%lu} Loaded.  gameObjects<%d>", currentLevel, world->Count());
	return true;
}


void Game::Update()
{
	//Level management
	if(player->IsDead())
	{
		//TODO
		logPrintf("GAME OVER!");
		//end the game
		//try again? yes/no
		//Resurrect player
	}
	else if(LevelComplete())
	{
		logPrintf("Level{%lu} Completed", currentLevel);

		if(currentLevel == MaxLevel)
		{
			//TODO
			//game complete
			//end the game
			//Play end credits
			logPrintf("*** GAME COMPLETED ***");
			return;
		}
		else
		{
			LoadNextLevel();
		}
	}

	//Gameplay
	//Update movement vectors
	if (upDown) player->GoUp();
	if (downDown) player->GoDown();
	if (rightDown) player->GoRight();
	if (leftDown) player->GoLeft();
	bg->SetScroll(player->GetState() == PlayerState::Walking
		|| player->GetState() == PlayerState::Jumping);

	//text
	std::stringstream ss;
	ss << "FPS: " << Fps();
	tbFps->SetText(ss.str());
	ss.str("");
	ss.clear();
	ss << "Pos: {" << (int)player->Position().x 
		<< "," << (int)player->Position().y 
		<< "," << (int)player->Position().z 
		<< "}  Health: " << player->GetHealth();
	tbPlayerPos->SetText(ss.str());
	//ss.str("");
	//ss.clear();
	//ss << "Enemy Pos: {" << (int)andore->Position().x 
	//	<< "," << (int)andore->Position().y 
	//	<< "," << (int)andore->Position().z 
	//	<< "}  Health: " << andore->GetHealth();
	//tbEnemyPos->SetText(ss.str());


	//Other game logic
	world->Update();
}


void Game::Render()
{
	//SDL_RenderClear( renderer_ );
	world->Draw(renderer());
	SDL_RenderPresent( &renderer() );
}


void Game::Stop()
{
	if(!player->IsDead())
	{
		player->Stop();
		bg->Stop();
	}
}
