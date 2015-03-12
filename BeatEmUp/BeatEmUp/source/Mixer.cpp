#include "Mixer.h"
#include "Util.h"
#include <string>


using namespace std;


void LoadChunk(Mixer& mixer, Mixer::SoundEffect effect, const string& file)
{
	if(mixer.effects.find(effect) != mixer.effects.end())
	{
		logPrintf( "Sound effect <%s> already loaded!", file.c_str() );
		return;
	}

	Mix_Chunk* chunk = Mix_LoadWAV(file.c_str());
	if(chunk)
	{
		mixer.effects[effect] = chunk;
	}
	else
	{
		logPrintf( "Failed to load '%s' sound effect! SDL_mixer Error: %s", file.c_str(), Mix_GetError() );
	}
}


void LoadMusic(Mixer& mixer, Mixer::SoundTrack track, const string& file)
{
	mixer.tracks[track] = file;
}


Mixer::Mixer()
	: currentTrack(NULL)
{
	//Load sound effects
	LoadChunk(*this, SE_Kick, "resources/kick.wav");
	LoadChunk(*this, SE_Punch, "resources/punch.wav");
	LoadChunk(*this, SE_Grunt, "resources/grunt.wav");
	LoadChunk(*this, SE_DragonRoar, "resources/dragonroar.wav");

	//Load tracks1
	LoadMusic(*this, ST_Track1, "resources/aldebaran.mp3");
}


Mixer::~Mixer(void)
{
	for(std::map< SoundEffect, Mix_Chunk* >::iterator it = effects.begin();
		it != effects.end(); ++it)
	{
		Mix_FreeChunk( it->second );
		it->second = NULL;
	}
}


void Mixer::Play(Mixer::SoundEffect effect)
{
	map< Mixer::SoundEffect, Mix_Chunk* >::iterator it = effects.find(effect);
	if(it != effects.end() && it->second)
	{
		Mix_PlayChannel(-1, it->second, 0);
	}
}


void Mixer::Play(Mixer::SoundTrack track)
{
	if(currentTrack)
	{
		Mix_FreeMusic(currentTrack);
		currentTrack = NULL;
	}

	map< Mixer::SoundTrack, string >::iterator it = tracks.find(track);
	if(it != tracks.end())
	{
		currentTrack = Mix_LoadMUS(it->second.c_str());
		if(currentTrack)
		{
			logPrintf("Playing sound track...");
			Mix_PlayMusic(currentTrack, -1);
		}
		else
		{
			logPrintf( "Failed to load track '%s'! SDL_mixer Error: %s", it->second.c_str(), Mix_GetError() );
		}
	}
}
