#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleMap.h"

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
		Application().physics->KillLateralVelocity(body->wheels[0]);
		Application().physics->KillLateralVelocity(body->wheels[1]);
		Application().physics->KillLateralVelocity(body->wheels[2]);
		Application().physics->KillLateralVelocity(body->wheels[3]);


		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, 90 + body->GetRotation() * RAD2DEG, WHITE);
	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

private:
	Texture2D texture;

};
void ModuleGame::move()
{
	// Check if playerCar exists
	if (playerCar == nullptr) return;

	float acceleration = 2.0f; 
	float steerSpeed = 1.5f; //w (rad/s)
	b2Vec2 forward = playerCar->body->wheels[0]->GetWorldVector(b2Vec2(0, -1));

	//Accelerate
	if (IsKeyDown(KEY_W)) 
	{
		playerCar->body->body->ApplyForceToCenter(acceleration * forward, true);
	}
	//Slow Down
	else if (IsKeyDown(KEY_S)) 
	{
		playerCar->body->body->ApplyForceToCenter(-acceleration * forward, true);
	}

	float steer = 0.0f;
	if (IsKeyDown(KEY_A)) 
	{
		steer = -steerSpeed;
	}
	if (IsKeyDown(KEY_D)) 
	{
		steer = steerSpeed;
	}
	playerCar->body->motorJoints[0]->SetMotorSpeed(steer);
	playerCar->body->motorJoints[1]->SetMotorSpeed(steer);
}

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

	App->renderer->camera.x = App->renderer->camera.y = 0;

	circle = LoadTexture("Assets/wheel.png");
	box = LoadTexture("Assets/crate.png");



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
		Car* newCar = new Car(App->physics, GetMouseX(), GetMouseY(), this, box);

		entities.emplace_back(newCar);

		playerCar = newCar;

	}


	// Center the camera on the player
	if (playerCar)
	{
		float camSpeed = 5;

		int x, y;
		playerCar->body->GetPhysicPosition(x, y);
		
		int mapWidthInPixels = App->map->map_data.width * App->map->map_data.tilewidth;
		float limitLeft = App->renderer->camera.width / 4;
		float limitRight = mapWidthInPixels - App->renderer->camera.width * 3 / 4;

		if (x - limitLeft > 0 && x < limitRight)
		{
			App->renderer->camera.x = -x + App->renderer->camera.width * 3 / 4;
		}
		else if (x - limitLeft <= 0)
		{
			App->renderer->camera.x = 0;
		}
		else if (mapWidthInPixels - x < App->renderer->camera.width)
		{
			App->renderer->camera.x = -mapWidthInPixels + App->renderer->camera.width;
		}

		int mapHeightInPixels = App->map->map_data.height * App->map->map_data.tileheight;
		float limitUp = App->renderer->camera.height / 4;
		float limitDown = mapHeightInPixels - App->renderer->camera.height * 3 / 4;

		if (y - limitUp > 0 && y < limitDown)
		{
			App->renderer->camera.y = -y + App->renderer->camera.height * 3 / 4;
		}
		else if (y - limitUp <= 0)
		{
			App->renderer->camera.y = 0;
		}
		else if (mapHeightInPixels - y < App->renderer->camera.height)
		{
			App->renderer->camera.y = -mapHeightInPixels + App->renderer->camera.height;
		}

	}

	//if (IsKeyDown(KEY_UP) && App->renderer->camera.y < 0)
	//	App->renderer->camera.y += (int)ceil(camSpeed);

	//int mapHeightInPixels = App->map->map_data.height * App->map->map_data.tileheight;
	//if (IsKeyDown(KEY_DOWN) && App->renderer->camera.y > -(mapHeightInPixels - SCREEN_HEIGHT))
	//	App->renderer->camera.y -= (int)ceil(camSpeed);

	//if (IsKeyDown(KEY_LEFT) && App->renderer->camera.x < 0)
	//	App->renderer->camera.x += (int)ceil(camSpeed);

	//int mapWidthInPixels = App->map->map_data.width * App->map->map_data.tilewidth;
	//if (IsKeyDown(KEY_RIGHT) && App->renderer->camera.x > -(mapWidthInPixels - SCREEN_WIDTH))
	//	App->renderer->camera.x -= (int)ceil(camSpeed);

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

	move();
	return UPDATE_CONTINUE;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (!bodyA || !bodyB)
	{
		return;
	}


}
