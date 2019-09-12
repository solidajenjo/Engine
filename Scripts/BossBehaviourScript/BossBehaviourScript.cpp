#include "BossBehaviourScript.h"

#include "EnemyControllerScript/EnemyControllerScript.h"
#include "GameObject.h"

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

}

void BossBehaviourScript::Start()
{
}

void BossBehaviourScript::Update()
{
}

void BossBehaviourScript::Expose(ImGuiContext * context)
{
}

void BossBehaviourScript::Serialize(JSON_value * json) const
{
}

void BossBehaviourScript::DeSerialize(JSON_value * json)
{
}
