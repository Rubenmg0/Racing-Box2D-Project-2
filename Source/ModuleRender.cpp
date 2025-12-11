#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
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

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRender::PreUpdate()
{
	return UPDATE_CONTINUE;
}

// Update: debug camera
update_status ModuleRender::Update()
{
	ClearBackground(background);
	
	//camera.target = App->scene_intro->player->GetPosition();
	camera.offset = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.f;

	EndMode2D();
	BeginMode2D(camera);
	BeginDrawing();

	for (UIElement* element : uiElements)
	{
		element->Update(1.0f / 60.0f);
	}
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRender::PostUpdate()
{
	// Draw everything in our batch!
	DrawFPS(10, 10);
	for (UIElement* element : uiElements)
	{
		element->Draw();
	}

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

	position.x = (float)(x - pivot_x) * scale + camera.target.x;
	position.y = (float)(y - pivot_y) * scale + camera.target.y;

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
