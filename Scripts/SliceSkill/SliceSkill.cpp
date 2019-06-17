#include "SliceSkill.h"

#include "Application.h"
#include "ModuleTime.h"

#include "imgui.h"
#include "JSON.h"

SliceSkill_API Script* CreateScript()
{
	SliceSkill* instance = new SliceSkill;
	return instance;
}

void SliceSkill::Start()
{
}

void SliceSkill::Update()
{
	timer += App->time->gameDeltaTime;
}

void SliceSkill::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::InputFloat("Speed", &speed);
	ImGui::InputFloat("Duration", &duration);
}

void SliceSkill::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddFloat("Speed", speed);
	json->AddFloat("Duration", duration);
}

void SliceSkill::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	speed = json->GetFloat("Speed");
	duration = json->GetFloat("Duration");
}

void SliceSkill::UseSkill()
{
	if (timer > duration)
	{

	}
	else
	{
		timer = 0.0f;
		//Enable(false);
	}
}
