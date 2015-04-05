#pragma once
#include "GameObject.h"
#include <SDL_ttf.h>
#include <memory>


class TextBlock : public GameObject
{
public:
	TextBlock(const string& text_, size_t size, float x, float y, SDL_Renderer* const renderer);
	virtual ~TextBlock(void);
	virtual void Update() override;
	virtual void Draw(SDL_Renderer* const renderer) const override;

	__forceinline string GetText() const { return text; }
	__forceinline void SetText(const string& txt) { text = txt; }
	__forceinline const SDL_Colour& GetColour() const { return colour; }
	
	__forceinline void SetColour(Uint8 r_, Uint8 g_, Uint8 b_, Uint8 a_)
	{
		colour.r = r_;
		colour.g = g_;
		colour.b = b_;
		colour.a = a_;
	}


private:
	unique_ptr<TTFont> font;
	string text;
	SDL_Colour colour;
};
