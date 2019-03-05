#include "ResourceMaterial.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

#include "ResourceTexture.h"

#include "JSON.h"
#include "GL/glew.h"

ResourceMaterial::ResourceMaterial(unsigned uid) : Resource(uid, TYPE::MATERIAL)
{
}

ResourceMaterial::ResourceMaterial(const ResourceMaterial& resource) : Resource(resource)
{
	name = resource.name;
	if (resource.shader != nullptr)
	{
		shader = App->program->GetProgram(resource.shader->file.c_str());
	}
	for (unsigned i = 0; i < MAXTEXTURES; ++i)
	{
		if (resource.textures[i] != nullptr)
		{
			textures[i] = (ResourceTexture*)App->resManager->Get(resource.textures[i]->GetExportedFile());
		}
	}

	diffuse_color = resource.diffuse_color;
	specular_color = resource.specular_color;
	emissive_color = resource.emissive_color;

	kAmbient = resource.kAmbient;
	kDiffuse = resource.kDiffuse;
	kSpecular = resource.kSpecular;
	shininess = resource.shininess;
}

ResourceMaterial::~ResourceMaterial()
{
	DeleteFromMemory();
}

void ResourceMaterial::DeleteFromMemory()
{
	Resource::DeleteFromMemory();
	if (shader != nullptr)
	{
		App->resManager->DeleteProgram(shader->file);
		shader = nullptr;
	}
	for (unsigned i = 0; i < MAXTEXTURES; i++)
	{
		if (textures[i] != nullptr)
		{
			App->resManager->DeleteResource(textures[i]->GetUID());
			textures[i] = nullptr;
		}
	}
}

bool ResourceMaterial::LoadInMemory()
{
	char* data = nullptr;
	// Load JSON
	if (App->fsystem->Load((MATERIALS + exportedFileName + JSONEXT).c_str(), &data) == 0)
		return false;

	JSON *json = new JSON(data);
	JSON_value *materialJSON = json->GetValue("material");

	name = exportedFileName;
	diffuse_color = materialJSON->GetColor4("diffuseColor");
	specular_color = materialJSON->GetColor3("specularColor");
	emissive_color = materialJSON->GetColor3("emissiveColor");

	kAmbient = materialJSON->GetFloat("kAmbient");
	kDiffuse = materialJSON->GetFloat("kDiffuse");
	kSpecular = materialJSON->GetFloat("kSpecular");
	shininess = materialJSON->GetFloat("shininess");

	const char *diffuseFile = materialJSON->GetString("diffuse");
	if (diffuseFile != nullptr)
	{
		textures[(unsigned)TextureType::DIFFUSE] = (ResourceTexture*)App->resManager->Get(diffuseFile);
	}
	const char *specularFile = materialJSON->GetString("specular");
	if (specularFile != nullptr)
	{
		textures[(unsigned)TextureType::SPECULAR] = (ResourceTexture*)App->resManager->Get(specularFile);
	}
	const char *occlusionFile = materialJSON->GetString("occlusion");
	if (occlusionFile != nullptr)
	{
		textures[(unsigned)TextureType::OCCLUSION] = (ResourceTexture*)App->resManager->Get(occlusionFile);
	}
	const char *emissiveFile = materialJSON->GetString("emissive");
	if (emissiveFile != nullptr)
	{
		textures[(unsigned)TextureType::EMISSIVE] = (ResourceTexture*)App->resManager->Get(emissiveFile);
	}

	const char* shaderName = materialJSON->GetString("shader");
	if (shaderName != nullptr)
	{
		shader = App->program->GetProgram(materialJSON->GetString("shader"));
	}

	RELEASE_ARRAY(data);
	RELEASE(json);
	++loaded;
	return true;
}

