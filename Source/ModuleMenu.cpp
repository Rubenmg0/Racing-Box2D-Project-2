#include "ModuleMenu.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h" 
#include "UIElement.h"

ModuleMenu::ModuleMenu(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleMenu::~ModuleMenu() {}

bool ModuleMenu::Start()
{
	LOG("Loading Menu Assets");
	bool ret = true;

	bgTexture = LoadTexture("Assets/Scenes/menu_bg.png");
	// Button Size
	int btnWidth = 200;
	int btnHeight = 40;

	// Button Position
	int centerX = SCREEN_WIDTH / 2 - btnWidth / 2;
	int startY = SCREEN_HEIGHT / 2 - btnHeight * 2; 
	int padding = 10;

	// PLAY Button
	App->renderer->CreateButton(ID_BTN_PLAY, Rectangle{ (float)centerX, (float)startY, (float)btnWidth, (float)btnHeight }, "PLAY", this);

	// SETTINGS Button
	App->renderer->CreateButton(ID_BTN_SETTINGS, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding), (float)btnWidth, (float)btnHeight }, "SETTINGS", this);

	// QUIT Button
	App->renderer->CreateButton(ID_BTN_QUIT, Rectangle{ (float)centerX, (float)startY + (btnHeight + padding) * 2, (float)btnWidth, (float)btnHeight }, "QUIT", this); 
	
	return ret;
}

update_status ModuleMenu::Update()
{
	if (moveToGame)
	{
		App->scene_intro->Enable();
		this->Disable();
		moveToGame = false;
		return UPDATE_CONTINUE;
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
		moveToGame = true;
	}
	else if (element->id == ID_BTN_QUIT)
	{
		exitGame = true;
	}
}