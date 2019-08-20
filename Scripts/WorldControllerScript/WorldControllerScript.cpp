#include "WorldControllerScript.h"

#include "Application.h"
#include "GameObject.h"
#include "ModuleNavigation.h"

#include "Globals.h"

WorldControllerScript_API Script* CreateScript()
{
	WorldControllerScript* instance = new WorldControllerScript;
	return instance;
}

WorldControllerScript::WorldControllerScript()
{
}

WorldControllerScript::~WorldControllerScript()
{
}

void WorldControllerScript::resetWorldAgents()
{
	crowdToolPointer.reset();//delete smart pointer calling the destructor
	crowdToolPointer = std::make_unique<crowdTool>();//create a new instance
}

void WorldControllerScript::setPlayer(GameObject* player)
{
	playerUID = player->UUID;
	
}

void WorldControllerScript::addEnemy(GameObject* enemy)
{
	
	//enemies.push_back(enemy);
}