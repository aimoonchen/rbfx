// #include <ProjectSettings.hpp>
// #include <ResourceLoader.hpp>
// #include <Viewport.hpp>
// #include <Camera.hpp>
// #include <Transform.hpp>
// #include <GDScript.hpp>
// #include <VisualServer.hpp>
#include "../Core/Context.h"
#include "../Core/CoreEvents.h"
#include "../Graphics/Camera.h"
#include "../Graphics/Graphics.h"
#include "../RenderAPI/RenderDevice.h"
#include "RendererUrho3D/EffekseerUrho3D.Renderer.h"
#include "LoaderUrho3D/EffekseerUrho3D.TextureLoader.h"
#include "LoaderUrho3D/EffekseerUrho3D.ModelLoader.h"
//#include "LoaderUrho3D/EffekseerUrho3D.MaterialLoader.h"
#include "LoaderUrho3D/EffekseerBGFX.MaterialLoader.h"
#include "LoaderUrho3D/EffekseerUrho3D.CurveLoader.h"
#include "LoaderUrho3D/EffekseerUrho3D.SoundLoader.h"
#include "SoundUrho3D/EffekseerUrho3D.SoundPlayer.h"
#include "Utils/EffekseerUrho3D.Utils.h"
#include "EffekseerSystem.h"
#include "EffekseerEffect.h"

namespace Urho3D {

EffekseerSystem* EffekseerSystem::s_instance = nullptr;

void EffekseerSystem::_register_methods()
{
// 	register_method("_init", &EffekseerSystem::_init);
// 	register_method("_enter_tree", &EffekseerSystem::_enter_tree);
// 	register_method("_exit_tree", &EffekseerSystem::_exit_tree);
// 	register_method("_process", &EffekseerSystem::_process);
// 	register_method("_update_draw", &EffekseerSystem::_update_draw);
// 	register_method("stop_all_effects", &EffekseerSystem::stop_all_effects);
// 	register_method("set_paused_to_all_effects", &EffekseerSystem::set_paused_to_all_effects);
// 	register_method("get_total_instance_count", &EffekseerSystem::get_total_instance_count);
}
::EffekseerRenderer::DistortingCallback*
CreateDistortingCallback(::EffekseerRenderer::RendererRef renderer,
                         Effekseer::RefPtr<::EffekseerRenderer::CommandList> commandList)
{
	return nullptr;// new DistortingCallbackGL();
}

EffekseerSystem::EffekseerSystem(Urho3D::Context* context)
	: Object(context)
{
	s_instance = this;

	int32_t instanceMaxCount = 8000;// 2000;
	int32_t squareMaxCount = 4000;// 
	int32_t drawMaxCount = 128;
// 	Ref<Script> soundScript;
// 
// 	auto settings = ProjectSettings::get_singleton();
// 
// 	if (settings->has_setting("effekseer/instance_max_count")) {
// 		instanceMaxCount = (int32_t)settings->get_setting("effekseer/instance_max_count");
// 	}
// 	if (settings->has_setting("effekseer/square_max_count")) {
// 		squareMaxCount = (int32_t)settings->get_setting("effekseer/square_max_count");
// 	}
// 	if (settings->has_setting("effekseer/draw_max_count")) {
// 		drawMaxCount = (int32_t)settings->get_setting("effekseer/draw_max_count");
// 	}
// 	if (settings->has_setting("effekseer/sound_script")) {
// 		soundScript = Ref<Script>(settings->get_setting("effekseer/sound_script"));
// 	} else {
// 		soundScript = ResourceLoader::get_singleton()->load("res://addons/effekseer/src/EffekseerSound.gd", "");
// 	}
// 	Ref<Reference> sound = EffekseerUrho3D::ScriptNew(soundScript);
	
//	SetUrho3DContext(context);

	m_manager = Effekseer::Manager::Create(instanceMaxCount);
    m_renderer = EffekseerUrho3D::Renderer::Create(GetSubsystem<RenderDevice>(), squareMaxCount);
    m_renderer->SetProjectionMatrix(Effekseer::Matrix44().Indentity());
    //m_renderer->SetCommandList(EffekseerRenderer::CreateCommandList(m_renderer->GetGraphicsDevice(), nullptr));
    //distorting_callback_ = CreateDistortingCallback(m_renderer, command_list_);

	m_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
#ifndef __EMSCRIPTEN__
	m_manager->LaunchWorkerThreads(2);
#endif
	m_manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
	m_manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
	m_manager->SetTrackRenderer(m_renderer->CreateTrackRenderer());
	m_manager->SetRingRenderer(m_renderer->CreateRingRenderer());
    m_manager->SetModelRenderer(m_renderer->CreateModelRenderer());
	//m_manager->SetSoundPlayer(Effekseer::MakeRefPtr<EffekseerUrho3D::SoundPlayer>(sound));

    m_manager->SetTextureLoader(Effekseer::MakeRefPtr<EffekseerUrho3D::TextureLoader>(context, m_renderer->GetGraphicsDevice()));
    m_manager->SetModelLoader(Effekseer::MakeRefPtr<EffekseerUrho3D::ModelLoader>(context));
    //	m_manager->SetMaterialLoader(Effekseer::MakeRefPtr<EffekseerUrho3D::MaterialLoader>());
    m_manager->SetCurveLoader(Effekseer::MakeRefPtr<EffekseerUrho3D::CurveLoader>());
    // m_manager->SetSoundLoader(Effekseer::MakeRefPtr<EffekseerUrho3D::SoundLoader>(sound));
    
	// SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(EffekseerSystem, HandleUpdate));
    SubscribeToEvent(E_RENDERUPDATE, URHO3D_HANDLER(EffekseerSystem, HandleRenderUpdate));
	
	//SetIsDistortionEnabled(true);
}

EffekseerSystem::~EffekseerSystem()
{
	s_instance = nullptr;
}

void EffekseerSystem::HandleRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    float timeStep = eventData[RenderUpdate::P_TIMESTEP].GetFloat();
    _process(timeStep);
}

