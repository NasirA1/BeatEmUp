#include "Text.h"




TextBlock::TextBlock(const string& text_, size_t size, float x, float y, SDL_Renderer* const renderer_)
	: GameObject()
	, text(text_)
	, font(new TTFont("resources/calibri.ttf", size))
{
	position.x = x, position.y = y;
	int w = 0, h = 0;
	TTF_SizeText(font->font, text.c_str(), &w, &h);
	position.w = (float)w, position.h = (float)h;	
	SetColour(0, 0, 0, 0);
}



TextBlock::~TextBlock(void)
{
}



void TextBlock::Update()
{
	int w = 0, h = 0;
	TTF_SizeText(font->font, text.c_str(), &w, &h);
	position.w = (float)w, position.h = (float)h;	
}



void TextBlock::Draw(SDL_Renderer* const renderer) const
{
	if(!font) return;

	TextTexture texture(renderer, *font, text, colour);

	SDL_Rect nPos;
	util::Convert(position, nPos);
	SDL_RenderCopyEx(renderer, texture.texture, NULL, &nPos, GetAngle(), NULL, SDL_FLIP_NONE);
}
