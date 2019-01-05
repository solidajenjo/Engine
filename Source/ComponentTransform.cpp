#include "ComponentTransform.h"

#include "GameObject.h"

#include <assimp/scene.h> 
#include "imgui.h"
#include "imgui_internal.h"
#include "Math/MathFunc.h"
#include "Math/float4x4.h"
#include "JSON.h"


ComponentTransform::ComponentTransform(GameObject* gameobject, const float4x4 &transform) : Component(gameobject, ComponentType::Transform)
{
	AddTransform(transform);
}

ComponentTransform::ComponentTransform(const ComponentTransform & component) : Component(component)
{
	position = component.position;
	rotation = component.rotation;
	eulerRotation = component.eulerRotation;
	scale = component.scale;
}


ComponentTransform::~ComponentTransform()
{
}

Component * ComponentTransform::Clone() const
{
	return new ComponentTransform(*this);
}

void ComponentTransform::AddTransform(const float4x4 & transform)
{
	transform.Decompose(position, rotation, scale);
	RotationToEuler();
}

void ComponentTransform::DrawProperties()
{
	if (ImGui::CollapsingHeader("Local Transformation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (gameobject->isStatic)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		ImGui::DragFloat3("Position", (float*)&position, 0.1f, -1000.f, 1000.f);

		ImGui::DragFloat3("Rotation", (float*)&eulerRotation, 0.5f, -180, 180.f);

		rotation = rotation.FromEulerXYZ(math::DegToRad(eulerRotation.x),
			math::DegToRad(eulerRotation.y), math::DegToRad(eulerRotation.z));

		ImGui::DragFloat3("Scale", (float*)&scale, 0.1f, 0.01f, 100.f);
		ImGui::Separator();

		if (gameobject->isStatic)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
	}
}

void ComponentTransform::SetRotation(const Quat & rot)
{
	rotation = rot;
	RotationToEuler();
}

void ComponentTransform::RotationToEuler()
{
	eulerRotation = rotation.ToEulerXYZ();
	eulerRotation.x = math::RadToDeg(eulerRotation.x);
	eulerRotation.y = math::RadToDeg(eulerRotation.y);
	eulerRotation.z = math::RadToDeg(eulerRotation.z);
}
void ComponentTransform::SetPosition(const float3 & pos)
{
	position = pos;
}

void ComponentTransform::SetLocalToWorld(const float4x4 & myGlobal)
{
	float4x4 world = myGlobal;
	world.Decompose(position, rotation, scale);
	RotationToEuler();
}



void ComponentTransform::SetWorldToLocal(const float4x4 & newparentGlobalMatrix)
{
	float4x4 world = float4x4::FromTRS(position, rotation, scale);
	float4x4 local = newparentGlobalMatrix.Inverted() * world;
	local.Decompose(position, rotation, scale);
	RotationToEuler();
}

void ComponentTransform::Save(JSON_value * value) const
{
	Component::Save(value);
	value->AddFloat3("Position", position);
	value->AddQuat("Rotation", rotation);
	value->AddFloat3("Euler", eulerRotation);
	value->AddFloat3("Scale", scale);
}

void ComponentTransform::Load(const JSON_value & value)
{
	Component::Load(value);
	position = value.GetFloat3("Position");
	rotation = value.GetQuat("Rotation");
	eulerRotation = value.GetFloat3("Euler");
	scale = value.GetFloat3("Scale");
}
