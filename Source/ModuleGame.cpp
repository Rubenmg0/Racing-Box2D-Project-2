#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleMap.h"
#include "ModuleMenu.h"

#include "Car.hpp"

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

	//sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);

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
		if (!mapLoad) {
		App->map->Load("Assets/map/CuteRacing.tmx");
		mapLoad = true;
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

		if (playerCar)
		{
			float friction = 0.1f;
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
				friction = 0.1f;
			}
			else if (touchingGrass)
			{
				friction = 7.0f;
			}

			playerCar->body->body->SetLinearDamping(friction);
			App->physics->MoveCar(playerCar->body);

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

		break;
	case GameScreen::GAMEOVER:
		delete playerCar;
		playerCar = nullptr;

		break;
	default:
		break;
	}


	if (IsKeyPressed(KEY_SPACE))
	{
		ray_on = !ray_on;
		ray.x = GetMouseX();
		ray.y = GetMouseY();
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
	if (!bodyA || !bodyB)
	{
		return;
	}
}

void ModuleGame::OnUIMouseClickEvent(UIElement* element)
{
	// Implement the action you want when you click the button
}