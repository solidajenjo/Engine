#include "HiddenEnemyAIScript.h"

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"
#include "ComponentRenderer.h"

#include "EnemyControllerScript.h"
#include "PlayerMovement.h"

#include "Geometry/AABB.h"
#include "Math/Quat.h"
#include "Math/float4x4.h"
#include "imgui.h"
#include <stack>
#include "JSON.h"

HiddenEnemyAIScript_API Script* CreateScript()
{
	HiddenEnemyAIScript* instance = new HiddenEnemyAIScript;
	return instance;
}

void HiddenEnemyAIScript::Start()
{
	startPosition = gameobject->transform->position;
	startPosition.y += yTranslation;

	enemyController = gameobject->GetComponent<EnemyControllerScript>();
	playerScript = App->scene->FindGameObjectByName("Player")->GetComponent<PlayerMovement>();

	//anim = (ComponentAnimation*)gameobject->GetComponent(ComponentType::Animation);
	//if (anim == nullptr) LOG("The GameObject %s has no Animation component attached \n", gameobject->name);
}

void HiddenEnemyAIScript::Update()
{
	if (enemyController->player == nullptr)
		return;

	if (enemyController->GetHealth() < 1)
		enemyState = EnemyState::DEAD;

	EnemyState previous = enemyState;

	switch (enemyState)
	{
	case EnemyState::WAIT:		Wait();					break;
	case EnemyState::SHOW_UP:	StandUp();				break;
	case EnemyState::CHASE:		Chase();				break;
	case EnemyState::RETURN:	ReturnToStartPosition(); break;
	case EnemyState::HIDE:		Laydown();				break;
	case EnemyState::ATTACK:	Attack();				break;
	case EnemyState::COOLDOWN:	Cooldown();				break;
	case EnemyState::DEAD:		Die();					break;
	default:
		break;
	}

	// Check animation to play
	//if(anim != nullptr)
	CheckStateChange(previous, enemyState);
}

void HiddenEnemyAIScript::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::Separator();
	ImGui::Text("Enemy:");

	switch (enemyState)
	{
	case EnemyState::WAIT:		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Wait");		break;
	case EnemyState::SHOW_UP:	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Stand-Up");	break;
	case EnemyState::CHASE:		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Chase");		break;
	case EnemyState::RETURN:	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Return");	break;
	case EnemyState::HIDE:	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Laydown");	break;
	case EnemyState::ATTACK:	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Attack");	break;
	case EnemyState::COOLDOWN:	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Cooldown");	break;
	case EnemyState::DEAD:		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Dead");		break;
	default:
		break;
	}

	ImGui::Text("Stand up:");
	ImGui::InputFloat("Distance to activate", &activationDistance);
	ImGui::InputFloat("Stand-up Speed", &standupSpeed);
	ImGui::Text("Chase:");
	ImGui::InputFloat("Y Translation", &yTranslation);
	ImGui::InputFloat("Chase Speed", &chaseSpeed);
	ImGui::Text("Return:");
	ImGui::InputFloat("Return Distance", &returnDistance);
	ImGui::InputFloat("Return Speed", &returnSpeed);
	ImGui::Text("Cooldown:");
	ImGui::InputFloat("Cooldown Time", &cooldownTime);
}

void HiddenEnemyAIScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);

	//Wait variables
	json->AddFloat("activationDistance", activationDistance);

	// Stand-Up variables
	json->AddFloat("standupSpeed", standupSpeed);
	json->AddFloat("yTranslation", yTranslation);

	// Chase variables
	json->AddFloat("chaseSpeed", chaseSpeed);

	// Return variables
	json->AddFloat("returnSpeed", returnSpeed);
	json->AddFloat("returnDistance", returnDistance);

	// Cooldown variables
	json->AddFloat("cooldownTime", cooldownTime);
}

void HiddenEnemyAIScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);

	//Wait variables
	activationDistance = json->GetFloat("activationDistance");

	// Stand-Up variables
	standupSpeed = json->GetFloat("standupSpeed");
	yTranslation = json->GetFloat("yTranslation");

	// Chase variables
	chaseSpeed = json->GetFloat("chaseSpeed");

	// Return variables
	returnSpeed = json->GetFloat("returnSpeed");
	returnDistance = json->GetFloat("returnDistance");

	// Cooldown variables
	cooldownTime = json->GetFloat("cooldownTime");
}

void HiddenEnemyAIScript::Wait()
{
	float3 enemyCurrentPosition = gameobject->transform->GetGlobalPosition();
	enemyCurrentPosition.y = 0.0f;
	float3 playerCurrentPosition = enemyController->player->transform->GetGlobalPosition();
	playerCurrentPosition.y = 0.0f;

	float distance = enemyCurrentPosition.Distance(playerCurrentPosition);

	if (distance < activationDistance)
	{
		enemyState = EnemyState::SHOW_UP;
	}
}