void ResourceMaterial::Save() const
{
	JSON *json = new JSON();
	JSON_value *materialJSON = json->CreateValue();

	if (textures[(unsigned)TextureType::DIFFUSE] != nullptr)
		materialJSON->AddFloat4("diffuseColor", diffuse_color);

	if (textures[(unsigned)TextureType::SPECULAR] != nullptr)
		materialJSON->AddFloat3("specularColor", specular_color);

	if (textures[(unsigned)TextureType::EMISSIVE] != nullptr)
		materialJSON->AddFloat3("emissiveColor", emissive_color);

	materialJSON->AddFloat("kAmbient", kAmbient);
	materialJSON->AddFloat("kDiffuse", kDiffuse);
	materialJSON->AddFloat("kSpecular", kSpecular);
	materialJSON->AddFloat("shininess", shininess);


	if (textures[(unsigned)TextureType::DIFFUSE] != nullptr)
	{
		materialJSON->AddString("diffuse", textures[(unsigned)TextureType::DIFFUSE]->GetFile());
	}
	if (textures[(unsigned)TextureType::SPECULAR] != nullptr)
	{
		materialJSON->AddString("specular", textures[(unsigned)TextureType::SPECULAR]->GetFile());
	}
	if (textures[(unsigned)TextureType::OCCLUSION] != nullptr)
	{
		materialJSON->AddString("occlusion", textures[(unsigned)TextureType::OCCLUSION]->GetFile());
	}
	if (textures[(unsigned)TextureType::EMISSIVE] != nullptr)
	{
		materialJSON->AddString("emissive", textures[(unsigned)TextureType::EMISSIVE]->GetFile());
	}

	if (shader != nullptr)
	{
		materialJSON->AddString("shader", shader->file.c_str());
	}
	
	json->AddValue("material", *materialJSON);

	App->fsystem->Save((MATERIALS + name + JSONEXT).c_str(), json->ToString().c_str(), json->Size());
	RELEASE(json);
}

ResourceTexture* ResourceMaterial::GetTexture(TextureType type) const
{
	return textures[(unsigned)type];
}

std::list<ResourceTexture*> ResourceMaterial::GetTextures() const
{
	std::list<ResourceTexture*> mytextures;
	for (unsigned i = 0; i < MAXTEXTURES; i++)
	{
		if (textures[i] != nullptr)
		{
			mytextures.push_back(textures[i]);
		}
	}
	return mytextures;
}

void ResourceMaterial::SetUniforms(unsigned shader) const
{
	for (unsigned int i = 0; i < MAXTEXTURES; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		char* textureType = nullptr;
		float* color = (float*)&float3::zero;
		switch ((TextureType)i)
		{
		case TextureType::DIFFUSE:
			textureType = "diffuse";
			color = (float*)&diffuse_color;
			break;

		case TextureType::SPECULAR:
			textureType = "specular";
			color = (float*)&specular_color;
			break;

		case TextureType::OCCLUSION:
			textureType = "occlusion";
			break;

		case TextureType::EMISSIVE:
			textureType = "emissive";
			color = (float*)&emissive_color;
			break;
		}

		char texture[32];
		sprintf(texture, "material.%s_texture", textureType);

		char uniform[32];
		sprintf(uniform, "material.%s_color", textureType);

		if (textures[i] != nullptr)
		{
			if (i == (unsigned)TextureType::DIFFUSE)
			{
				glUniform4fv(glGetUniformLocation(shader,
					uniform), 1, color);
			}
			else
			{
				glUniform3fv(glGetUniformLocation(shader,
					uniform), 1, color);
			}
			glBindTexture(GL_TEXTURE_2D, textures[i]->gpuID);

			glUniform1i(glGetUniformLocation(shader, texture), i);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			glUniform1i(glGetUniformLocation(shader, texture), i);
			float3 noColor = float3::zero; //Used as a fallback

			glUniform3fv(glGetUniformLocation(shader,
				uniform), 1, (GLfloat*)&noColor);
		}
		glDisable(GL_TEXTURE_2D);
	}

	glUniform1fv(glGetUniformLocation(shader,
		"material.k_ambient"), 1, (GLfloat*)&kAmbient);
	glUniform1fv(glGetUniformLocation(shader,
		"material.k_diffuse"), 1, (GLfloat*)&kDiffuse);
	glUniform1fv(glGetUniformLocation(shader,
		"material.k_specular"), 1, (GLfloat*)&kSpecular);
	glUniform1fv(glGetUniformLocation(shader,
		"material.shininess"), 1, (GLfloat*)&shininess);
}

void ResourceMaterial::Reset(const ResourceMaterial & material)
{
	name = material.name;

	if (shader != nullptr)
	{
		App->resManager->DeleteProgram(shader->file);
	}
	if (material.shader != nullptr)
	{
		shader = App->program->GetProgram(material.shader->file.c_str());
	}
	for (unsigned i = 0; i < MAXTEXTURES; ++i)
	{
		if (textures[i] != nullptr)
		{
			App->resManager->DeleteResource(textures[i]->GetUID());
		}
		if (material.textures[i] != nullptr)
		{
			textures[i] = (ResourceTexture*)App->resManager->Get(material.textures[i]->GetExportedFile());
		}
	}
	diffuse_color = material.diffuse_color;
	specular_color = material.specular_color;
	emissive_color = material.emissive_color;

	kAmbient = material.kAmbient;
	kDiffuse = material.kDiffuse;
	kSpecular = material.kSpecular;
	shininess = material.shininess;
}
