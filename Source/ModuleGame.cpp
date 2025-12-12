#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleMap.h"

#include "UIElement.h"

class PhysicEntity
{
protected:
	PhysicEntity(PhysBody* _body, Module* _listener)
		: body(_body)
		, listener(_listener)
	{
		body->listener = listener;
	}

public:
	virtual ~PhysicEntity() = default;
	virtual void Update() = 0;

	virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
	{
		return 0;
	}

public:
	PhysBody* body;
	Module* listener;
};

class Car : public PhysicEntity
{
public:
	Car(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateCar(_x, _y), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		physics->KillLateralVelocity(body->wheels[0]);
		physics->KillLateralVelocity(body->wheels[1]);
		physics->KillLateralVelocity(body->wheels[2]);
		physics->KillLateralVelocity(body->wheels[3]);


		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

private:
	Texture2D texture;
	ModulePhysics* physics;
};

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

	circle = LoadTexture("Assets/wheel.png");
	box = LoadTexture("Assets/crate.png");
	burgerCar = LoadTexture("Assets/Cars/BurgerCar/burger_car.png");


	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);

	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");


	return true;
}

// Update: draw background
update_status ModuleGame::Update()
{
	if (IsKeyPressed(KEY_SPACE))
	{
		ray_on = !ray_on;
		ray.x = GetMouseX();
		ray.y = GetMouseY();
	}


	if (IsKeyPressed(KEY_TWO))
	{
		Car* newCar = new Car(App->physics, GetMouseX(), GetMouseY(), this, burgerCar);

		entities.emplace_back(newCar);

		playerCar = newCar;

	}


	// Center the camera on the player
	if (playerCar)
	{
			float friction = 0.3f;

			int carX, carY;
			playerCar->body->GetPhysicPosition(carX, carY);

			if (App->map->map_data.tilewidth > 0 && App->map->map_data.tileheight > 0)
			{
				int tileX = carX / App->map->map_data.tilewidth;
				int tileY = carY / App->map->map_data.tileheight;

				for (const auto& layer : App->map->map_data.layers)
				{
					if (layer->name == "Barreras")
					{
						
							int index = (tileY * layer->width) + tileX;

							if (layer->tiles[index] > 0)
							{
								friction = 5.0f;
							}
							
						
						break; 
					}
				}
			}

		

		playerCar->body->body->SetLinearDamping(friction);
		App->physics->MoveCar(playerCar->body);

		//debug para detectar que va lo de la friccion
		Color textColor = (friction > 1.0f) ? RED : GREEN;
		DrawText(TextFormat("Friccion Actual: %.1f", friction), 20, 20, 30, textColor);

		playerCar->body->GetPhysicPosition(carX, carY);
		int mapWidth = App->map->map_data.width * App->map->map_data.tilewidth - 150;
		int mapHeight = App->map->map_data.height * App->map->map_data.tileheight;

		float targetX = -(carX - SCREEN_WIDTH / 2.0f);
		float targetY = -(carY - SCREEN_HEIGHT / 2.0f);

		// -- EJE X --
		if (targetX > 0) targetX = 0;
		else if (targetX < SCREEN_WIDTH - mapWidth) targetX = SCREEN_WIDTH - mapWidth;

		// -- EJE Y --
		if (targetY > 0) targetY = 0;
		else if (targetY < SCREEN_HEIGHT - mapHeight) targetY = SCREEN_HEIGHT - mapHeight;

		/*App->renderer->camera.x = targetX * 1.5;
		App->renderer->camera.y = targetY * 2.5;*/
		App->renderer->camera.target.x = targetX * 1.5;
		App->renderer->camera.target.y = targetY * 2.5;
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