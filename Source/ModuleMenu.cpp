#include "ModuleMenu.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h" 
#include "UIElement.h"
#include "UIButton.h"

ModuleMenu::ModuleMenu(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleMenu::~ModuleMenu() {}

bool ModuleMenu::Start()
{
	LOG("Loading Menu Assets");
	bool ret = true;

	bgTexture = LoadTexture("Assets/Scenes/menu_bg.png");

	return ret;
}

update_status ModuleMenu::Update()
{
	switch (currentScreen)
	{
	case MENU:
		//PLAY Button
		App->renderer->CreateButton(ID_BTN_PLAY, Rectangle{ (float)centerX, (float)startY, (float)btnWidth, (float)btnHeight }, "PLAY", this);

		//SETTINGS Button
		App->renderer->CreateButton(ID_BTN_SETTINGS, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding), (float)btnWidth, (float)btnHeight }, "SETTINGS", this);

		//QUIT Button
		App->renderer->CreateButton(ID_BTN_QUIT, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 2, (float)btnWidth, (float)btnHeight }, "QUIT", this);
		
		break;
	case CONTROLS:
		//SOUND Button
		App->renderer->CreateButton(ID_BTN_SOUND, Rectangle{ (float)centerX, (float)startY, (float)btnWidth, (float)btnHeight }, "SOUND", this);
		
		//MUSIC Button
		App->renderer->CreateButton(ID_BTN_MUSIC, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding), (float)btnWidth, (float)btnHeight }, "MUSIC", this);

		//BACK Button
		App->renderer->CreateButton(ID_BTN_BACK, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 2, (float)btnWidth, (float)btnHeight }, "BACK", this);

		break;
	case CREDITS:


		break;
	case CAR_SELECT:


		//BACK Button
		App->renderer->CreateButton(ID_BTN_BACK, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 2, (float)btnWidth, (float)btnHeight }, "BACK", this);

		break;
	case MAP_SELECT:


		//BACK Button
		App->renderer->CreateButton(ID_BTN_BACK, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 2, (float)btnWidth, (float)btnHeight }, "BACK", this);

		break;
	case GAME:
		this->Disable();

		break;
	case GAMEOVER:


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
	DrawTexturePro(bgTexture, Rectangle{ 0, 0, (float)bgTexture.width, (float)bgTexture.height }, Rectangle{ 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT }, Vector2{ 0, 0 }, 0.0f, WHITE);

	return UPDATE_CONTINUE;
}

bool ModuleMenu::CleanUp()
{
	LOG("Unloading Menu Scene");
	UnloadTexture(bgTexture);

	App->renderer->ClearUI();

	return true;
}

void ModuleMenu::OnUIMouseClickEvent(UIElement* element)
{
	if (element->id == ID_BTN_PLAY)
	{
		currentScreen = GameScreen::CAR_SELECT;
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