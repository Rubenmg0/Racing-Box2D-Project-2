#include "Globals.h"
#include "Application.h"
#include "ModuleGame.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleMenu.h"
#include "ModuleAudio.h"
#include "UIButton.h"
#include <math.h>

ModuleRender::ModuleRender(Application* app, bool start_enabled) : Module(app, start_enabled)
{
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
	normalCar = LoadTexture("Assets/Cars/BurgerCar/burger_car.png");

	aiCarTextures[0] = LoadTexture("Assets/Cars/NormalCars/normal_car_green.png");
	aiCarTextures[1] = LoadTexture("Assets/Cars/NormalCars/normal_car_blue.png");
	aiCarTextures[2] = LoadTexture("Assets/Cars/NormalCars/normal_car_cian.png");
	aiCarTextures[3] = LoadTexture("Assets/Cars/NormalCars/normal_car_pink.png");

	wheel = LoadTexture("Assets/Cars/NormalCars/wheel_animation_spritesheet.png");

	//Scenes
	menu = LoadTexture("Assets/Scenes/menu.png");
	controls = LoadTexture("Assets/Scenes/controls.png");
	victory = LoadTexture("Assets/Scenes/Victory.png");
	defeat = LoadTexture("Assets/Scenes/Defeat.png");

	//Text
	timerChrono.LoadFont("Assets/Font/alagard.ttf", 48);

	chrono_base = LoadTexture("Assets/Scenes/Base_Chrono.png");
	chrono_base2 = LoadTexture("Assets/Scenes/Base_Chrono.png");

	//Load music and sfx (No inicia nada, falta poner alguna musica en la carpeta)
	menuMusic = App->audio->LoadFx("Assets/Audio/menuMusic.wav");
	gameMusic = App->audio->LoadFx("Assets/Audio/gameMusic.wav");
	endMusic = App->audio->LoadFx("Assets/Audio/endMusic.wav");

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
	case GameScreen::CONTROLS:

		DrawTexturePro(controls, Rectangle{ 0, 0, (float)controls.width, (float)controls.height }, Rectangle{ 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT }, Vector2{ 0, 0 }, 0.0f, WHITE);
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

	case GameScreen::GAME:
	{
		if (!resetTime) {
			timer.Start();
			//App->audio->PlayFx(gameMusic);
			resetTime = true;
		}

		Vector2 position = { 960.0f, -2.0f };
		float rotation = 0.0f;
		float scale = 0.47f;

		DrawTextureEx(chrono_base, position, rotation, scale, WHITE);

		double totalSeconds = timer.ReadSec();

		int minutes = (int)totalSeconds / 60;
		int seconds = (int)totalSeconds % 60;
		int centiseconds = (int)((totalSeconds - (int)totalSeconds) * 100);

		//Chrono
		const char* formTime = TextFormat("%02d:%02d.%02d", minutes, seconds, centiseconds);

		timerChrono.Draw(1025, 75, formTime, WHITE, 8);

		//Laps
		const char* lapText = TextFormat("LAP %i/%i", App->scene_intro->completedLaps + 1, App->scene_intro->totalLaps);

		timerChrono.Draw(100, 65, lapText, YELLOW, 8);

		//Checkpoints
		if (App->physics->IsDebug() || App->scene_intro->checkpointFeedbackTimer > 0.0f)
		{
			int currentCP = App->scene_intro->nextCheckpointRequired - 1;
			if (currentCP < 0) currentCP = 0;

			const char* cpText = TextFormat("Checkpoints: %i/%i", currentCP, App->scene_intro->totalCheckpoints - 1);

			Color textColor = App->physics->IsDebug() ? ORANGE : GREEN;

			timerChrono.Draw(100, 110, cpText, textColor, 20);
		}
	}
	break;
	case GameScreen::GAMEOVER:

		timer.Stop();

		Vector2 position = { -40, -38 };
		float rotation = 0.0f;
		float scale = 0.56f;

		gameover = victory;

		DrawTextureEx(gameover, position, rotation, scale, WHITE);
		DrawTextureEx(chrono_base2, { 20, 360 }, 0.0f, 2.81f, WHITE);

		double totalSeconds = timer.ReadSec();

		int minutes = (int)totalSeconds / 60;
		int seconds = (int)totalSeconds % 60;
		int centiseconds = (int)((totalSeconds - (int)totalSeconds) * 100);

		if (minutes < bestTime[0])
		{
			bestTime[0] = minutes;
			bestTime[1] = seconds;
			bestTime[2] = centiseconds;
		}
		else if (minutes == bestTime[0] && seconds < bestTime[1])
		{
			bestTime[1] = seconds;
			bestTime[2] = centiseconds;
		}
		else if (minutes == bestTime[0] && seconds == bestTime[1] && centiseconds < bestTime[2])
		{
			bestTime[2] = centiseconds;
		}

		//Chrono
		const char* formTime = TextFormat("Your time: %02d:%02d.%02d", minutes, seconds, centiseconds);
		const char* formBestTime = TextFormat("Best time: %02d:%02d.%02d", bestTime[0], bestTime[1], bestTime[2]);

		timerChrono.Draw(68, 430, formTime, GREEN, 20);
		timerChrono.Draw(70, 500, formBestTime, RED, 20);

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
	UnloadTexture(normalCar);
	UnloadTexture(wheel);

	for (int i = 0; i < 4; i++) {
		UnloadTexture(aiCarTextures[i]);
	}
	//Scenes
	UnloadTexture(menu);
	UnloadTexture(chrono_base);
	UnloadTexture(chrono_base2);
	UnloadTexture(controls);      
	UnloadTexture(victory);       
	UnloadTexture(defeat);

	
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
bool ModuleRender::DrawSemaphore(Texture2D texture, int state, int frameWidth, int frameHeight, float scale)
{
	Rectangle sourceRec = { (float)(frameWidth * state), 0.0f, (float)frameWidth,(float)frameHeight };

	float destWidth = frameWidth * scale;
	float destHeight = frameHeight * scale;

	float destX = (SCREEN_WIDTH - destWidth) / 2.0f;
	float destY = (SCREEN_HEIGHT - destHeight) / 2.0f;

	Rectangle destRec = { destX, destY, destWidth, destHeight };

	DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 200 });

	DrawTexturePro(texture, sourceRec, destRec, Vector2{ 0, 0 }, 0.0f, WHITE);
	return true;
}


bool ModuleRender::DrawText(const char* text, int x, int y, Font font, int spacing, Color tint) const
{
	bool ret = true;

	Vector2 position = { (float)x, (float)y };

	DrawTextEx(font, text, position, (float)font.baseSize, (float)spacing, tint);

	return ret;
}
