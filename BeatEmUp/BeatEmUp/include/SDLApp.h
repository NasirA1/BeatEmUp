#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>


class SDLApp
{
public:
	SDLApp(const int clientWidth, const int clientHeight, const std::string& appTitle);
	virtual ~SDLApp();
	int Run();
	virtual bool Init();
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void ProcessEvent(const SDL_Event& e) = 0;
	

	__forceinline float Fps() const { return fps_; }
	__forceinline unsigned int ClientWidth() const { return clientWidth_; }
	__forceinline unsigned int ClientHeight() const { return clientHeight_; }

	void ToggleFullScreen(); 


protected:
	const unsigned int clientWidth_;
	const unsigned int clientHeight_;
	const std::string appTitle_;
	SDL_Renderer* renderer_;
	SDL_Window*  window_;
	bool quit_;
	bool fullScreen_;

private:
	float fps_;
};