void EffekseerSystem::Render()
{
	if (!main_camera_) {
		return;
	}
//	Effekseer::Matrix44 matrix = EffekseerUrho3D::ToEfkMatrix44(camera_transform /*.inverse()*/);

	m_renderer->SetCameraMatrix(EffekseerUrho3D::ToEfkMatrix44(main_camera_->GetView().ToMatrix4()));
	//static bool first_time = true;
    if (update_proj_ /*first_time*/) {
        update_proj_ = false;
		proj_mat_ = EffekseerUrho3D::ToEfkMatrix44(main_camera_->GetGPUProjection());
		//first_time = false;
	}
	m_renderer->SetProjectionMatrix(proj_mat_);
    m_renderer->BeginRendering();
    m_manager->Draw();
    m_renderer->EndRendering();
}

void EffekseerSystem::SetCamera(Camera* camera)
{
	main_camera_ = camera;
    update_proj_ = true;
    //     Matrix4 projection = camera->GetGPUProjection();
// //#ifdef URHO3D_OPENGL
//     // Add constant depth bias manually to the projection matrix due to glPolygonOffset() inconsistency
//     float constantBias = 2.0f * 0.0f/*graphics_->GetDepthConstantBias()*/;
//     projection.m22_ += projection.m32_ * constantBias;
//     projection.m23_ += projection.m33_ * constantBias;
// //#endif
// 	proj_mat_ = EffekseerUrho3D::ToEfkMatrix44(main_camera_->GetGPUProjection());
//     m_renderer->SetProjectionMatrix(proj_mat_);
}

void EffekseerSystem::SetIsDistortionEnabled(bool value)
{
	is_distortion_enabled_ = value;
    if (is_distortion_enabled_) {
		m_renderer->SetDistortingCallback(distorting_callback_);
    } else {
		m_renderer->SetDistortingCallback(nullptr);
    }
}

void EffekseerSystem::_init()
{
}

void EffekseerSystem::_enter_tree()
{
	//set_process_priority(100);
	//VisualServer::get_singleton()->connect("frame_pre_draw", this, "_update_draw");
}

void EffekseerSystem::_exit_tree()
{
	//VisualServer::get_singleton()->disconnect("frame_pre_draw", this, "_update_draw");
}

void EffekseerSystem::_process(float delta)
{
	// Stabilize in a variable frame environment
//  float deltaFrames = delta * 60.0f;//30.0f;// 
// 	int iterations = std::max(1, (int)roundf(deltaFrames));
// 	float advance = deltaFrames / iterations;
// 	for (int i = 0; i < iterations; i++) {
// 		m_manager->Update(advance);
// 	}
    m_manager->Update();
	m_renderer->SetTime(m_renderer->GetTime() + delta);
}

void EffekseerSystem::_update_draw()
{
	//m_renderer->ResetState();
}

void EffekseerSystem::draw3D(Effekseer::Handle handle, const Urho3D::Matrix4& camera_transform)
{
	Effekseer:: Matrix44 matrix = EffekseerUrho3D::ToEfkMatrix44(camera_transform/*.inverse()*/);
	m_renderer->SetCameraMatrix(matrix);

	m_renderer->BeginRendering();
	m_manager->DrawHandle(handle);
	m_renderer->EndRendering();
}

// void EffekseerSystem::draw2D(Effekseer::Handle handle, const Transform2D& camera_transform)
// {
// 	Effekseer:: Matrix44 matrix = EffekseerUrho3D::ToEfkMatrix44(camera_transform.inverse());
// 	matrix.Values[3][2] = -1.0f; // Z offset
// 	m_renderer->SetCameraMatrix(matrix);
// 
// 	m_renderer->BeginRendering();
// 	m_manager->DrawHandle(handle);
// 	m_renderer->EndRendering();
// }

void EffekseerSystem::stop_all_effects()
{
	m_manager->StopAllEffects();
}

void EffekseerSystem::set_paused_to_all_effects(bool paused)
{
	m_manager->SetPausedToAllEffects(paused);
}

int EffekseerSystem::get_total_instance_count() const
{
	return m_manager->GetTotalInstanceCount();
}

}
