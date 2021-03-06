#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ComponentTransform.h"

#include "BombExplosionScript.h"
#include "PlayerMovement/PlayerMovement.h"


#include "JSON.h"
#include "imgui.h"

BombExplosionScript_API Script* CreateScript()
{
	BombExplosionScript* instance = new BombExplosionScript;
	return instance;
}

void BombExplosionScript::Awake()
{
	renderGO = App->scene->FindGameObjectByTag("mesh", gameobject);
	if (renderGO == nullptr)
	{
		LOG("render Go for the explosion not found");
	}
	else
	{
		myRenderer = renderGO->GetComponent<ComponentRenderer>();
		if (myRenderer == nullptr)
		{
			LOG("No componentRenderer found");
		}
	}

	particlesGO = App->scene->FindGameObjectByTag("particles", gameobject);
	if (particlesGO == nullptr)
	{
		LOG("Particles go for the explosion not found");
	}

	hitboxGO = App->scene->FindGameObjectByTag("hitbox", gameobject);
	if (hitboxGO == nullptr)
	{
		LOG("Hitbox GO for the explosion not found");
	}

	playerGO = App->scene->FindGameObjectByTag("Player", App->scene->root);
	{
		if (playerGO == nullptr)
		{
			LOG("player not found");
		}
		else
		{
			playerScript = playerGO->GetComponent<PlayerMovement>();
			if (playerScript == nullptr)
			{
				LOG("playerScript not found for the explosion");
			}
		}
	}

	myTransform = gameobject->GetComponent<ComponentTransform>();
}

void BombExplosionScript::Start()
{
	myRenderer->dissolveAmount = 1.0f;
	hitboxGO->SetActive(false);
	particlesGO->SetActive(false);
}

void BombExplosionScript::Update()
{
	switch (currentState)
	{
	case ExplosionState::None:
		currentState = ExplosionState::Appear;
		break;
	case ExplosionState::Appear:
		myRenderer->dissolveAmount -= App->time->gameDeltaTime * dissolveSpeed;	

		if (myRenderer->dissolveAmount <= 0.0f)
		{
			currentState = ExplosionState::Grow;
		}
		break;
	case ExplosionState::Grow:
		myTransform->scale += myTransform->scale * (scalingSpeed * App->time->gameDeltaTime);
		if (myTransform->scale.Length() == math::float3(finalScale, finalScale, finalScale).Length())
		{
			currentState = ExplosionState::Explode;
		}
		break;
	case ExplosionState::Explode:
		renderGO->SetActive(false);
		hitboxGO->SetActive(true);
		particlesGO->SetActive(true);
		currentState = ExplosionState::Finished;
		break;
	case ExplosionState::Finished:
		hitboxGO->SetActive(false);
		particlesGO->SetActive(false);
		break;
	}
}

void BombExplosionScript::OnTriggerEnter(GameObject * go)
{
	if (go == playerGO && !hasDamaged)
	{
		playerScript->Damage(damageToPlayer);
		hasDamaged = true;
	}
}

void BombExplosionScript::Expose(ImGuiContext * context)
{
	ImGui::DragFloat("Dissolve speed", &dissolveSpeed, 0.1f, 0.1f, 10.0f);
	ImGui::DragFloat("Damage", &damageToPlayer, 1.0f, 1.0f, 1000.0f);
	ImGui::DragFloat("Scale", &finalScale, 1.0f, 1.0f, 10.0f);
	ImGui::DragFloat("Scaling Speed", &scalingSpeed, 0.001f, 0.001f, 10.0f);
}

void BombExplosionScript::Serialize(JSON_value * json) const
{
	json->AddFloat("dissolveSpeed", dissolveSpeed);
	json->AddFloat("damageToPlayer", damageToPlayer);
	json->AddFloat("finalScale", finalScale);
	json->AddFloat("scalingSpeed", scalingSpeed);
}

void BombExplosionScript::DeSerialize(JSON_value * json)
{
	dissolveSpeed = json->GetFloat("dissolveSpeed");
	damageToPlayer = json->GetFloat("damageToPlayer");
	finalScale = json->GetFloat("finalScale");
	scalingSpeed = json->GetFloat("scalingSpeed");
}
