#pragma once
#include "Module.h"
#include "Timer.h"
#include "Globals.h"
#include "raylib.h"

struct UIElement;

enum class GameScreen {
	MENU,
	CONTROLS,
	CREDITS,
	MAP_SELECT,
	CAR_SELECT,
	GAME,
	GAMEOVER
};

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

	void ChangeScreen(GameScreen newScreen);
public:
	bool exitGame = false;

	GameScreen currentScreen;
	bool pendingChange = false;
	GameScreen nextScreenToLoad;

	//Button Size
	int btnWidth = 200;
	int btnHeight = 40;

	//Button Position
	int centerX;
	int startY;
	int padding = 10;

	//IDs para identificar los botones
	//IDs Game
	const int ID_BTN_PLAY = 1;
	const int ID_BTN_CAR1 = 2;
	const int ID_BTN_CAR2 = 3;
	const int ID_BTN_MAP1 = 4;
	const int ID_BTN_MAP2 = 5;

	//IDs Settings
	const int ID_BTN_SETTINGS = 6;
	const int ID_BTN_SOUND = 7;
	const int ID_BTN_MUSIC = 8;
	
	//IDs Extra
	const int ID_BTN_BACK = 9;
	const int ID_BTN_QUIT = 10;
	const int ID_BTN_CREDITS = 11;

	Timer timer;
};