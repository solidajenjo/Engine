#include "Application.h"
#include "ModuleTime.h"
#include "ModuleScene.h"
#include "ModuleNavigation.h"

#include "GameObject.h"

#include "ComponentTransform.h"
#include "ComponentRenderer.h"

#include "BossBehaviourScript.h"
#include "EnemyControllerScript/EnemyControllerScript.h"

#include "BossState.h"
#include "BossStateIdle.h"
#include "BossStateNotActive.h"
#include "BossStateActivated.h"
#include "BossStateDeath.h"
#include "BossStateCast.h"
#include "BossStatePreCast.h"
#include "BossStateInterPhase.h"

#include "imgui.h"
#include <stdlib.h>

#include "JSON.h"

#define FIRSTAOE "AOEFirstPhase"
#define SECONDAOE "AOESecondPhase"
#define THIRDAOE "AOEThirdPhase"

#define FIRSTSUMMON "BasicEnemy"
#define FIRSTRANGEDSUMMON "BandoleroPrefab"

#define BOSSPROJECTILE "BossProjectilePrefab"

BossBehaviourScript_API Script* CreateScript()
{
	BossBehaviourScript* instance = new BossBehaviourScript;
	return instance;
}

void BossBehaviourScript::Awake()
{
	// Look for Enemy Controller Script of the enemy
	enemyController = gameobject->GetComponent<EnemyControllerScript>();
	if (enemyController == nullptr)
	{
		LOG("The GameObject %s has no Enemy Controller Script component attached \n", gameobject->name.c_str());
	}

	firstMeshFloor = App->scene->FindGameObjectByTag("firstFloor", App->scene->root);
	if (firstMeshFloor == nullptr)
	{
		LOG("first mesh not found");
	}
	else
	{
		floorGOs.push_back(firstMeshFloor);
	}

	secondMeshFloor = App->scene->FindGameObjectByTag("secondFloor", App->scene->root);
	if (secondMeshFloor == nullptr)
	{
		LOG("second mesh not found");
	}
	else
	{
		floorGOs.push_back(secondMeshFloor);
	}

}

void BossBehaviourScript::Start()
{
	startingPoint = enemyController->GetPosition();
	bossPhase = BossPhase::None;

	bossStates.reserve(7);
	bossStates.push_back(notActive = new BossStateNotActive(this));
	bossStates.push_back(activated = new BossStateActivated(this));
	bossStates.push_back(idle = new BossStateIdle(this));
	bossStates.push_back(cast = new BossStateCast(this));
	bossStates.push_back(precast = new BossStatePreCast(this));
	bossStates.push_back(interPhase = new BossStateInterPhase(this));
	bossStates.push_back(death = new BossStateDeath(this));

	currentState = notActive;

}

void BossBehaviourScript::Update()
{
	GetPositionVariables();

	BossState* previous = currentState;
	
	FloatInSpace();
	HandleSkills();
	
	currentState->HandleIA();
	CheckHealth();

	CheckStates(previous);
	currentState->UpdateTimer();
	currentState->Update();
	
}

void BossBehaviourScript::Expose(ImGuiContext * context)
{
	//Boss state check
	if (currentState == notActive)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Not active");
	else if (currentState == activated)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Activated");
	else if (currentState == death)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Dead");
	else if (currentState == interPhase)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: interPhase");
	else if (currentState == precast)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Precast");
	else if (currentState == cast)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Cast");
	else if (currentState == idle)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: idle");

	if (bossPhase == BossPhase::First)			ImGui::TextColored(ImVec4(0, 1, 0, 1), "First phase");
	else if(bossPhase == BossPhase::Second)		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Second phase");
	else if (bossPhase == BossPhase::Third)		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Third phase");

	if (lastUsedSkill == BossSkill::Aoe)			ImGui::TextColored(ImVec4(0, 1, 1, 1), "AOE skill");
	else if(lastUsedSkill == BossSkill::Summon)		ImGui::TextColored(ImVec4(0, 1, 1, 1), "Summon skill");
	else if (lastUsedSkill == BossSkill::Teleport)	ImGui::TextColored(ImVec4(0, 1, 1, 1), "Teleport skill");


	ImGui::Text("Floating parameters:");
	ImGui::DragFloat("floating constant:", &floatConstant, 0.1f,0.0f,10.0f);
	ImGui::DragFloat("angle constant:", &angleConstant, 0.1f, 0.0f, 10.0f);

	ImGui::Separator();
	ImGui::Text("Positions first cutscene");
	ImGui::InputFloat3("HighPoint position", (float*)&highPointFirstCS);
	ImGui::InputFloat3("Start Fight position", (float*)&pointStartFight);

	ImGui::Separator();
	ImGui::Text("Fight parameters");
	ImGui::DragFloat("Activation Distance", &activationDistance, 10.0f,500.0f,10000.0f);
	ImGui::DragFloat("Activation time", &activationTime, 0.1f, 0.5f, 20.0f);
	ImGui::Separator();
	ImGui::DragFloat("Circle noise", &circleNoise, 1.0f, 1.0f, 500.0f);
	ImGui::Separator();
	ImGui::InputInt("Skulls first phase", &skullsToShootFirst);
	ImGui::InputFloat3("top TP", (float*)&topTP);
	ImGui::InputFloat3("bottom TP", (float*)&bottomTP);
	ImGui::InputFloat3("left TP", (float*)&leftTP);
	ImGui::InputFloat3("right TP", (float*)&rightTP);
}

