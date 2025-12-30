#include "Globals.h"
#include "ModuleMenu.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h" 
#include "UIElement.h"
#include "UIButton.h"
#include "ModuleAudio.h"

ModuleMenu::ModuleMenu(Application* app, bool start_enabled) : Module(app, start_enabled), currentScreen(GameScreen::MENU)
{
}

ModuleMenu::~ModuleMenu() {}

bool ModuleMenu::Start()
{
	LOG("Loading Menu Assets");
	bool ret = true;

	centerX = SCREEN_WIDTH / 2 - btnWidth / 2;
	startY = SCREEN_HEIGHT / 2 - btnHeight * 2;

	ChangeScreen(GameScreen::MENU);
	btnSound = LoadSound("Assets/Audio/Music/button.mp3");
	return ret;
}
void ModuleMenu::ChangeScreen(GameScreen newScreen)
{
	App->renderer->ClearUI();

	currentScreen = newScreen;

	App->renderer->camera.zoom = 1.0f;
	App->renderer->camera.target = { 0.0f, 0.0f };
	App->renderer->camera.offset = { 0.0f, 0.0f };
	App->renderer->camera.rotation = 0.0f;

	switch (currentScreen)
	{
	case GameScreen::MENU:
		App->renderer->CreateButton(ID_BTN_PLAY, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 6, (float)btnWidth, (float)btnHeight }, "PLAY", this);
		App->renderer->CreateButton(ID_BTN_SETTINGS, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 7, (float)btnWidth, (float)btnHeight }, "SETTINGS", this);
		App->renderer->CreateButton(ID_BTN_QUIT, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 8, (float)btnWidth, (float)btnHeight }, "QUIT", this);
		break;

	case GameScreen::MAP_SELECT:
		App->renderer->CreateButton(ID_BTN_MAP1, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 6, (float)btnWidth, (float)btnHeight }, "CIRCUIT 1", this);
		App->renderer->CreateButton(ID_BTN_MAP2, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 7, (float)btnWidth, (float)btnHeight }, "CIRCUIT 2", this);
		App->renderer->CreateButton(ID_BTN_BACK, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 8, (float)btnWidth, (float)btnHeight }, "BACK", this);
		break;

	case GameScreen::CONTROLS:
		App->renderer->CreateButton(ID_BTN_SOUND, Rectangle{ (float)centerX, (float)startY, (float)btnWidth, (float)btnHeight }, "SOUND", this);
		App->renderer->CreateButton(ID_BTN_MUSIC, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding), (float)btnWidth, (float)btnHeight }, "MUSIC", this);
		App->renderer->CreateButton(ID_BTN_BACK, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 2, (float)btnWidth, (float)btnHeight }, "BACK", this);
		break;

	case GameScreen::CAR_SELECT:
		App->renderer->CreateButton(ID_BTN_BACK, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 2, (float)btnWidth, (float)btnHeight }, "BACK", this);
		break;

	case GameScreen::GAME:
		break;
	case GameScreen::GAMEOVER:
		App->renderer->CreateButton(ID_BTN_GAMEOVER, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 2, (float)btnWidth, (float)btnHeight }, "Main Menu", this);
	default:
		break;
	}
}

update_status ModuleMenu::Update()
{
	if (pendingChange)
	{
		ChangeScreen(nextScreenToLoad);
		pendingChange = false;
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
	UnloadSound(btnSound);

	return true;
}

void ModuleMenu::OnUIMouseClickEvent(UIElement* element)
{
	bool sound = App->audio->CheckFX();
	if(sound)
	{
		PlaySound(btnSound);
	}
	if (element->id == ID_BTN_PLAY)
	{
		pendingChange = true;
		nextScreenToLoad = GameScreen::MAP_SELECT;
	}
	else if (element->id == ID_BTN_SETTINGS)
	{
		pendingChange = true;
		nextScreenToLoad = GameScreen::CONTROLS;
	}
	else if (element->id == ID_BTN_MAP1)
	{
		App->scene_intro->mapPath = "Assets/map/CuteRacing.tmx";
		App->scene_intro->levelLoaded = 1;
		pendingChange = true;
		nextScreenToLoad = GameScreen::GAME;
	}
	else if (element->id == ID_BTN_MAP2)
	{
		App->scene_intro->mapPath = "Assets/map/CuteRacing_Level2.tmx";
		App->scene_intro->levelLoaded = 2;
		pendingChange = true;
		nextScreenToLoad = GameScreen::GAME;
	}
	else if (element->id == ID_BTN_BACK)
	{
		pendingChange = true;

		if (currentScreen == GameScreen::MAP_SELECT)
		{
			nextScreenToLoad = GameScreen::MENU;
		}
		else if (currentScreen == GameScreen::CAR_SELECT)
		{
			nextScreenToLoad = GameScreen::MAP_SELECT; 
		}
		else
		{
			nextScreenToLoad = GameScreen::MENU;
		}
	}
	 if(element->id ==ID_BTN_MUSIC)
	{
		
		
		App->audio->MusicOn();
		
	}
	else if (element->id == ID_BTN_SOUND)
	{
	
		App->audio->FXOn();
		
	}
	else if (element->id == ID_BTN_QUIT && currentScreen == GameScreen::MENU)
	{
		exitGame = true;
	}
	else if (element->id == ID_BTN_GAMEOVER && currentScreen == GameScreen::GAMEOVER)
	{
		pendingChange = true;
		nextScreenToLoad = GameScreen::MENU;
	}
}