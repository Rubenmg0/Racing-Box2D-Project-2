#include "Globals.h"
#include "ModuleMenu.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h" 
#include "UIElement.h"
#include "UIButton.h"

ModuleMenu::ModuleMenu(Application* app, bool start_enabled) : Module(app, start_enabled), currentScreen(GameScreen::MENU)
{
}

ModuleMenu::~ModuleMenu() {}

bool ModuleMenu::Start()
{
	LOG("Loading Menu Assets");
	bool ret = true;

	return ret;
}

update_status ModuleMenu::Update()
{
	App->renderer->camera.zoom = 1.0f;
	App->renderer->camera.target = { 0.0f, 0.0f };
	App->renderer->camera.offset = { 0.0f, 0.0f };
	App->renderer->camera.rotation = 0.0f;

	switch (currentScreen)
	{
	case GameScreen::MENU:
		this->Enable();

		//PLAY Button
		App->renderer->CreateButton(ID_BTN_PLAY, Rectangle{ (float)centerX, (float)startY, (float)btnWidth, (float)btnHeight }, "PLAY", this);
		
		//SETTINGS Button
		App->renderer->CreateButton(ID_BTN_SETTINGS, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding), (float)btnWidth, (float)btnHeight }, "SETTINGS", this);

		//QUIT Button
		App->renderer->CreateButton(ID_BTN_QUIT, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 2, (float)btnWidth, (float)btnHeight }, "QUIT", this);
		
		break;
	case GameScreen::CONTROLS:
		//SOUND Button
		App->renderer->CreateButton(ID_BTN_SOUND, Rectangle{ (float)centerX, (float)startY, (float)btnWidth, (float)btnHeight }, "SOUND", this);
		
		//MUSIC Button
		App->renderer->CreateButton(ID_BTN_MUSIC, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding), (float)btnWidth, (float)btnHeight }, "MUSIC", this);

		//BACK Button
		App->renderer->CreateButton(ID_BTN_BACK, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 2, (float)btnWidth, (float)btnHeight }, "BACK", this);

		break;
	case GameScreen::CREDITS:


		break;
	case GameScreen::CAR_SELECT:


		//BACK Button
		App->renderer->CreateButton(ID_BTN_BACK, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 2, (float)btnWidth, (float)btnHeight }, "BACK", this);

		break;
	case GameScreen::MAP_SELECT:


		//BACK Button
		App->renderer->CreateButton(ID_BTN_BACK, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 2, (float)btnWidth, (float)btnHeight }, "BACK", this);

		break;
	case GameScreen::GAME:
		this->Disable();

		break;
	case GameScreen::GAMEOVER:


		break;
	default:
		break;
	}
	
	if (exitGame)
	{
		return UPDATE_STOP;
	}
	return UPDATE_CONTINUE;
}

update_status ModuleMenu::PostUpdate()
{

	return UPDATE_CONTINUE;
}

bool ModuleMenu::CleanUp()
{
	LOG("Unloading Menu Scene");
	App->renderer->ClearUI();

	return true;
}

void ModuleMenu::OnUIMouseClickEvent(UIElement* element)
{
	if (element->id == ID_BTN_PLAY)
	{
		currentScreen = GameScreen::GAME; //CAR_SELECT
	}

	else if (element->id == (ID_BTN_CAR1 || ID_BTN_CAR2)) 
	{
		currentScreen = GameScreen::MAP_SELECT;
	}

	else if (element->id == (ID_BTN_MAP1 || ID_BTN_MAP2))
	{
		currentScreen = GameScreen::GAME;
	}

	else if (element->id == ID_BTN_SETTINGS)
	{
		currentScreen = GameScreen::CONTROLS;
	}

	else if (element->id == ID_BTN_BACK)
	{
		if (currentScreen == GameScreen::CAR_SELECT || currentScreen == GameScreen::CONTROLS || currentScreen == GameScreen::CREDITS) currentScreen = GameScreen::MENU;
		else if (currentScreen == GameScreen::MAP_SELECT) currentScreen = GameScreen::CAR_SELECT;
	}

	else if (element->id == ID_BTN_QUIT && currentScreen == GameScreen::MENU)
	{
		exitGame = true;
	}
	
}