void BossBehaviourScript::Serialize(JSON_value * json) const
{
	json->AddFloat("floatConstant", floatConstant);
	json->AddFloat("angleConstant", angleConstant);
	json->AddFloat("activationDistance", activationDistance);
	json->AddFloat("activationTime", activationTime);
	
	json->AddFloat3("highPointFirstCS", highPointFirstCS);
	json->AddFloat3("pointStartFight", pointStartFight);

	json->AddInt("skullsToShootFirst", skullsToShootFirst);
	json->AddFloat3("topTP",topTP);
	json->AddFloat3("bottomTP",bottomTP);
	json->AddFloat3("leftTP",leftTP);
	json->AddFloat3("rightTP",rightTP);

	json->AddFloat("circleNoise", circleNoise);
}

void BossBehaviourScript::DeSerialize(JSON_value * json)
{
	floatConstant = json->GetFloat("floatConstant", 0.0f);
	angleConstant = json->GetFloat("angleConstant", 0.0f);
	activationDistance = json->GetFloat("activationDistance", 0.0f);
	activationTime = json->GetFloat("activationTime", 0.0f);
	highPointFirstCS = json->GetFloat3("highPointFirstCS");
	pointStartFight = json->GetFloat3("pointStartFight");

	skullsToShootFirst = json->GetInt("skullsToShootFirst", 5);
	topTP = json->GetFloat3("topTP");
	bottomTP = json->GetFloat3("bottomTP");
	leftTP = json->GetFloat3("leftTP");
	rightTP = json->GetFloat3("rightTP");	

	circleNoise = json->GetFloat("circleNoise",0.0f);
}

void BossBehaviourScript::GenerateNewNavigability()
{
	App->navigation->GenerateNavigabilityFromGOs(floorGOs);
}

void BossBehaviourScript::GetPositionVariables()
{
	distanceToPlayer = enemyController->GetDistanceToPlayer2D();
	playerPosition = enemyController->GetPlayerPosition();
	currentPosition = enemyController->GetPosition();
	currentRotation = enemyController->GetRotation();
}

void BossBehaviourScript::CheckStates(BossState * previous)
{
	if (previous != currentState)
	{
		previous->ResetTimer();

		previous->Exit();
		currentState->Enter();
	}
}

void BossBehaviourScript::CheckHealth()
{
	float actualHealth = enemyController->GetHealth();
	float maxHealth = enemyController->GetMaxHealth();

	float healthPerc = actualHealth / maxHealth;
	switch (bossPhase)
	{
		case BossPhase::None:
			break;
		case BossPhase::First:
			if (healthPerc < firstHealthThreshold)
			{
				bossPhase = BossPhase::Second;
			}
			break;
		case BossPhase::Second:
			if (healthPerc < secondHealthThreshold)
			{
				bossPhase = BossPhase::Third;
				currentState = interPhase;
				isFloating = false;
			}
			break;
		case BossPhase::Third:
			break;
	}


}

void BossBehaviourScript::FloatInSpace()
{
	if (isFloating)
	{
		yOffset = floatConstant * sin(angle);
		angle += angleConstant * App->time->gameDeltaTime;

		math::float3 enemyPosition = enemyController->GetPosition();
		enemyController->SetPosition({ enemyPosition.x,enemyPosition.y + yOffset,enemyPosition.z });
	}
}

void BossBehaviourScript::HandleSkills()
{
	if (circlesSpawning)
	{
		circlesTimer += App->time->gameDeltaTime;

		switch (bossPhase)
		{
			case BossPhase::First:
				HandleFirstPhaseCircles();
				break;
			case BossPhase::Second:
				HandleSecondPhaseCircles();
				break;
			case BossPhase::Third:
				HandleThirdPhaseCircles();
				break;
		}
	}
	if (bossSummoning)
	{
		switch (bossPhase)
		{
		case BossPhase::First:
			HandleFirstSummons();
			break;
		case BossPhase::Second:
			HandleSecondSummons();
			break;
		case BossPhase::Third:
			HandleThirdSummons();
			break;
		}
	}
	if (bossTeleporting)
	{
		switch (bossPhase)
		{
			case BossPhase::First:
				HandleFirstTP();
				break;
			case BossPhase::Second:
				HandleSecondTP();
				break;
			case BossPhase::Third:
				break;
		}
	}
}

