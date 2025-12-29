#pragma once
#include "Module.h"
#include "Text.h"
#include "Timer.h"
#include "Globals.h"
#include <vector>
#include <limits.h>

class UIElement;

class ModuleRender : public Module
{
public:
	ModuleRender(Application* app, bool start_enabled = true);
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void ClearUI();

    void SetBackgroundColor(Color color);
	bool Draw(Texture2D texture, int x, int y, const Rectangle* section = NULL, double angle = 0, int pivot_x = 0, int pivot_y = 0) const;
    bool DrawText(const char* text, int x, int y, Font font, int spacing, Color tint) const;
	bool DrawSemaphore(Texture2D texture, int state, int frameWidth, int frameHeight, float scale);

	UIElement* CreateButton(int id, Rectangle bounds, const char* text, Module* observer);
	std::vector<UIElement*> uiElements;

public:

	Color background;
	Camera2D camera;

	Texture2D circle;
	Texture2D box;

	//Cars texture
	Texture2D burgerCar;
	Texture2D normalCar;
	Texture2D wheel;

	//Scenes textures
	Texture2D menu;

	Text timerChrono;
	Timer timer;
	Texture2D chrono_base;
	bool resetTime = false;

	//Music
	uint32 menuMusic;
	uint32 gameMusic;
	uint32 endMusic;
};