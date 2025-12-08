#pragma once
#include "Module.h"
#include "Globals.h"
#include "raylib.h"

struct UIElement;

class ModuleMenu : public Module
{
public:
	ModuleMenu(Application* app, bool start_enabled = true);
	~ModuleMenu();

	bool Start();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	// Callback para los botones
	void OnUIMouseClickEvent(UIElement* element);

public:
	Texture2D bgTexture; // Para la imagen de fondo
	bool moveToGame = false;
	bool exitGame = false;
	// IDs para identificar los botones
	const int ID_BTN_PLAY = 1;
	const int ID_BTN_SETTINGS = 2;
	const int ID_BTN_CREDITS = 3;
	const int ID_BTN_QUIT = 4;
};