void HiddenEnemyAIScript::StandUp()
{
	// Translate on the Z axis
	float3 movement = gameobject->transform->up.Normalized() * standupSpeed * App->time->gameDeltaTime;
	gameobject->transform->SetPosition(gameobject->transform->GetPosition() + movement);
	auxTranslation += movement.y;

	// Check if the needed Z has been reached
	if (yTranslation <= auxTranslation)
	{
		enemyState = EnemyState::CHASE;
		auxTranslation = 0.0f;
	}
}

void HiddenEnemyAIScript::Chase()
{
	// Look at player and move towards
	gameobject->transform->LookAt(enemyController->player->transform->position);
	MoveTowards(chaseSpeed);

	// Check collision
	if (enemyController->myBbox != nullptr && enemyController->myBbox->Intersects(*enemyController->playerBbox))
	{
		// Player intersected, change to attack
		enemyState = EnemyState::ATTACK;
	}
	else
	{
		// Check if player is too far
		float3 enemyCurrentPosition = gameobject->transform->GetGlobalPosition();
		enemyCurrentPosition.y = 0.0f;
		float3 playerCurrentPosition = enemyController->player->transform->GetGlobalPosition();
		playerCurrentPosition.y = 0.0f;

		float distance = enemyCurrentPosition.Distance(playerCurrentPosition);

		if (distance > returnDistance)
		{
			// Return to start position
			enemyState = EnemyState::RETURN;
		}
	}
}

void HiddenEnemyAIScript::ReturnToStartPosition()
{
	// Look at start position and move towards
	gameobject->transform->LookAt(startPosition);
	MoveTowards(returnSpeed);

	// Check distance to player
	math::float3 enemyCurrentPosition = gameobject->transform->GetGlobalPosition();
	enemyCurrentPosition.y = startPosition.y;
	float3 playerCurrentPosition = enemyController->player->transform->GetGlobalPosition();
	playerCurrentPosition.y = startPosition.y;

	float distance = enemyCurrentPosition.Distance(playerCurrentPosition);
	if (distance < activationDistance)
	{
		enemyState = EnemyState::CHASE;
	}
	else if (startPosition.Distance(enemyCurrentPosition) < 1.5f)
	{
		enemyState = EnemyState::HIDE;
	}
}

void HiddenEnemyAIScript::Laydown()
{
	// Translate on the Z axis
	float3 movement = gameobject->transform->up.Normalized() * standupSpeed * App->time->gameDeltaTime;
	gameobject->transform->SetPosition(gameobject->transform->GetPosition() - movement);
	auxTranslation += movement.y;

	// Check if the needed Z has been reached
	if (yTranslation <= auxTranslation)
	{
		enemyState = EnemyState::WAIT;
		auxTranslation = 0.0f;
	}
}

void HiddenEnemyAIScript::Attack()
{
	// Keep looking at player
	gameobject->transform->LookAt(enemyController->player->transform->position);

	if (enemyController->myBbox != nullptr && !enemyController->myBbox->Intersects(*enemyController->playerBbox))
	{
		enemyState = EnemyState::CHASE;
	}
	else
	{
		// TODO: Add function to make damage to the player

		playerScript->Damage(damage);
		auxTimer = App->time->gameTime;
		enemyState = EnemyState::COOLDOWN;
	}
}

void HiddenEnemyAIScript::Cooldown()
{
	float waitedTime = (App->time->gameTime - auxTimer);

	if (waitedTime > cooldownTime)
	{
		enemyState = EnemyState::ATTACK;
		auxTimer = 0.0f;
	}
}

void HiddenEnemyAIScript::Die()
{
	gameobject->SetActive(false);
}

void HiddenEnemyAIScript::MoveTowards(float speed) const
{
	math::float3 movement = gameobject->transform->front.Normalized() * -speed * App->time->gameDeltaTime;
	gameobject->transform->SetPosition(gameobject->transform->GetPosition() + movement);
}

void HiddenEnemyAIScript::CheckStateChange(EnemyState previous, EnemyState newState)
{
	if (previous != newState)
	{
		switch (newState)
		{
		case EnemyState::WAIT:
			//anim->SendTriggerToStateMachine("Wait");
			enemyController->myRender->SetMaterial("White");
			break;
		case EnemyState::SHOW_UP:
			//anim->SendTriggerToStateMachine("StandUp");
			enemyController->myRender->SetMaterial("Blue");
			break;
		case EnemyState::CHASE:
			//anim->SendTriggerToStateMachine("Chase");
			enemyController->myRender->SetMaterial("Yellow");
			break;
		case EnemyState::RETURN:
			//anim->SendTriggerToStateMachine("Return");
			enemyController->myRender->SetMaterial("Green");
			break;
		case EnemyState::HIDE:
			//anim->SendTriggerToStateMachine("Laydown");
			enemyController->myRender->SetMaterial("Blue");
			break;
		case EnemyState::ATTACK:
			//anim->SendTriggerToStateMachine("Attack");
			enemyController->myRender->SetMaterial("Red");
			break;
		case EnemyState::COOLDOWN:
			//anim->SendTriggerToStateMachine("Cooldown");
			enemyController->myRender->SetMaterial("White");
			break;
		case EnemyState::DEAD:
			//anim->SendTriggerToStateMachine("Dead");
			break;
		default:
			break;

		}
	}
}