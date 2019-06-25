#include "LoopStateMenu.h"

#include "GameLoop.h"

#include "GameObject.h"
#include "ComponentButton.h"

#include "PlayerPrefs.h"

#define GRAVEYARD_SCENE "Level0-TheGraveyard"

LoopStateMenu::LoopStateMenu(GameLoop* GL) : LoopState(GL)
{
}


LoopStateMenu::~LoopStateMenu()
{
}

void LoopStateMenu::Update()
{
	if (((Button*)(gLoop->menuButtons[0]))->IsPressed()) //PlayButton
	{
		gLoop->currentLoopState = (LoopState*)gLoop->loadingState;
		gLoop->menu->SetActive(false);
		gLoop->loadingGO->SetActive(true);
		gLoop->sceneToLoad = GRAVEYARD_SCENE;
		PlayerPrefs::DeleteAll(true);
	}
	else if (gLoop->optionButton->IsPressed())
	{
		gLoop->options->SetActive(true);
		gLoop->EnableMenuButtons(false);
		gLoop->currentLoopState = (LoopState*)gLoop->optionsState;
	}
	else if (gLoop->controlsButton->IsPressed())
	{
		gLoop->controls->SetActive(true);
		gLoop->EnableMenuButtons(false);
		gLoop->currentLoopState = (LoopState*)gLoop->controlsState;
	}
	else if (gLoop->creditsButton->IsPressed())
	{
		gLoop->currentLoopState = (LoopState*)gLoop->creditsState;
	}
	else if (gLoop->exitButton->IsPressed())
	{
		gLoop->currentLoopState = (LoopState*)gLoop->quitState;
	}
}
