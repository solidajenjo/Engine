#ifndef __ComponentVolumetricLight_h__
#define __ComponentVolumetricLight_h__

#include "Component.h"
#include "Math/float4.h"
#include <vector>

class GameObject;
struct par_shapes_mesh_s;

class ComponentVolumetricLight : public Component
{
public:
	ComponentVolumetricLight(GameObject* gameobject);
	ComponentVolumetricLight(const ComponentVolumetricLight &copy);
	
	Component* Clone() const override;

	void Update() override;

	void DrawProperties() override;
	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;

	void Init();

private:

	par_shapes_mesh_s* circle1 = nullptr;
	par_shapes_mesh_s* circle2 = nullptr;

};

#endif // __ComponentVolumetricLight_h__
