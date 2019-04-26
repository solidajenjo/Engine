#include "BasicEnemyAIScript.h"

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"

#include "Geometry/AABB.h"
#include "Math/Quat.h"
#include "Math/float4x4.h"
#include "imgui.h"
#include <stack>
#include "JSON.h"

BasicEnemyAIScript_API Script* CreateScript()
{
	BasicEnemyAIScript* instance = new BasicEnemyAIScript;
	return instance;
}

void BasicEnemyAIScript::Start()
{
	player = App->scene->FindGameObjectByName(App->scene->root, playerName.c_str());
	myBbox = &App->scene->FindGameObjectByName(gameObject, myBboxName.c_str())->bbox;
	playerBbox = &App->scene->FindGameObjectByName(player, playerBboxName.c_str())->bbox;
	startPosition = gameObject->transform->position;
	startPosition.y += yTranslation;

	anim = (ComponentAnimation*)gameObject->GetComponent(ComponentType::Animation);
	if (anim == nullptr) LOG("The GameObject %s has no Animation component attached \n", gameObject->name);
}

void BasicEnemyAIScript::Update()
{
	EnemyState previous = enemyState;

	switch (enemyState)
	{
	case EnemyState::WAIT:		Wait();						break;
	case EnemyState::STANDUP:	StandUp();					break;
	case EnemyState::CHASE:		Chase();					break;
	case EnemyState::RETURN:	ReturnToStartPosition();	break;
	case EnemyState::LAYDOWN:	Laydown();					break;
	case EnemyState::ATTACK:	Attack();					break;
	case EnemyState::COOLDOWN:	Cooldown();					break;
	case EnemyState::DEAD:									break;
	default:
		break;
	}

	// Check animation to play
	if(anim != nullptr)
		CheckStateChange(previous, enemyState);
}

void BasicEnemyAIScript::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::Separator();
	ImGui::Text("Enemy:");

	char* bboxName = new char[64];
	strcpy_s(bboxName, strlen(myBboxName.c_str()) + 1, myBboxName.c_str());
	ImGui::InputText("My BBox Name", bboxName, 64);
	myBboxName = bboxName;
	delete[] bboxName;

	switch (enemyState)
	{
	case EnemyState::WAIT:		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Wait");		break;
	case EnemyState::STANDUP:	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Stand-Up");	break;
	case EnemyState::CHASE:		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Chase");		break;
	case EnemyState::RETURN:	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Return");	break;
	case EnemyState::LAYDOWN:	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Laydown");	break;
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

	ImGui::Separator();
	ImGui::Text("Player:");

	char* goName = new char[64];
	strcpy_s(goName, strlen(playerName.c_str()) + 1, playerName.c_str());
	ImGui::InputText("playerName", goName, 64);
	playerName = goName;
	delete[] goName;

	char* targetBboxName = new char[64];
	strcpy_s(targetBboxName, strlen(playerBboxName.c_str()) + 1, playerBboxName.c_str());
	ImGui::InputText("Player BBox Name", targetBboxName, 64);
	playerBboxName = targetBboxName;
	delete[] targetBboxName;
}

void BasicEnemyAIScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddString("playerName", playerName.c_str());
	json->AddString("playerBboxName", playerBboxName.c_str());
	json->AddString("myBboxName", myBboxName.c_str());

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

void BasicEnemyAIScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	playerName = json->GetString("playerName");
	playerBboxName = json->GetString("playerBboxName");
	myBboxName = json->GetString("myBboxName");

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

void BasicEnemyAIScript::Wait()
{
	if (player == nullptr)
		return;

	float3 enemyCurrentPosition = gameObject->transform->GetGlobalPosition();
	enemyCurrentPosition.y = 0.0f;
	float3 playerCurrentPosition = player->transform->GetGlobalPosition();
	playerCurrentPosition.y = 0.0f;

	float distance = enemyCurrentPosition.Distance(playerCurrentPosition);

	if (distance < activationDistance)
	{
		enemyState = EnemyState::STANDUP;
	}
}

void BasicEnemyAIScript::StandUp()
{
	// Translate on the Z axis
	float3 movement = gameObject->transform->up.Normalized() * standupSpeed * App->time->gameDeltaTime;
	gameObject->transform->SetPosition(gameObject->transform->GetPosition() + movement);
	auxTranslation += movement.y;

	// Check if the needed Z has been reached
	if (yTranslation <= auxTranslation)
	{
		enemyState = EnemyState::CHASE;
		auxTranslation = 0.0f;

		// Fix to avoid rotating in other axis
		gameObject->transform->position.y = player->transform->position.y;
	}
}

