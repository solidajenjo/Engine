#include "FearControllerScript.h"

#include "Application.h"

#include "ModuleScene.h"
#include "GameObject.h"

#include "BasicEnemyAIScript.h"
#include "EnemyControllerScript.h"

#include "imgui.h"
#include "Math/float3.h"


FearControllerScript_API Script* CreateScript()
{
	FearControllerScript* instance = new FearControllerScript;
	return instance;
}

void FearControllerScript::Awake()
{
}

void FearControllerScript::Start()
{
	GameObject* firstEnemy = App->scene->FindGameObjectByName(enemy1Name.c_str());
	if (firstEnemy == nullptr)
	{
		LOG("The GO %s couldn't be found \n", enemy1Name);
	}
	else
	{
		enemy1 = firstEnemy->GetComponent<BasicEnemyAIScript>();
		if (enemy1 == nullptr)
		{
			LOG("The enemy %s has no Script attached", firstEnemy->name);
		}
	}

	GameObject* secondEnemy = App->scene->FindGameObjectByName(enemy2Name.c_str());
	if (secondEnemy == nullptr)
	{
		LOG("The GO %s couldn't be found \n", enemy2Name);
	}
	else
	{
		enemy2 = secondEnemy->GetComponent<BasicEnemyAIScript>();
		if (enemy2 == nullptr)
		{
			LOG("The enemy %s has no Script attached", secondEnemy->name);
		}
	}

	GameObject* thirdEnemy = App->scene->FindGameObjectByName(enemy3Name.c_str());
	if (thirdEnemy == nullptr)
	{
		LOG("The GO %s couldn't be found \n", enemy3Name);
	}
	else
	{
		enemy3 = thirdEnemy->GetComponent<BasicEnemyAIScript>();
		if (enemy3 == nullptr)
		{
			LOG("The enemy %s has no Script attached", thirdEnemy->name);
		}
	}
}

void FearControllerScript::Update()
{
	if (IsSomeoneDead())
	{
		math::float3 deathPosition = LocateDeath();
		LaunchDeathEvent();
	}
}

void FearControllerScript::Expose(ImGuiContext * context)
{
	char* targetName = new char[64];
	strcpy_s(targetName, strlen(enemy1Name.c_str()) + 1, enemy1Name.c_str());
	ImGui::InputText("Enemy 1 Name", targetName, 64);
	enemy1Name = targetName;
	delete[] targetName;

	char* targetName2 = new char[64];
	strcpy_s(targetName2, strlen(enemy2Name.c_str()) + 1, enemy2Name.c_str());
	ImGui::InputText("Enemy 2 Name", targetName2, 64);
	enemy2Name = targetName2;
	delete[] targetName2;

	char* targetName3 = new char[64];
	strcpy_s(targetName3, strlen(enemy3Name.c_str()) + 1, enemy3Name.c_str());
	ImGui::InputText("Enemy 3 Name", targetName3, 64);
	enemy3Name = targetName3;
	delete[] targetName3;

}

void FearControllerScript::Serialize(JSON_value * json) const
{
}

void FearControllerScript::DeSerialize(JSON_value * json)
{
}

void FearControllerScript::LaunchDeathEvent()
{
	//maybe we should check positions



	if (enemy1 != nullptr)
		enemy1->scared = true;
	if (enemy2 != nullptr)
		enemy2->scared = true;
	if (enemy3 != nullptr)
		enemy3->scared = true;
}

math::float3 FearControllerScript::LocateDeath()
{
	if (enemy1->enemyController->isDead)
		return enemy1->enemyController->GetPosition();
}

bool FearControllerScript::IsSomeoneDead()
{
	return (enemy1->enemyController->isDead || enemy2->enemyController->isDead
		|| enemy3->enemyController->isDead);
}
