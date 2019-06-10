#include "ExperienceController.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "ModuleInput.h"

#include "GameObject.h"
#include "ComponentText.h"
#include "ComponentImage.h"

#include "SkillTreeController.h"

ExperienceController_API Script* CreateScript()
{
	ExperienceController* instance = new ExperienceController;
	return instance;
}

void ExperienceController::Start()
{
	GameObject* levelInventory = App->scene->FindGameObjectByName("LevelBackground");
	assert(levelInventory != nullptr);
	xpProgressInventory = App->scene->FindGameObjectByName("LevelProgress", levelInventory)->GetComponent<ComponentImage>();
	assert(xpProgressInventory != nullptr);
	xpProgressHUD = App->scene->FindGameObjectByName("XpProgress", gameobject)->GetComponent<ComponentImage>();
	assert(xpProgressHUD != nullptr);
	levelText = App->scene->FindGameObjectByName("LevelActual", levelInventory)->GetComponent<Text>();
	assert(levelText != nullptr);
	xpText = App->scene->FindGameObjectByName("LevelExperience", levelInventory)->GetComponent<Text>();
	assert(xpText != nullptr);

	skillTreeScript = App->scene->FindGameObjectByName("Skills")->GetComponent<SkillTreeController>();
}

void ExperienceController::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		AddXP(42);
	}
	//if (updateXP)
	//{
	//	//*100 * App->time->gameDeltaTime
	//	xpProgressHUD->SetMaskAmount();
	//}
}

void ExperienceController::AddXP(int xp)
{
	totalXPAcumulated += xp;
	if (currentLevel < maxLevel)
	{
		updateXP = true;
		previousXP = currentXP;
		currentXP += xp;
		if (currentXP >= maxXPLevel)
		{
			while (currentXP >= maxXPLevel)
			{
				currentXP -= maxXPLevel;
				++currentLevel;
				maxXPLevel = levelsExp[currentLevel - 1];
				skillTreeScript->AddSkillPoint();
				levelUP = true;
				if (currentLevel == maxLevel)
				{
					currentXP == maxXPLevel;
					break;
				}
			}
			levelText->text = "LVL " + std::to_string(currentLevel);
		}
		int mask = (currentXP * 100) / maxXPLevel;
		xpText->text = std::to_string(currentXP) + "/" + std::to_string(maxXPLevel);
		xpProgressHUD->SetMaskAmount(mask);
		xpProgressInventory->SetMaskAmount(mask);
	}
}
