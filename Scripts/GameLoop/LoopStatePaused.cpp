#include "LoopStatePaused.h"

#include "GameLoop.h"

#include "ModuleInput.h"
#include "ModuleTime.h".
#include "GameObject.h"
#include "ComponentButton.h"

LoopStatePaused::LoopStatePaused(GameLoop* GL) : LoopState(GL)
{
}


LoopStatePaused::~LoopStatePaused()
{
}

void LoopStatePaused::Enter()
{
	gLoop->App->time->gameTimeScale = 0.0F;
	if (gLoop->pauseMenuGO) gLoop->pauseMenuGO->SetActive(true);
}

void LoopStatePaused::Update()
{
	if ( (gLoop->pauseResume && gLoop->pauseResume->KeyUp()) || 
		gLoop->App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN ||
		gLoop->App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN ||
		gLoop->hudBackToMenuButton->IsPressed())
	{
		gLoop->currentLoopState = (LoopState*)gLoop->playingState;
		return;
	}

	if (gLoop->pauseOptions && gLoop->pauseOptions->KeyUp())
	{
		LOG("OPTIONS MENU PRESSED");
		return;
	}

	if (gLoop->pauseExit && gLoop->pauseExit->KeyUp())
	{
		gLoop->currentLoopState = (LoopState*)gLoop->loadingState;
		gLoop->playerMenuGO->SetActive(false);
		gLoop->hudGO->SetActive(false);
		gLoop->loadingGO->SetActive(true);
		gLoop->sceneToLoad = MENU_SCENE;
		return;
	}
}

void LoopStatePaused::Exit()
{
		gLoop->App->time->gameTimeScale = 1.0F;
		if(gLoop->pauseMenuGO) gLoop->pauseMenuGO->SetActive(false);
}
