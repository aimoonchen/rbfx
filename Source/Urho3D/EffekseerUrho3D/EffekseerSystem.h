#pragma once

// #include <Godot.hpp>
// #include <World.hpp>
// #include <Camera.hpp>
// #include <Camera2D.hpp>
// #include <Node.hpp>
#include "Effekseer.h"
#include "EffekseerRendererLLGI/EffekseerRendererLLGI.Renderer.h"
#include "SoundUrho3D/EffekseerUrho3D.SoundPlayer.h"
#include "../Core/Object.h"
namespace EffekseerUrho3D
{
class Renderer;
}

namespace Urho3D {

class Camera;
class EffekseerEffect;

class URHO3D_API EffekseerSystem : public Object
{
	URHO3D_OBJECT(EffekseerSystem, Object)

public:
	static void _register_methods();

	static EffekseerSystem* get_instance() { return s_instance; }

	EffekseerSystem(Urho3D::Context* context);

	~EffekseerSystem();

	void _init();

	void _enter_tree();

	void _exit_tree();

	void _process(float delta);

	void _update_draw();

	void draw3D(Effekseer::Handle handle, const /*Transform*/Urho3D::Matrix4& camera_transform);

	//void draw2D(Effekseer::Handle handle, const Transform2D& camera_transform);

	void stop_all_effects();

	void set_paused_to_all_effects(bool paused);

	int get_total_instance_count() const;

	const Effekseer::ManagerRef& get_manager() { return m_manager; }
    // void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandleRenderUpdate(StringHash eventType, VariantMap& eventData);
    void Render();
    void SetCamera(Camera* camera);
	void SetIsDistortionEnabled(bool value);
private:
	static EffekseerSystem* s_instance;

	Effekseer::ManagerRef m_manager;
	//EffekseerUrho3D::RendererRef m_renderer;
	EffekseerRenderer::RendererRef m_renderer;
    Camera* main_camera_{ nullptr };
	Effekseer::Matrix44 proj_mat_;
    bool update_proj_{ false };
	bool is_distortion_enabled_{ false };
	::EffekseerRenderer::DistortingCallback* distorting_callback_{ nullptr };
	::Effekseer::RefPtr<::EffekseerRenderer::CommandList> command_list_{ nullptr };
};

}
