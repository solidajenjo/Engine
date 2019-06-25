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
		gLoop->App->input->GetKey(SDL_SCANCODE_P) == KEY_UP ||
		gLoop->App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_UP ||
		gLoop->hudBackToMenuButton->KeyUp())
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
		gLoop->currentLoopState = (LoopState*)gLoop->quitState;
		return;
	}
}

void LoopStatePaused::Exit()
{
		gLoop->App->time->gameTimeScale = 1.0F;
		if(gLoop->pauseMenuGO) gLoop->pauseMenuGO->SetActive(false);
}
