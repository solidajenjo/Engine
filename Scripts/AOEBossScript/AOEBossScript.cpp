#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentBoxTrigger.h"
#include "ComponentAudioSource.h"
#include "ComponentTransform.h"

#include "AOEBossScript.h"
#include "PlayerMovement/PlayerMovement.h"
#include "BossBehaviourScript/BossBehaviourScript.h"

#include "Math/float3.h"
#include "Math/Quat.h"
#include "imgui.h"
#include "JSON.h"

#define AOEPREFAB "AOEFirstPhase"

AOEBossScript_API Script* CreateScript()
{
	AOEBossScript* instance = new AOEBossScript;
	return instance;
}

void AOEBossScript::Awake()
{
	playerGO = App->scene->FindGameObjectByTag("Player", gameobject->parent);
	if (playerGO == nullptr)
	{
		LOG("The AOE didnt find the playerGO!");
	}
	else
	{
		playerScript = playerGO->GetComponent<PlayerMovement>();
		if (playerScript == nullptr)
		{
			LOG("The AOE didnt find the playerScript!");
		}
	}
}

void AOEBossScript::Start()
{
	prepParticlesGO = App->scene->FindGameObjectByName("Prep Particles", gameobject);
	if (prepParticlesGO == nullptr)
	{
		LOG("PrepParticles not found");
	}
	beamParticlesGO = App->scene->FindGameObjectByName("Beam Particles", gameobject);
	if (beamParticlesGO == nullptr)
	{
		LOG("beamParticlesGO not found");
	}
	boxTriggerGO = App->scene->FindGameObjectByName("Hitbox", gameobject);
	if (boxTriggerGO == nullptr)
	{
		LOG("boxTriggerGO not found");
	}
	else
	{
		boxTrigger = boxTriggerGO->GetComponent<ComponentBoxTrigger>();
		if (boxTrigger == nullptr)
		{
			LOG("boxTrigger not found");
		}
	}

	audio = App->scene->FindGameObjectByTag("audioExplosion", gameobject)->GetComponent<ComponentAudioSource>();
	if (audio == nullptr)
	{
		LOG("Audio for the AOE not found");
	}

	boxTrigger->Enable(false);
	boxTriggerGO->SetActive(false);
	prepParticlesGO->SetActive(true);
	beamParticlesGO->SetActive(false);
}

void AOEBossScript::Update()
{
	timer += App->time->gameDeltaTime;

	switch (circleType)
	{
		case 1:
			if (!hasExploded && timer > timerFade && timer < duration)
			{
				beamParticlesGO->SetActive(true);
				prepParticlesGO->SetActive(false);
				boxTriggerGO->SetActive(true);
				boxTrigger->Enable(true);
				audio->Play();
				hasExploded = true;
			}
			else if (timer > duration)
			{
				boxTriggerGO->SetActive(false);
				beamParticlesGO->SetActive(false);
				boxTrigger->Enable(false);
				gameobject->deleteFlag = true;
			}
			break;
		case 2:
			if (!hasExploded && timer > timerFade && timer < duration)
			{
				beamParticlesGO->SetActive(true);
				prepParticlesGO->SetActive(false);
				boxTriggerGO->SetActive(true);
				boxTrigger->Enable(true);
				audio->Play();
				hasExploded = true;
			}
			else if (timer > duration)
			{
				boxTriggerGO->SetActive(false);
				beamParticlesGO->SetActive(false);
				boxTrigger->Enable(false);

				SpawnSecondaryCircles();

				gameobject->deleteFlag = true;
			}
			break;
		case 3:
			break;
	}

}

void AOEBossScript::OnTriggerEnter(GameObject * go)
{
	if (go == playerGO && !hasDamaged)
	{
		playerScript->Damage(damageToPlayer);
		hasDamaged = true;
	}
}

void AOEBossScript::Expose(ImGuiContext * context)
{
	ImGui::DragFloat("Time until particles change", &timerFade,0.1f,0.0f,20.0f);
	ImGui::DragFloat("duration", &duration, 0.1f, 1.0f, 20.0f);
	ImGui::InputFloat("Damage", &damageToPlayer);
	ImGui::DragFloat("Spawn radius", &spawnRadius, 10.0f, 10.0f, 1000.0f);
}

void AOEBossScript::Serialize(JSON_value * json) const
{
	json->AddFloat("timerFade", timerFade);
	json->AddFloat("duration", duration);
	json->AddFloat("damageToPlayer", damageToPlayer);
	json->AddFloat("spawnRadius", spawnRadius);
}

void AOEBossScript::DeSerialize(JSON_value * json)
{
	timerFade = json->GetFloat("timerFade", 0.0f);
	duration = json->GetFloat("duration", 0.0f);
	damageToPlayer = json->GetFloat("damageToPlayer", 0.0f);
	spawnRadius = json->GetFloat("spawnRadius", 200.0f);
}

void AOEBossScript::SpawnSecondaryCircles()
{
	math::float3 defaultPos = gameobject->transform->GetPosition();

	float randZ = rand() % 300 - 150.0f;
	float randX = rand() % 300 - 150.0f;

	App->scene->Spawn(AOEPREFAB, math::float3(defaultPos.x + spawnRadius + randX, defaultPos.y, defaultPos.z + spawnRadius - randZ), math::Quat::identity);
	App->scene->Spawn(AOEPREFAB, math::float3(defaultPos.x + spawnRadius - randX, defaultPos.y, defaultPos.z - spawnRadius + randZ), math::Quat::identity);
	App->scene->Spawn(AOEPREFAB, math::float3(defaultPos.x - spawnRadius + randZ, defaultPos.y, defaultPos.z - spawnRadius - randX), math::Quat::identity);
	App->scene->Spawn(AOEPREFAB, math::float3(defaultPos.x - spawnRadius - randZ, defaultPos.y, defaultPos.z + spawnRadius + randX), math::Quat::identity);

}
