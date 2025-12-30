#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleMap.h"
#include "ModuleMenu.h"

#include "Car.hpp"
#include "IA.h"

#include <random>

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ray_on = false;
	sensed = false;
	
}

ModuleGame::~ModuleGame()
{
}

// Load assets
bool ModuleGame::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	crash = LoadSound("Assets/Audio/Car/car-crash.wav");
	backgroundMusic = LoadMusicStream("Assets/Audio/Music/music.wav");
	finishMusic = LoadMusicStream("Assets/Audio/Music/music1.mp3");
	waitMusic = LoadMusicStream("Assets/Audio/Music/music2.wav");

	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");
	for (auto& entity : entities)
	{
		delete entity;
	}
	entities.clear();
	playerCar = nullptr;
	UnloadSound(crash);
	UnloadMusicStream(backgroundMusic);
	UnloadMusicStream(finishMusic);
	UnloadMusicStream(waitMusic);
	
	
	return true;
}

// Update: draw background
update_status ModuleGame::Update()
{
		
	if (checkpointFeedbackTimer > 0.0f)
	{
		checkpointFeedbackTimer -= GetFrameTime();
	}

	switch (App->menu->currentScreen)
	{
	case GameScreen::MENU:
		if (!IsMusicStreamPlaying(waitMusic))
		{
			PlayMusicStream(waitMusic);
		}
		UpdateMusicStream(waitMusic);
		break;
	case GameScreen::CONTROLS:
		if (!IsMusicStreamPlaying(waitMusic))
		{
			PlayMusicStream(waitMusic);
		}
		UpdateMusicStream(waitMusic);
		break;
	case GameScreen::CREDITS:
		if (!IsMusicStreamPlaying(waitMusic))
		{
			PlayMusicStream(waitMusic);
		}
		UpdateMusicStream(waitMusic);
		break;
	case GameScreen::CAR_SELECT:
		if (!IsMusicStreamPlaying(waitMusic))
		{
			PlayMusicStream(waitMusic);
		}
		UpdateMusicStream(waitMusic);
		break;
	case GameScreen::MAP_SELECT:

		if (!IsMusicStreamPlaying(waitMusic))
		{
			PlayMusicStream(waitMusic);
		}
		UpdateMusicStream(waitMusic);
		break;
	case GameScreen::GAME:
	
		if (!IsMusicStreamPlaying(backgroundMusic))
		{
			PlayMusicStream(backgroundMusic);
		}
		UpdateMusicStream(backgroundMusic);
		//Aqui se carga el mapa del Tiled
		if (!mapLoad)
		{
			App->map->Load(mapPath.c_str());
			mapLoad = true;

			semaphoreTileset = App->map->GetTileset("semaforo");

			if (semaphoreTileset == nullptr) {
				LOG("Error: No se encontró el tileset 'semaforo' en el mapa Tiled.");
			}


			if (playerCar == nullptr) {
				Car* newCar = new Car(App->physics, posInit.x, posInit.y, this, App->renderer->normalCar, App->renderer->wheel);
				entities.emplace_back(newCar);
				playerCar = newCar;

				bodyCar = playerCar->body->body;
				positionCar = bodyCar->GetPosition();

				float angleRadians = 100.0f * (PI / 180.0f);

				bodyCar->SetTransform(positionCar, angleRadians);
			}

			//We define the central route of the circuit of the first circuit
			std::vector<b2Vec2> centralRoute1 = {
				{1888, 640},	//Point 0: Start
				{6000, 640},	//Point 1: End of top  straight stretch
				{6200, 1400},	//Point 2: Right Curve 1
				{7000, 1400},	//Point 3: Right Curve 2
				{7000, 2050},	//Point 4: Right Curve 3
				{6000, 2200},	//Point 5: Right Curve 4
				{6000, 3000},	//Point 6: Right Curve 5
				{7000, 3000},	//Point 7: Right Curve 6
				{7000, 4500},	//Point 8: End of Right Curve
				{5500, 4500},	//Point 9: Bottom straight stretch 1
				{5500, 4000},	//Point 9: Bottom Curve
				{650, 4000},	//Point 9: Bottom straight stretch 1
				{650, 2950},	//Point 10: Left curve 1
				{5000, 2950},	//Point 11: Left straight stretch 1
				{5000, 1670},	//Point 12: Central curve 1
				{3460, 1670},	//Point 13: Central straight stretch 1
				{3460, 2180},	//Point 14: Central curve 2
				{2400, 2180},	//Point 15: Central  straight stretch 2
				{2400, 1400},	//Point 16: Central  curve 3
				{1400, 1400},	//Point 17: Central  curve 4
				{1400, 2200},	//Point 18: Central  curve 5
				{400, 2200},	//Point 19: Left Curve 2
				{400, 640},		//Point 20: Left straight stretch 2
			};

			std::vector<b2Vec2> centralRoute2 = { //TODO NAME POINTS IF NEEDED
				{2700, 640},	//Point 0: Start
				{3100, 640},	//Point 1:
				{3100, 900},	//Point 2:
				{3700, 900},	//Point 3:
				{3700, 1100},	//Point 4:
				{4700, 1100},	//Point 5:
				{4700, 3000},	//Point 6: Right straight stretch
				{3700, 3000},
				{3700, 1900},
				{2700, 1900},
				{2700, 3200},
				{400, 3200},
				{400, 2400},
				{1600, 2400},
				{1600, 1400},
				{1000, 1400},
				{600, 1000},
				{600, 650},		//End
			};

			for (int i = 1; i < 4; i++)
			{
				float inX = posInit.x - (150.0f * i);
				float inY = posInit.y - (75.0f * (i % 2)); // Alternate up/down

				//Randomness (From an online forum)
				std::random_device rd;
				std::mt19937 gen(rd()); // Mersenne Twister engine
				std::uniform_int_distribution<> dist(0, 3);
				int randomNumber = dist(gen);

				IACar* newIA = new IACar(App->physics, (int)inX, (int)inY, this, App->renderer->aiCarTextures[randomNumber], App->renderer->wheel);

				// Asignamos carril: el coche 1 va por dentro (-50), el 2 por el centro (0), el 3 por fuera (50)
				newIA->laneOffset = (float)(i - 2) * 60.0f;

				// Creamos su ruta personalizada sumando el offset
				std::vector<b2Vec2> centralRoute;
				if (levelLoaded == 1) centralRoute = centralRoute1;
				else if (levelLoaded == 2) centralRoute = centralRoute2;
				for (auto& p : centralRoute) {
					newIA->waypoints.push_back({ (p.x) + newIA->laneOffset, (p.y) + newIA->laneOffset });
				}

				entities.emplace_back(newIA);

				float angleRadians = 100.0f * (PI / 180.0f);
				newIA->body->body->SetTransform(newIA->body->body->GetPosition(), angleRadians);
			}
		}

		if (playerCar)
		{
			float friction = 0.5f;
			float powerMultiplier = 1.0f;
			int carX, carY;

			playerCar->body->GetPhysicPosition(carX, carY);

			bool touchingCircuit = false;
			bool touchingGrass = false;

			if (App->map->map_data.tilewidth > 0 && App->map->map_data.tileheight > 0)
			{
				int tileX = carX / App->map->map_data.tilewidth;
				int tileY = carY / App->map->map_data.tileheight;

				for (const auto& layer : App->map->map_data.layers)
				{
					int index = (tileY * layer->width) + tileX;

					if (index >= 0 && index < layer->width * layer->height)
					{
						if (layer->tiles[index] > 0)
						{
							if (layer->name == "Circuito")
							{
								touchingCircuit = true;
							}
							else if (layer->name == "Hierba")
							{
								touchingGrass = true;
							}
						}
					}

				}
			}
			if (touchingCircuit)
			{
				friction = 6.0f;
				powerMultiplier = 1.0f;
			}
			else if (touchingGrass)
			{
				friction = 7.0f;
				powerMultiplier = 0.3f;
			}

			playerCar->body->body->SetLinearDamping(friction);
			if (raceStarted)
			{
				App->physics->MoveCar(playerCar->body, powerMultiplier);
			}
			else
			{
				playerCar->body->body->SetLinearVelocity(b2Vec2(0, 0));
				playerCar->body->body->SetAngularVelocity(0);
			}

			// Center the camera on the player
			float currentZoom = App->renderer->camera.zoom = 1.5f;

			float halfScreenWidth = SCREEN_WIDTH / 2.0f;
			float halfScreenHeight = SCREEN_HEIGHT / 2.0f;

			float visibleHalfWidth = halfScreenWidth / currentZoom;
			float visibleHalfHeight = halfScreenHeight / currentZoom;

			App->renderer->camera.offset = Vector2{ halfScreenWidth, halfScreenHeight };

			int mapWidth = App->map->map_data.width * App->map->map_data.tilewidth;
			int mapHeight = App->map->map_data.height * App->map->map_data.tileheight;

			float targetX = (float)carX;
			float targetY = (float)carY;

			// -- X AXIS --
			if (targetX < visibleHalfWidth)
			{
				targetX = visibleHalfWidth;
			}
			else if (targetX > mapWidth - visibleHalfWidth)
			{
				targetX = mapWidth - visibleHalfWidth;
			}

			// -- Y AXIS --
			if (targetY < visibleHalfHeight)
			{
				targetY = visibleHalfHeight;
			}
			else if (targetY > mapHeight - visibleHalfHeight)
			{
				targetY = mapHeight - visibleHalfHeight;
			}

			App->renderer->camera.target = Vector2{ targetX, targetY };
		}

		if (IsKeyPressed(KEY_J)) completedLaps = 2, nextCheckpointRequired = totalCheckpoints;
		break;
	case GameScreen::GAMEOVER:

		for (const auto entity : entities) {

		}
		break;
	default:
		break;
	}

	// Prepare for raycast ------------------------------------------------------

	vec2i mouse;
	mouse.x = GetMouseX();
	mouse.y = GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	vec2f normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------

	for (PhysicEntity* entity : entities)
	{
		entity->Update();
		entity->Draw();
		if (ray_on)
		{
			int hit = entity->RayHit(ray, mouse, normal);
			if (hit >= 0)
			{
				ray_hit = hit;
			}
		}
	}

	if (App->menu->currentScreen == GameScreen::GAME)
	{
		EndMode2D();
		{
			int totalFrames = 10;
			float timePerFrame = 0.2f;
			if (semaphoreTileset != nullptr && semaphoreState < totalFrames)
			{
				semaphoreTimer += GetFrameTime();
				if (semaphoreTimer >= timePerFrame)
				{
					semaphoreState++;
					semaphoreTimer = 0.0f;
					if (semaphoreState == 9)
					{
						raceStarted = true;
					}
				}
				if (semaphoreState < totalFrames)
				{
					// Llamamos a la nueva función pasando la textura y las medidas fijas
					App->renderer->DrawSemaphore(semaphoreTileset->texture, semaphoreState, 96, 176, 3.0f);
				}
			}
			float miniMapSize = 200.0f;
			float margen = 20.0f;

			Rectangle miniMapArea = {
				(float)SCREEN_WIDTH - miniMapSize - margen,
				(float)SCREEN_HEIGHT - miniMapSize - margen,
				miniMapSize,
				miniMapSize
			};

			if (playerCar != nullptr && playerCar->body != nullptr) {
				int x, y;
				playerCar->body->GetPhysicPosition(x, y);
				App->map->DrawMinimap(miniMapArea, Vector2{ (float)x, (float)y });
			}
		}
		BeginMode2D(App->renderer->camera);
	}
	// ray -----------------
	if (ray_on == true)
	{
		vec2f destination((float)(mouse.x - ray.x), (float)(mouse.y - ray.y));
		destination.Normalize();
		destination *= (float)ray_hit;

		DrawLine(ray.x, ray.y, (int)(ray.x + destination.x), (int)(ray.y + destination.y), RED);

		if (normal.x != 0.0f)
		{
			DrawLine((int)(ray.x + destination.x), (int)(ray.y + destination.y), (int)(ray.x + destination.x + normal.x * 25.0f), (int)(ray.y + destination.y + normal.y * 25.0f), Color{ 100, 255, 100, 255 });
		}
	}

	return UPDATE_CONTINUE;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	//Security check. If any are null, we ignore the collision.
	if (bodyA == nullptr || bodyB == nullptr) {
		return;
	}

	PhysBody* car = nullptr;
	PhysBody* IAcar = nullptr;
	PhysBody* checkpoint = nullptr;

	if (bodyA->type == BodyType::CAR && bodyB->type != BodyType::CHECKPOINT) {
		car = bodyA;
		if (IsSoundPlaying(crash)) {}
		else {
			PlaySound(crash);
		}
	}
	else if (bodyB->type == BodyType::CAR && bodyA->type != BodyType::CHECKPOINT) {
		car = bodyB; 
		if (IsSoundPlaying(crash)) {}
		else {
			PlaySound(crash);
		}
	}

	if (bodyA->type == BodyType::CAR) car = bodyA;
	else if (bodyB->type == BodyType::CAR) car = bodyB;
	if (bodyA->type == BodyType::CHECKPOINT) checkpoint = bodyA;
	else if (bodyB->type == BodyType::CHECKPOINT) checkpoint = bodyB;

	if (car != nullptr && checkpoint != nullptr)
	{
		//If you cross the finish line
		if (checkpoint->checkpointID == 0)
		{
			//Start of the race
			if (nextCheckpointRequired == 0)
			{
				nextCheckpointRequired = 1;
			}
			//If you have already passed through all the checkpoints
			else if (nextCheckpointRequired == totalCheckpoints)
			{
				completedLaps++;

				//If you have completed all the laps
				if (completedLaps >= totalLaps) {
					App->menu->ChangeScreen(GameScreen::GAMEOVER);
				}

				//If not, reset checkpoints
				nextCheckpointRequired = 1;
			}
		}
		//If you cross a checkpoint
		else if (checkpoint->checkpointID == nextCheckpointRequired)
		{
			nextCheckpointRequired++;
			checkpointFeedbackTimer = 2.0f;
		}
	}
	
	
	
	
}

void ModuleGame::OnUIMouseClickEvent(UIElement* element)
{
	// Implement the action you want when you click the button
}