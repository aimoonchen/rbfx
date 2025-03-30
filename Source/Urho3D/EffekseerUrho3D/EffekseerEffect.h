#pragma once

// #include <Godot.hpp>
// #include <Resource.hpp>
#include <EASTL/string.h>
#include "../Resource/Resource.h"
#include "Effekseer.h"

namespace Urho3D {

class Context;

class URHO3D_API EffekseerEffect : public Resource
{
	URHO3D_OBJECT(EffekseerEffect, Resource);

public:
	static void _register_methods();

	EffekseerEffect(Context* context);
	~EffekseerEffect();

	static void RegisterObject(Context* context);
	bool BeginLoad(Deserializer& source) override;
	bool EndLoad() override;
	bool Save(Serializer& dest) const override { return true; }

	void _init();

	void load(ea::string path);

	void release();

	void resolve_dependencies();
	
	void setup();

	ea::string get_data_path() const { return m_data_path; }

	void set_data_path(ea::string path) { m_data_path = path; }

// 	Dictionary get_subresources() const { return m_subresources; }
// 
// 	void set_subresources(Dictionary subresources) { m_subresources = subresources; }
// 
// 	PoolByteArray get_data_bytes() const { return m_data_bytes; }
// 
// 	void set_data_bytes(PoolByteArray bytes) { m_data_bytes = bytes; }

	float get_scale() const { return m_scale; }

	void set_scale(float scale) { m_scale = scale; }

	Effekseer::EffectRef& get_native() { return m_native; }

private:
	void get_material_path(char16_t* path, size_t path_size);

	ea::string m_data_path;
	std::vector<char> m_data_bytes;
// 	PoolByteArray m_data_bytes;
// 	Dictionary m_subresources;
	float m_scale = 1.0f;
	Effekseer::EffectRef m_native;
};

}
