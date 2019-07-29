#include "BuriedEnemyAIScript.h"

#include "EnemyControllerScript.h"

#include "GameObject.h"

BuriedEnemyAIScript_API Script* CreateScript()
{
	BuriedEnemyAIScript* instance = new BuriedEnemyAIScript;
	return instance;
}

void BuriedEnemyAIScript::Awake()
{
	// Look for Enemy Controller Script of the enemy
	enemyController = gameobject->GetComponent<EnemyControllerScript>();
	if (enemyController == nullptr)
	{
		LOG("The GameObject %s has no Enemy Controller Script component attached \n", gameobject->name);
	}
}

void BuriedEnemyAIScript::Start()
{
	//Create states
	enemyStates.reserve(9);

}

void BuriedEnemyAIScript::Update()
{
}

void BuriedEnemyAIScript::Expose(ImGuiContext * context)
{
}

void BuriedEnemyAIScript::Serialize(JSON_value * json) const
{
}

void BuriedEnemyAIScript::DeSerialize(JSON_value * json)
{
}
