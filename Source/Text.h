#pragma once
#include "Globals.h"
#include <string>
#include "raylib.h"

//To visualize all the text in the game
class Text 
{
public:

	Text();
	~Text();

	void LoadFont(const char* path, int fontSize);

	//Draw the text at a specified position
	void Draw(int x, int y, const std::string& msg, Color color = WHITE, int limit = 0) const;

private:
	Font gameFont;
	float baseSize;
};