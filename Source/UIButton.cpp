#include "UIButton.h"
#include "raylib.h"

UIButton::UIButton(int id, Rectangle bounds, const char* text) : UIElement(UIElementType::BUTTON, id, bounds, text)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
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
				NotifyObserver(); // ¡Avisa a ModuleGame!
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
	Color drawColor = color; 

	switch (state)
	{
	case UIElementState::NORMAL:
		drawColor = RED;
		break;
	case UIElementState::FOCUSED:
		drawColor = MAROON; 
		break;
	case UIElementState::PRESSED:
		drawColor = DARKGRAY;
		break;
	case UIElementState::DISABLED:
		drawColor = LIGHTGRAY;
		break;
	}

	DrawRectangleRec(bounds, drawColor);

	DrawRectangleLinesEx(bounds, 2, BLACK);

	if (!text.empty())
	{
		int fontSize = 16;
		int textWidth = MeasureText(text.c_str(), fontSize);

		// Calcular posición para centrar exacto
		int textX = (int)(bounds.x + (bounds.width - textWidth) / 2);
		int textY = (int)(bounds.y + (bounds.height - fontSize) / 2);

		DrawText(text.c_str(), textX, textY, fontSize, WHITE);
	}

	return true;
}