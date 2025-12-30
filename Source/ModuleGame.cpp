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


		break;
	case GameScreen::CONTROLS:


		break;
	case GameScreen::CREDITS:


		break;
	case GameScreen::CAR_SELECT:



		break;
	case GameScreen::MAP_SELECT:



		break;
	case GameScreen::GAME:

		//Aqui se carga el mapa del Tiled
		if (!mapLoad)
		{
			App->map->Load(mapPath.c_str());
			mapLoad = true;

			semaphoreTileset = App->map->GetTileset("semaforo");

			if (semaphoreTileset == nullptr) {
				LOG("Error: No se encontró el tileset 'semaforo' en el mapa Tiled.");
			}

			std::reverse(IA_Route.begin(), IA_Route.end());

			if (playerCar == nullptr) {
				Car* newCar = new Car(App->physics, posInit.x, posInit.y, this, App->renderer->normalCar, App->renderer->wheel);
				entities.emplace_back(newCar);
				playerCar = newCar;

				bodyCar = playerCar->body->body;
				positionCar = bodyCar->GetPosition();

				float angleRadians = 100.0f * (PI / 180.0f);

				bodyCar->SetTransform(positionCar, angleRadians);
			}

			//We define the central route of the circuit
			std::vector<b2Vec2> centralRoute = { //Falta ordenarlo y crear mas puntos
				{1888, 640},  //Punto 0: Salida
				{5500, 640},  //Punto 1: Fin recta superior
				{6200, 1200}, //Punto 2: Curva arriba-derecha
				{6200, 4000}, //Punto 3: Recta larga derecha (bajada)
				{3000, 4100}, //Punto 4: Recta inferior
				{800, 4100},  //Punto 5: Curva abajo-izquierda
				{800, 2400},  //Punto 6: Recta izquierda (subida)
				{4000, 2400}, //Punto 7: Recta central
				{4000, 1000}, //Punto 8: Giro hacia meta
				{1888, 640}   //Punto 9: Meta
			};

			for (int i = 1; i < 4; i++)
			{
				float inX = posInit.x - (150.0f * i);
				float inY = posInit.y - (75.0f * (i % 2)); // Alternate up/down

				IACar* newIA = new IACar(App->physics, (int)inX, (int)inY, this, App->renderer->normalCar, App->renderer->wheel);

				// Asignamos carril: el coche 1 va por dentro (-50), el 2 por el centro (0), el 3 por fuera (50)
				newIA->laneOffset = (float)(i - 2) * 60.0f;

				// Creamos su ruta personalizada sumando el offset
				for (auto& p : centralRoute) {
					newIA->waypoints.push_back({ p.x + newIA->laneOffset, p.y + newIA->laneOffset });
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
		break;
	case GameScreen::GAMEOVER:
		delete playerCar;
		playerCar = nullptr;

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
	PhysBody* checkpoint = nullptr;

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
					App->menu->currentScreen = GameScreen::GAMEOVER;
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