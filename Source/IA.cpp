#include "IA.h"


void IACar::Update()
{
	//TODO Move IA 

	physics->KillLateralVelocity(body->wheels[0]);
	physics->KillLateralVelocity(body->wheels[1]);
	physics->KillLateralVelocity(body->wheels[2]);
	physics->KillLateralVelocity(body->wheels[3]);
}

void IACar::Draw()
{
	int x, y;
	body->GetPhysicPosition(x, y);

	DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
		Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
		Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);

	if (wheel.height != 0)
	{
		for (int i = 0; i < 4; i++)
		{
			b2Vec2 pos = body->wheels[i]->GetPosition();
			int wx = METERS_TO_PIXELS(pos.x);
			int wy = METERS_TO_PIXELS(pos.y);

			DrawTexturePro(wheel, Rectangle{ 0, 0, (float)wheel.width , (float)wheel.height },
				Rectangle{ (float)wx, (float)wy, (float)wheel.width, (float)wheel.height },
				Vector2{ (float)wheel.width / 2.0f, (float)wheel.height / 2.0f }, 90 + body->wheels[i]->GetAngle() * RAD2DEG, WHITE);
		}
	}
}

int IACar::RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
{
	return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
}