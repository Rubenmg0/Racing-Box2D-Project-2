#include "UIButton.h"
#include "raylib.h"

UIButton::UIButton(int id, Rectangle bounds, const char* text) : UIElement(UIElementType::BUTTON, id, bounds, text)
{
	this->bounds = bounds;
	this->text = text;

	drawBasic = false;
}

UIButton::~UIButton(){}

bool UIButton::Update(float dt)
{
	if (state != UIElementState::DISABLED)
	{
		Vector2 mousePos = GetMousePosition();

		if (CheckCollisionPointRec(mousePos, bounds))
		{
			state = UIElementState::FOCUSED;

			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
			{
				state = UIElementState::PRESSED;
			}

			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
			{
				NotifyObserver();
			}
		}
		else
		{
			state = UIElementState::NORMAL;
		}
	}

	return true;
}

bool UIButton::Draw()
{
	//Button Colors
	Color colorNormal = BLUE; 
	Color colorFocused = SKYBLUE; 
	Color colorPressed = DARKBLUE;  

	Color drawColor = colorNormal;

	switch (state)
	{
	case UIElementState::NORMAL:  drawColor = colorNormal; break;
	case UIElementState::FOCUSED: drawColor = colorFocused; break;
	case UIElementState::PRESSED: drawColor = colorPressed; break;
	case UIElementState::DISABLED: drawColor = LIGHTGRAY; break;
	}

	//Rectamgle Rouned varibales
	float roundness = 0.5f; 
	int segments = 10;      

	DrawRectangleRounded(bounds, roundness, segments, drawColor);

	if (!text.empty())
	{
		int fontSize = 20;
		int textWidth = MeasureText(text.c_str(), fontSize);

		int textX = (int)(bounds.x + (bounds.width - textWidth) / 2);
		int textY = (int)(bounds.y + (bounds.height - fontSize) / 2);

		DrawText(text.c_str(), textX + 2, textY + 2, fontSize, BLACK);
		DrawText(text.c_str(), textX, textY, fontSize, WHITE);
	}

	return true;
}