void BasicEnemyAIScript::Chase()
{
	if (player == nullptr)
		return;

	LookAtPlayer();
	MoveTowards(chaseSpeed);

	// Check collision
	if (myBbox != nullptr && myBbox->Intersects(*playerBbox))
	{
		// Player intersected, change to attack
		enemyState = EnemyState::ATTACK;
	}
	else
	{
		// Check if player is too far
		float3 enemyCurrentPosition = gameObject->transform->GetGlobalPosition();
		enemyCurrentPosition.y = 0.0f;
		float3 playerCurrentPosition = player->transform->GetGlobalPosition();
		playerCurrentPosition.y = 0.0f;

		float distance = enemyCurrentPosition.Distance(playerCurrentPosition);

		if (distance > returnDistance)
		{
			// Return to start position
			enemyState = EnemyState::RETURN;
		}
	}
}

void BasicEnemyAIScript::ReturnToStartPosition()
{
	math::float3 enemyCurrentPosition = gameObject->transform->GetGlobalPosition();

	// Look at start position
	math::float3 dir = (enemyCurrentPosition - startPosition);
	math::Quat currentRotation = gameObject->transform->GetRotation();
	math::Quat rotation = currentRotation.LookAt(gameObject->transform->front.Normalized(), dir.Normalized(), float3::unitY, float3::unitY);
	gameObject->transform->SetRotation(rotation.Mul(currentRotation));

	MoveTowards(returnSpeed);

	// Check distance to player
	enemyCurrentPosition.y = 0.0f;
	float3 playerCurrentPosition = player->transform->GetGlobalPosition();
	playerCurrentPosition.y = 0.0f;

	float distance = enemyCurrentPosition.Distance(playerCurrentPosition);
	if (distance < activationDistance)
	{
		enemyState = EnemyState::CHASE;
	}
	else if (startPosition.Distance(enemyCurrentPosition) < 1.0f)
	{
		enemyState = EnemyState::LAYDOWN;
	}
}

void BasicEnemyAIScript::Laydown()
{
	// Translate on the Z axis
	float3 movement = gameObject->transform->up.Normalized() * standupSpeed * App->time->gameDeltaTime;
	gameObject->transform->SetPosition(gameObject->transform->GetPosition() - movement);
	auxTranslation += movement.y;

	// Check if the needed Z has been reached
	if (yTranslation <= auxTranslation)
	{
		enemyState = EnemyState::WAIT;
		auxTranslation = 0.0f;
	}
}

void BasicEnemyAIScript::Attack()
{
	LookAtPlayer();

	if (myBbox != nullptr && !myBbox->Intersects(*playerBbox))
	{
		enemyState = EnemyState::CHASE;
	}
	else
	{
		auxTimer = App->time->gameTime;
		enemyState = EnemyState::COOLDOWN;
	}
}

void BasicEnemyAIScript::Cooldown()
{
	float waitedTime = (App->time->gameTime - auxTimer);

	if (waitedTime > cooldownTime)
	{
		enemyState = EnemyState::ATTACK;
		auxTimer = 0.0f;
	}
}

void BasicEnemyAIScript::LookAtPlayer()
{
	math::float3 enemyCurrentPosition = gameObject->transform->GetGlobalPosition();
	math::float3 playerCurrentPosition = player->transform->GetGlobalPosition();

	// Look at player
	math::float3 dir = (enemyCurrentPosition - playerCurrentPosition);
	math::Quat currentRotation = gameObject->transform->GetRotation();
	math::Quat rotation = currentRotation.LookAt(gameObject->transform->front.Normalized(), dir.Normalized(), float3::unitY, float3::unitY);
	gameObject->transform->SetRotation(rotation.Mul(currentRotation));
}

void BasicEnemyAIScript::MoveTowards(float speed) const
{
	math::float3 movement = gameObject->transform->front.Normalized() * -speed * App->time->gameDeltaTime;
	gameObject->transform->SetPosition(gameObject->transform->GetPosition() + movement);
}

void BasicEnemyAIScript::CheckStateChange(EnemyState previous, EnemyState newState)
{
	if (previous != newState)
	{
		switch (newState)
		{
		case EnemyState::WAIT:
			anim->SendTriggerToStateMachine("Wait");
			break;
		case EnemyState::STANDUP:
			anim->SendTriggerToStateMachine("StandUp");
			break;
		case EnemyState::CHASE:
			anim->SendTriggerToStateMachine("Chase");
			break;
		case EnemyState::RETURN:
			anim->SendTriggerToStateMachine("Return");
			break;
		case EnemyState::LAYDOWN:
			anim->SendTriggerToStateMachine("Laydown");
			break;
		case EnemyState::ATTACK:
			anim->SendTriggerToStateMachine("Attack");
			break;
		case EnemyState::COOLDOWN:
			anim->SendTriggerToStateMachine("Cooldown");
			break;
		case EnemyState::DEAD:
			anim->SendTriggerToStateMachine("Dead");
			break;
		default:
			break;

		}
	}
}
