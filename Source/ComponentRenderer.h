#ifndef __ComponentRenderer_h__
#define __ComponentRenderer_h__

#include "Component.h"
#include "Math/float3.h"
#include "Math/float4.h"
#include <list>

#define MAXTEXTURES 4
struct Texture;
struct Shader;

enum class TextureType
{
	DIFFUSE=0,
	SPECULAR,
	OCCLUSION,
	EMISSIVE
};
struct Material
{
	std::string name = "Default";
	Shader* shader = nullptr;

	Texture* textures[MAXTEXTURES]{nullptr}; //TODO: default specular texture?

	float4 diffuse_color = float4::one;
	float3 specular_color = float3::one;
	float3 emissive_color = float3::one;
	
	float kAmbient = 0.3f;
	float kDiffuse = 0.2f;
	float kSpecular = 0.1f;
	float shininess = 8.f;

	void SetShader(Shader* s)
	{
		shader = s;
	}

	void SetTexture(Texture* t, TextureType type)
	{
		textures[(unsigned)type] = t;
	}

	std::list<Texture*> GetTextures() const
	{
		std::list<Texture*> mytextures;
		for (unsigned i = 0; i < MAXTEXTURES; i++)
		{
			if (textures[i] != nullptr)
			{
				mytextures.push_back(textures[i]);
			}
		}
		return mytextures;
	}
};

class ComponentRenderer :
	public Component
{
public:
	ComponentRenderer(GameObject* gameobject);
	ComponentRenderer(const ComponentRenderer& component);
	~ComponentRenderer();

	Component* Clone() override;
	void DeleteTexture();
	
	void SetTexture(const char * data, TextureType type);
	void SetShader(const char * shaderName);
	Texture * GetTexture(TextureType type) const;
	Shader* GetShader() const;
	void DrawProperties() override;
	void Save(JSON_value *value) const override;
	void Load(JSON_value *value) override;

	void LoadMaterial(const char * material);

	void SaveMaterial() const;

public:
	Material* material = nullptr;

private:
	std::string selected_texture[MAXTEXTURES]{ "None selected" };
	std::string selected_shader = "Default";
};

#endif //__ComponentRenderer_h__
