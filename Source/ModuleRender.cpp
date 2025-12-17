#include "Globals.h"
#include "Application.h"
#include "ModuleGame.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleMenu.h"
#include "UIButton.h"
#include <math.h>

ModuleRender::ModuleRender(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	background = DARKGREEN;
}

// Destructor
ModuleRender::~ModuleRender()
{}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");
	bool ret = true;

	circle = LoadTexture("Assets/wheel.png");
	box = LoadTexture("Assets/crate.png");

	//Cars
	burgerCar = LoadTexture("Assets/Cars/BurgerCar/burger_car.png");
	normalCar = LoadTexture("Assets/Cars/NormalCars/normal_car_yellow.png");
	wheel = LoadTexture("Assets/Cars/NormalCars/wheel_animation_spritesheet.png");

	//Scenes
	menu = LoadTexture("Assets/Scenes/menu_bg.png");



	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRender::PreUpdate()
{
	BeginDrawing();
	BeginMode2D(camera);

	switch (App->menu->currentScreen) {

	case GameScreen::MENU:

		DrawTexturePro(menu, Rectangle{ 0, 0, (float)menu.width, (float)menu.height }, Rectangle{ 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT }, Vector2{ 0, 0 }, 0.0f, WHITE);
		break;
	}
	return UPDATE_CONTINUE;
}

// Update: debug camera
update_status ModuleRender::Update()
{
	for (UIElement* element : uiElements)
	{
		element->Update(1.0f / 60.0f);
	}
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRender::PostUpdate()
{
	EndMode2D();

	for (UIElement* element : uiElements)
	{
		element->Draw();
	}

	//Draw all the textures of every scene
	switch (App->menu->currentScreen) {

	case GameScreen::CONTROLS:


		break;
	case GameScreen::GAMEOVER:


		break;
	}
	// Draw everything in our batch!
	DrawFPS(10, 10);

	EndDrawing();

	return UPDATE_CONTINUE;
}

// Called before quitting
void ModuleRender::ClearUI()
{
	for (UIElement* element : uiElements)
	{
		delete element;
	}
	uiElements.clear();
}

bool ModuleRender::CleanUp()
{
	ClearUI();
	UnloadTexture(circle);
	UnloadTexture(box);

	//Cars
	UnloadTexture(burgerCar);
	UnloadTexture(normalCar);
	UnloadTexture(wheel);

	//Scenes
	UnloadTexture(menu);


	return true;
}

void ModuleRender::SetBackgroundColor(Color color)
{
	background = color;
}

UIElement* ModuleRender::CreateButton(int id, Rectangle bounds, const char* text, Module* observer)
{
	UIButton* newButton = new UIButton(id, bounds, text);

	newButton->SetObserver(observer);

	uiElements.push_back(newButton);

	return newButton;
}

// Draw to screen
bool ModuleRender::Draw(Texture2D texture, int x, int y, const Rectangle* section, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;

	float scale = 1.0f;
	Vector2 position = { (float)x, (float)y };
	Rectangle rect = { 0.f, 0.f, (float)texture.width, (float)texture.height };

	if (section != NULL) rect = *section;

	position.x = (float)(x - pivot_x) * scale;
	position.y = (float)(y - pivot_y) * scale;

	rect.width *= scale;
	rect.height *= scale;

	DrawTextureRec(texture, rect, position, WHITE);

	return ret;
}

bool ModuleRender::DrawText(const char* text, int x, int y, Font font, int spacing, Color tint) const
{
	bool ret = true;

	Vector2 position = { (float)x, (float)y };

	DrawTextEx(font, text, position, (float)font.baseSize, (float)spacing, tint);

	return ret;
}
