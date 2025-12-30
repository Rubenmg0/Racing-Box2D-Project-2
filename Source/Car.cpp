#include "Car.hpp"


void Car::Update() 
{


}

void Car::Draw() 
{
	int x, y;
	body->GetPhysicPosition(x, y);
	float car_scaleX = 1.3f;
	float car_scaleY = 1.6f;

	if (wheel.height != 0)
	{
		for (int i = 0; i < 4; i++)
		{
			b2Vec2 pos = body->wheels[i]->GetPosition();
			int wx = METERS_TO_PIXELS(pos.x) ;
			int wy = METERS_TO_PIXELS(pos.y);
			float wheel_scale = 0.6f;

			DrawTexturePro(wheel, Rectangle{ 0, 0, (float)wheel.width , (float)wheel.height },
				Rectangle{ (float)wx , (float)wy, (float)wheel.width * wheel_scale, (float)wheel.height * wheel_scale },
				Vector2{ (float)wheel.width * wheel_scale / 2.0f, (float)wheel.height * wheel_scale / 2.0f }, 90+ body->wheels[i]->GetAngle() * RAD2DEG, WHITE);
		}
	}
	DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
		Rectangle{ (float)x, (float)y, (float)texture.width * car_scaleX, (float)texture.height * car_scaleY },
		Vector2{ (float)texture.width * car_scaleX / 2.0f, (float)texture.height * car_scaleY / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);

}

int Car::RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) 
{
	return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
}

void Car::DeletePhysBody()
{
	this->body->DeleteAllBodies(app);
}
