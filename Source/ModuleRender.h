#pragma once
#include "Module.h"
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

	UIElement* CreateButton(int id, Rectangle bounds, const char* text, Module* observer);
	std::vector<UIElement*> uiElements;

public:

	Color background;
	Camera2D camera = {
		camera.offset = { 200, 200 },
		camera.target = { 0, 0 },
		camera.rotation = 0.0f,
		camera.zoom = 1.0f,
	};
};