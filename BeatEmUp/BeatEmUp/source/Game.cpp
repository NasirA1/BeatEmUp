#include "Game.h"
#include <sstream>
#include <algorithm>
#include "Mixer.h"


Game::Game() 
	: SDLApp(SCREEN_WIDTH
	, SCREEN_HEIGHT
	, "Nasir's Beat 'em Up Game")
	, MoveBounds(0.0f, 370.0f, (float)SCREEN_WIDTH, 120.0f)
	, bg(nullptr)
	, tbFps(nullptr), tbPlayerPos(nullptr), tbEnemyPos(nullptr)
	, player(nullptr)
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

	//optimisation - set desired size
	gameObjects.reserve(20);
	enemies.reserve(20);
	
	//Create and add player
	//currently only one character (baddude) supported
	player = new Player(renderer());
	gameObjects.push_back(player);

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


void Game::CleanupLevel()
{
	/* Player object never gets deleted */
	if(gameObjects.size() <= 1)
		return;

	//Garbage-collect
	for (auto it = gameObjects.begin(); it != gameObjects.end(); )
	{
		if ((*it)->GetType() != GameObject::GT_Player)
		{
			util::Delete(*it);
			it = gameObjects.erase(it);
		}
		else
		{
			++it;
		}
	}

	enemies.clear();
	logPrintf("Level{%lu} Cleaned up.  gameObjects<%d>", currentLevel, gameObjects.size());
}


bool Game::LevelComplete() const
{
	//all enemies destroyed!
	return enemies.size() <= 0;
}


bool Game::LoadNextLevel()
{
	if(currentLevel < MaxLevel) currentLevel++;
	else return false;
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
			bg = new Background(clientWidth_, clientHeight_, renderer(), 
				"resources/bg1.gif", "resources/bg2.gif", "resources/bg3.gif");

			Roamer* skaterboy = new Roamer(renderer(), 
				Sprite::FromFile("resources/skater_left.png", renderer(), 71, 90, 11, 0),
				Sprite::FromFile("resources/skater_right.png", renderer(), 71, 90, 11, 0), 
				-200, 390, -200, 1000, true);
			Roamer* knight1 = new Roamer(renderer(), 
				Sprite::FromFile("resources/knightwalk_left.png", renderer(), 128, 128, 4, 15),
				Sprite::FromFile("resources/knightwalk_right.png", renderer(), 128, 128, 4, 3), 
				5000, 480, -5000, 5000, false);

			enemies.push_back(new Andore(renderer(), 1200, 450));
			enemies.push_back(new Andore(renderer(), 2400, 450));
			enemies.push_back(new Joker(renderer(), 1000, 400));
			enemies.push_back(new Axl(renderer(), 800, 400));
			enemies.push_back(new Andore(renderer(), 700, 380));
			enemies.push_back(new Axl(renderer(), -200, 400));
			enemies.push_back(new Joker(renderer(), 1100, 400));
			enemies.push_back(new Axl(renderer(), 500, 400));

			tbFps = new TextBlock("FPS: 00.000000", 16, 0.0f, 0.0f, renderer());	
			tbPlayerPos = new TextBlock("Pos {}", 16, 0.0f, tbFps->Position().bottom() + 1, renderer());
			tbEnemyPos = new TextBlock("Enemy Pos {}", 16, 0.0f, tbPlayerPos->Position().bottom() + 1, renderer());

			gameObjects.push_back(bg);
			gameObjects.push_back(tbFps);
			gameObjects.push_back(tbPlayerPos);
			gameObjects.push_back(tbEnemyPos);
			gameObjects.push_back(skaterboy);
			gameObjects.push_back(knight1);

			for(unsigned int i = 0; i < enemies.size(); ++i)
				gameObjects.push_back(enemies[i]);

			gameObjects.push_back(new Rock("resources/rock.png", renderer()));
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

	logPrintf("Level{%lu} Loaded.  gameObjects<%d>", currentLevel, gameObjects.size());
	return true;
}


void Game::Update()
{
	//Level management
	if(player->IsDead())
	{
		//TODO
		CleanupLevel();
		logPrintf("GAME OVER!");
		//end the game
		//try again? yes/no
		//Resurrect player
		return;
	}
	else if(LevelComplete())
	{
		logPrintf("Level{%lu} Completed", currentLevel);

		if(currentLevel == MaxLevel)
		{
			//TODO
			//CleanupLevel();
			//game complete
			//end the game
			//Play end credits
			logPrintf("*** GAME COMPLETED ***");
			return;
		}
		else
		{
			CleanupLevel();
			LoadNextLevel();
		}
	}

	//Gameplay
	//Update movement vectors
	if (upDown) player->GoUp();
	if (downDown) player->GoDown();
	if (rightDown) player->GoRight();
	if (leftDown) player->GoLeft();
	bg->SetScroll(player->GetState() == Player::PS_Walking
		|| player->GetState() == Player::PS_Jumping);

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
	std::for_each(gameObjects.begin(), gameObjects.end(), [](auto& obj) { obj->Update(); });
}


void Game::Render()
{
	//SDL_RenderClear( renderer_ );
	//Sort by depth, then draw
	std::sort(gameObjects.begin(), gameObjects.end(), GameObjectSortByDepth());
	std::for_each(gameObjects.begin(), gameObjects.end(), [&](const auto& obj) { obj->Draw(renderer()); });
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
