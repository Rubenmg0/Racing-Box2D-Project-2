#include "IA.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleGame.h"


void IACar::Update()
{
	if (app->scene_intro->raceStarted == false)
	{
		body->body->SetLinearVelocity(b2Vec2(0, 0));
		body->body->SetAngularVelocity(0);
		return;
	}

	//Positions
	int x, y; body->GetPhysicPosition(x, y); x += 100; y += 100; // Temporal Correction
	b2Vec2 targetPos = app->scene_intro->IA_Route[currentPoint + 1];

	//Direction
	b2Vec2 dir = b2Vec2(targetPos.x - x, targetPos.y - y);

	//Movement
	int horitzontal = 0;
	bool forward = true;

	//Angle to look forward
	float desiredAngle = atan2f(dir.y, dir.x) - b2_pi / 2.0f;

	float currentRotation = body->GetRotation();

	float angleDif = desiredAngle - currentRotation;

	//Normalization
	while (angleDif > b2_pi)
	{
		angleDif -= 2.0f * b2_pi;
	}
	while (angleDif < -b2_pi)
	{
		angleDif += 2.0f * b2_pi;
	}

	//Ray
	b2Vec2 forwardVec = b2Vec2(
		cosf(currentRotation + b2_pi / 2.0f),
		sinf(currentRotation + b2_pi / 2.0f)
	);

	float rayLength = 2.5f; // meters
	b2Vec2 rayEnd = b2Vec2(x + forwardVec.x * rayLength, y + forwardVec.y * rayLength);

	vec2<float> normal;
	bool hitWall = (RayHit(vec2<int>(METERS_TO_PIXELS(x), METERS_TO_PIXELS(y)), vec2<int>(METERS_TO_PIXELS(rayEnd.x), METERS_TO_PIXELS(rayEnd.y)), normal) > 0);

	// WALL AVOIDANCE
	if (hitWall)
	{
		forward = 0; // Decelerate

		// Turn away from wall using normal (cross product 2D)
		b2Vec2 forwardVec = b2Vec2(cosf(currentRotation + b2_pi / 2.0f), sinf(currentRotation + b2_pi / 2.0f));
		float side = forwardVec.x * normal.y - forwardVec.y * normal.x;
		horitzontal = (side > 0) ? -1 : 1;
	}
	else
	{
		float TOL = 1.0f;

		if (angleDif > TOL)
		{
			horitzontal = 1;
		}
		else if (angleDif < -TOL)
		{
			horitzontal = -1;
		}
	}

	app->physics->MoveAI(body, horitzontal, forward);
	
	for (int i = 0; i < 4; i++)
	{
		physics->KillLateralVelocity(body->wheels[i]);
	}


	float distance = dir.Length();

	if (distance < 4.0f * 256.0f)
	{
		currentPoint++;
	}
}

void IACar::Draw()
{
	int x, y;
	body->GetPhysicPosition(x, y);
	float car_scaleX = 1.3f;
	float car_scaleY = 1.6f;

	DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
		Rectangle{ (float)x, (float)y, (float)texture.width * car_scaleX, (float)texture.height * car_scaleY },
		Vector2{ (float)texture.width * car_scaleX / 2.0f, (float)texture.height * car_scaleY / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);

	if (wheel.height != 0)
	{
		for (int i = 0; i < 4; i++)
		{
			b2Vec2 pos = body->wheels[i]->GetPosition();
			int wx = METERS_TO_PIXELS(pos.x);
			int wy = METERS_TO_PIXELS(pos.y);
			float wheel_scale = 0.6f;

			DrawTexturePro(wheel, Rectangle{ 0, 0, (float)wheel.width , (float)wheel.height },
				Rectangle{ (float)wx , (float)wy, (float)wheel.width * wheel_scale, (float)wheel.height * wheel_scale },
				Vector2{ (float)wheel.width * wheel_scale / 2.0f, (float)wheel.height * wheel_scale / 2.0f }, 90 + body->wheels[i]->GetAngle() * RAD2DEG, WHITE);
		}
	}
}

int IACar::RayHit(vec2<int> ray, vec2<int> pos, vec2<float>& normal)
{
	return body->RayCastWorld(ray.x, ray.y, pos.x, pos.y, normal.x, normal.y, app);
}