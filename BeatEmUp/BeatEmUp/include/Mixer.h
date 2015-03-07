#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include "Util.h"
#include <map>
#include <string>

#define MIXER	Mixer::Instance()


class Mixer : public Singleton<Mixer>
{
public:
	enum SoundEffect
	{
		SE_DragonRoar,
		SE_Grunt,
		SE_Kick,
		SE_Punch
	};

	enum SoundTrack
	{
		ST_Track1,
	};

	Mixer(void);
	~Mixer(void);
	void Play(SoundEffect effect);
	void Play(SoundTrack track);

	friend void LoadChunk(Mixer& mixer, Mixer::SoundEffect effect, const std::string& file);
	friend void LoadMusic(Mixer& mixer, Mixer::SoundTrack track, const std::string& file);


private:
	std::map< SoundEffect, Mix_Chunk* > effects;
	std::map< SoundTrack, std::string > tracks;
	Mix_Music* currentTrack;
};
