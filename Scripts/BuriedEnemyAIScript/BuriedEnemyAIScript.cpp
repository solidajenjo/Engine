#include "BuriedEnemyAIScript.h"

#include "EnemyControllerScript.h"

BuriedEnemyAIScript_API Script* CreateScript()
{
	BuriedEnemyAIScript* instance = new BuriedEnemyAIScript;
	return instance;
}
