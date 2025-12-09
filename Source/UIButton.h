#pragma once

#include "UIElement.h"

class UIButton : public UIElement
{

public:

	UIButton(int id, Rectangle bounds, const char* text);	
	virtual ~UIButton();

	bool Update(float dt);
	bool Draw() override;
private:

	bool canClick = true;
	bool drawBasic = false;
};