void BossBehaviourScript::HandleFirstTP()
{
	ComponentRenderer* mainRender = enemyController->GetMainRenderer();

	switch (teleportState)
	{
	case TPState::None:
		teleportState = TPState::FadeOut;
		break;
	case TPState::FadeOut:
		mainRender->dissolveAmount += App->time->gameDeltaTime;
		if (mainRender->dissolveAmount >= 1.0f)
		{
			teleportState = TPState::Relocate;
		}	
		break;
	case TPState::Relocate:
		TPtoLocation(ChooseNextTP(currentLocation));
		teleportState = TPState::FadeIn;
		break;
	case TPState::FadeIn:
		mainRender->dissolveAmount -= App->time->gameDeltaTime;
		if (mainRender->dissolveAmount <= 0.0f)
		{
			teleportState = TPState::Projectiles;
		}
		break;
	case TPState::Projectiles:
		skullsTimer += App->time->gameDeltaTime;

		if (skullsTimer > timeBetweenSkulls)
		{
			float randZ = rand() % 300 - 150.0f;
			float randY = rand() % 300 - 150.0f;
			GameObject* skull = App->scene->Spawn(BOSSPROJECTILE, currentPosition +
				math::float3(200.0f, randY + 100.0f, randZ), currentRotation);
			++numberSkullsShot;
			skullsTimer = 0.0f;
			if (numberSkullsShot >= skullsToShootFirst)
			{
				numberSkullsShot = 0;
				teleportState = TPState::Finished;
			}
		}
			break;
	case TPState::Finished:
		bossTeleporting = false;
		teleportState = TPState::None;
		break;
	}

	//We first make it disappear then we TP and then we show her again
}

void BossBehaviourScript::HandleSecondTP()
{

}

TPlocations BossBehaviourScript::ChooseNextTP(TPlocations currentLoc)
{
	TPlocations nextLocation;

	if (currentLoc == TPlocations::None)
	{
		nextLocation = TPlocations::Left;
	}
	else
	{
		nextLocation = currentLoc;

		while (currentLoc == nextLocation)
		{
			int randomInt = rand() % 4;

			switch (randomInt)
			{
			case 0:
				nextLocation = TPlocations::Top;
				break;
			case 1:
				nextLocation = TPlocations::Bottom;
				break;
			case 2:
				nextLocation = TPlocations::Right;
				break;
			case 3:
				nextLocation = TPlocations::Left;
				break;
			}
		}
	}

	currentLocation = nextLocation;

	return nextLocation;
}

void BossBehaviourScript::TPtoLocation(TPlocations tpLoc)
{
	switch (tpLoc)
	{
	case TPlocations::Left:
		enemyController->SetPosition(leftTP);
		break;
	case TPlocations::Right:
		enemyController->SetPosition(rightTP);
		break;
	case TPlocations::Top:
		enemyController->SetPosition(topTP);
		break;
	case TPlocations::Bottom:
		enemyController->SetPosition(bottomTP);
		break;
	}
}

void BossBehaviourScript::HandleFirstPhaseCircles()
{
	if (circlesTimer > timeBetweenCirclesFirst && circlesCast < circlesInFirstPhase)
	{
		float randX = (float(rand() % 100) - 50.f) / 100.f * circleNoise;
		float randZ = (float(rand() % 100) - 50.f) / 100.f * circleNoise;

		math::float3 newPosition = playerPosition + math::float3(randX, 50, randZ);

		App->scene->Spawn(FIRSTAOE, newPosition, math::Quat::identity, App->scene->root);
		circlesTimer = 0.0f;
		++circlesCast;
	}
	else if (circlesCast >= circlesInFirstPhase)
	{
		circlesCast = 0;
		circlesTimer = 0.0f;
		circlesSpawning = false;
	}
	
}

void BossBehaviourScript::HandleSecondPhaseCircles()
{
}

void BossBehaviourScript::HandleThirdPhaseCircles()
{
}


void BossBehaviourScript::HandleFirstSummons()
{
	math::float3 bossPosition = enemyController->GetPosition();
	math::float3 vectorPlayerBoss = playerPosition - bossPosition;
	vectorPlayerBoss.Normalize();

	math::float3 sideVector = vectorPlayerBoss.Cross(math::float3::unitY);
	sideVector.Normalize();

	math::Quat bossRotation = enemyController->GetRotation();

	math::float3 positionFirst = bossPosition + sideVector * 400.0f + vectorPlayerBoss * 200.0f;
	positionFirst.y = playerPosition.y;

	math::float3 positionSecond = bossPosition - sideVector * 400.0f + vectorPlayerBoss * 200.0f;
	positionSecond.y = playerPosition.y;

	App->scene->Spawn(FIRSTSUMMON, positionFirst, bossRotation);
	GameObject* bandolero = App->scene->Spawn(FIRSTRANGEDSUMMON, math::float3::zero, math::Quat::identity);
	GameObject* controller = App->scene->FindGameObjectByTag("Controller", bandolero);
	controller->transform->SetPosition(positionSecond);
	controller->transform->SetRotation(bossRotation);

	bossSummoning = false;
}

void BossBehaviourScript::HandleSecondSummons()
{
}

void BossBehaviourScript::HandleThirdSummons()
{
}
