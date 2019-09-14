#ifndef  __AOEBossScript_h__
#define  __AOEBossScript_h__

#include "BaseScript.h"
#include <vector>
#include "Math/float3.h"

class ComponentBoxTrigger;

#ifdef AOEBossScript_EXPORTS
#define AOEBossScript_API __declspec(dllexport)
#else
#define AOEBossScript_API __declspec(dllimport)
#endif

class AOEBossScript_API AOEBossScript : public Script
{
public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	inline virtual AOEBossScript* Clone() const
	{
		return new AOEBossScript(*this);
	}
private:

	GameObject* prepParticlesGO = nullptr;
	GameObject* beamParticlesGO = nullptr;
	GameObject* boxTriggerGO = nullptr;

	ComponentBoxTrigger* boxTrigger = nullptr;

	float duration = 3.0f;
	float timerFade = 2.0f;
	float timer = 0.0f;
};

extern "C" AOEBossScript_API Script* CreateScript();

#endif __AOEBossScript_h__