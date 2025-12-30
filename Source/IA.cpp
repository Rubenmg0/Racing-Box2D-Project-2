#include "IA.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleGame.h"


void IACar::Update()
{
	if (app->scene_intro->raceStarted == false || waypoints.empty())
	{
		body->body->SetLinearVelocity(b2Vec2(0, 0));
		body->body->SetAngularVelocity(0);
		return;
	}

    int x, y;
    body->GetPhysicPosition(x, y);
    b2Vec2 pos((float)x, (float)y);
    b2Vec2 velocity = body->body->GetLinearVelocity();

    b2Vec2 target = waypoints[currentWaypoint];
    b2Vec2 dir = target - pos;
    float distance = dir.Length();

    if (velocity.Length() < 0.5f) {
        stuckTimer += GetFrameTime();
    }
    else {
        stuckTimer = 0.0f;
        isStuck = false;
    }

    if (stuckTimer > 1.5f) isStuck = true;

    //If we are close to the point, we move on to the next one.
    if (distance < 250.0f) {
        currentWaypoint = (currentWaypoint + 1) % waypoints.size();
        return;
    }

    //Calculation of rotation
    float desiredAngle = atan2f(dir.y, dir.x) + b2_pi / 2.0f;
    float currentRotation = body->GetRotation();
    float angleDif = desiredAngle - currentRotation;

    while (angleDif > b2_pi) angleDif -= 2.0f * b2_pi;
    while (angleDif < -b2_pi) angleDif += 2.0f * b2_pi;

    float steerMagnitude = angleDif * 1.5f;
    if (steerMagnitude > 1.0f) steerMagnitude = 1.0f;
    if (steerMagnitude < -1.0f) steerMagnitude = -1.0f;

    int forward = (abs(angleDif) < 0.7f) ? 1 : 0;
    if (isStuck) forward = 1;
    else if (abs(angleDif) < 0.8f) {
        forward = 1;
    }
    else {
        body->body->SetAngularVelocity(body->body->GetAngularVelocity() * 0.9f);
    }

    app->physics->MoveAI(body, (int)steerMagnitude, forward);

    if (!body->isColliding) {
        for (int i = 0; i < 4; i++) {
            physics->KillLateralVelocity(body->wheels[i]);
        }
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

    //Draw the points and the route line in debug
    if (app->physics->IsDebug()) {
        for (size_t i = 0; i < waypoints.size(); i++) {
            //If it is the current target, we colour it red. If not, we colour it blue.
            Color c = (i == currentWaypoint) ? RED : BLUE;
            DrawCircle(waypoints[i].x, waypoints[i].y, 20.0f, c);

            //We draw the line to the next point
            b2Vec2 next = waypoints[(i + 1) % waypoints.size()];
            DrawLine(waypoints[i].x, waypoints[i].y, next.x, next.y, Fade(BLUE, 0.3f));
        }
    }
}

int IACar::RayHit(vec2<int> ray, vec2<int> pos, vec2<float>& normal)
{
	return body->RayCastWorld(ray.x, ray.y, pos.x, pos.y, normal.x, normal.y, app);
}