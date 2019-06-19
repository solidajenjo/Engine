#include "CameraController.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "imgui.h"
#include "JSON.h"
#include "Math/MathFunc.h"

#define ANGLE_MULTIPLIER 0.05f

CameraController_API Script* CreateScript()
{
	CameraController* instance = new CameraController;
	return instance;
}

void CameraController::Start()
{
	player = App->scene->FindGameObjectByName("Player");
	assert(player != nullptr);
	offset = gameobject->transform->GetPosition() - player->transform->GetPosition();
	//Shake(10.0f, 30.0f, 0.2f, 0.8f);
}

void CameraController::Update()
{
	math::float3 newPosition = offset + player->transform->GetPosition();
	if (isShaking)
	{
		ShakeCamera(newPosition);
	}
	gameobject->transform->SetPosition(newPosition);
}

void CameraController::Shake(float duration, float intensity, float fadeInTime, float fadeOutTime)
{
	shakeDuration = duration;
	shakeIntensity = intensity;

	shakeFadeInTime = fadeInTime;
	shakeFadeOutTime = fadeOutTime;
	roll = 0.0f;

	if (!isShaking)
	{
		isShaking = true;
		originalRotation = gameobject->transform->GetRotation();
		shakeTimer = .0f;
	}
}

void CameraController::ShakeCamera(math::float3& position) //smooth
{
	shakeTimer += App->time->gameDeltaTime;
	float range = shakeIntensity;

	if (shakeTimer >= shakeDuration)
	{
		isShaking = false;
		gameobject->transform->SetRotation(originalRotation);
	}
	else
	{
		if (shakeTimer <= shakeFadeInTime * shakeDuration)
		{
			float decay = shakeTimer / (shakeFadeInTime * shakeDuration); //increases over time
			range *= decay;
		}
		else if (shakeTimer >= shakeFadeOutTime * shakeDuration)
		{
			float fadeOutTime = (1 - shakeFadeOutTime) * shakeDuration;
			float decay = 1 - (shakeTimer - shakeFadeOutTime * shakeDuration) / fadeOutTime; //decreases over time
			range *= decay;
		}

		math::float3 lastPosition = position;
		position = math::float3::RandomSphere(rand, position, range);
		position = (position + lastPosition) * 0.5f;

		float lastRoll = roll;
		roll = ANGLE_MULTIPLIER * range * (rand.Float() * 2 - 1);
		roll = (lastRoll + roll) * 0.5f;
		gameobject->transform->SetRotation(originalRotation.Mul(Quat::RotateZ(math::DegToRad(roll))));
	}
}