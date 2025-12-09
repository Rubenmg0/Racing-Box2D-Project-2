#pragma once

#include "Module.h"
#include "raylib.h"
#include <string>

enum class UIElementType
{
	BUTTON,
	TOGGLE,
	CHECKBOX,
	SLIDER,
	SLIDERBAR,
	COMBOBOX,
	DROPDOWNBOX,
	INPUTBOX,
	VALUEBOX,
	SPINNER
};

enum class UIElementState
{
	DISABLED,
	NORMAL,
	FOCUSED,
	PRESSED,
	SELECTED
};

class UIElement {
public:

	UIElement() {}

	UIElement(UIElementType type, int id, Rectangle bounds, const char* text) : type(type), id(id), bounds(bounds), text(text), state(UIElementState::NORMAL)
	{
		color = WHITE; 
		texture = { 0 };
	}

	virtual ~UIElement() {}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual bool Draw()
	{
		return true;
	}

	void SetObserver(Module* module)
	{
		observer = module;
	}

	void NotifyObserver()
	{
		observer->OnUIMouseClickEvent(this);
	}

public:
	int id;
	UIElementType type;
	UIElementState state;

	std::string text;
	Rectangle bounds; //Size, x,y and then width and Height
	Color color;
	Texture2D texture;

	Module* observer; 
};