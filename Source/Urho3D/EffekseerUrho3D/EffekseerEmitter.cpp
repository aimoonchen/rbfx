// #include <Viewport.hpp>
// #include <VisualServer.hpp>
// #include "GDLibrary.h"
#include "../Core/Context.h"
#include "../Scene/Node.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneEvents.h"
#include "../Graphics/Camera.h"
#include "../Math/Frustum.h"
#include "EffekseerSystem.h"
#include "EffekseerEmitter.h"
#include "Utils/EffekseerUrho3D.Utils.h"

namespace Urho3D {

void EffekseerEmitter::_register_methods()
{
// 	register_method("_init", &EffekseerEmitter::_init);
// 	register_method("_ready", &EffekseerEmitter::_ready);
// 	register_method("_process", &EffekseerEmitter::_process);
// 	register_method("_enter_tree", &EffekseerEmitter::_enter_tree);
// 	register_method("_exit_tree", &EffekseerEmitter::_exit_tree);
// 	register_method("_update_draw", &EffekseerEmitter::_update_draw);
// 	register_method("play", &EffekseerEmitter::play);
// 	register_method("stop", &EffekseerEmitter::stop);
// 	register_method("stop_root", &EffekseerEmitter::stop_root);
// 	register_method("is_playing", &EffekseerEmitter::is_playing);
// 	register_property<EffekseerEmitter, Ref<EffekseerEffect>>("effect", 
// 		&EffekseerEmitter::set_effect, &EffekseerEmitter::get_effect, nullptr);
// 	register_property<EffekseerEmitter, bool>("autoplay", 
// 		&EffekseerEmitter::set_autoplay, &EffekseerEmitter::is_autoplay, true);
// 	register_property<EffekseerEmitter, bool>("paused", 
// 		&EffekseerEmitter::set_paused, &EffekseerEmitter::is_paused, false);
// 	register_property<EffekseerEmitter, float>("speed", 
// 		&EffekseerEmitter::set_speed, &EffekseerEmitter::get_speed, 1.0f,
// 		GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT,
// 		GODOT_PROPERTY_HINT_RANGE, "0.0,10.0,0.01");
// 	register_property<EffekseerEmitter, Color>("color", 
// 		&EffekseerEmitter::set_color, &EffekseerEmitter::get_color, Color(1.0f, 1.0f, 1.0f, 1.0f));
}

EffekseerEmitter::EffekseerEmitter(Urho3D::Context* context)
	: Drawable(context, DRAWABLE_GEOMETRY)
{
}

EffekseerEmitter::~EffekseerEmitter()
{
    stop();
}

void EffekseerEmitter::RegisterObject(Context* context)
{
	context->RegisterFactory<EffekseerEmitter>(Category_Geometry);
}
// void EffekseerEmitter::OnNodeSet(Urho3D::Node* node)
// {
//     if (node)
//     {
//         Drawable::OnNodeSet(node);
// 
//         if (m_autoplay)
//         {
//             play();
//         }
//     }
//     else
//     {
//         stop();
// 
//         Drawable::OnNodeSet(node);
//     }
// }
void EffekseerEmitter::OnWorldBoundingBoxUpdate()
{
    worldBoundingBox_ = boundingBox_.Transformed(node_->GetWorldTransform());
}

void EffekseerEmitter::HandleScenePostUpdate(StringHash eventType, VariantMap& eventData)
{
    auto system = EffekseerSystem::get_instance();
    bool active = true;
    if (auto camera = system->GetCamera(); camera && m_docull) {
        const auto& frustum = camera->GetFrustum();
        active = (frustum.IsInside(GetWorldBoundingBox()) != OUTSIDE);
        set_paused(!active);
        set_visible(active);
    }

    if (active) {
        const auto& manager = system->get_manager();
        for (auto it = m_handles.begin(); it != m_handles.end();) {
            auto handle = *it;
            if (!manager->Exists(handle)) {
			    if (m_looping) {
                    *it = play(0, false);
                    handle = *it;
                } else {
                    it = m_handles.erase(it);
                    continue;
                }
            }
            manager->SetMatrix(handle, EffekseerUrho3D::ToEfkMatrix43(node_->GetWorldTransform() /*get_global_transform()*/));
            ++it;
        }
    }
}

void EffekseerEmitter::_init()
{
}

void EffekseerEmitter::_ready()
{
	if (m_autoplay) {
		play();
	}
}

void EffekseerEmitter::_enter_tree()
{
	//VisualServer::get_singleton()->connect("frame_pre_draw", this, "_update_draw");
}

void EffekseerEmitter::_exit_tree()
{
	//VisualServer::get_singleton()->disconnect("frame_pre_draw", this, "_update_draw");
}

void EffekseerEmitter::_process(float delta)
{
// 	auto system = EffekseerSystem::get_instance();
// 	auto manager = system->get_manager();
// 
// 	for (int i = 0; i < m_handles.size(); ) {
// 		auto handle = m_handles[i];
// 		if (!manager->Exists(handle)) {
// 			m_handles.remove(i);
// 			continue;
// 		}
// 		manager->SetMatrix(handle, EffekseerUrho3D::ToEfkMatrix43(Urho3D::Matrix4::IDENTITY/*get_global_transform()*/));
// 		i++;
// 	}
}

void EffekseerEmitter::_update_draw()
{
// 	if (!is_visible()) {
// 		return;
// 	}
// 
// 	auto viewport = get_viewport();
// 	if (viewport == nullptr) return;
// 
// 	auto camera = viewport->get_camera();
// 	if (camera == nullptr) return;
// 
// 	auto system = EffekseerSystem::get_instance();
// 	
// 	for (int i = 0; i < m_handles.size(); i++) {
// 		system->draw3D(m_handles[i], camera->get_camera_transform());
// 	}
}

Effekseer::Handle EffekseerEmitter::play(int32_t startFrame, bool mgr)
{
	auto system = EffekseerSystem::get_instance();
    const auto& manager = system->get_manager();

	if (m_effect/*.is_valid()*/) {
		Effekseer::Handle handle = manager->Play(m_effect->get_native(), Effekseer::Vector3D(0, 0, 0), startFrame);
		if (handle >= 0) {
			manager->SetMatrix(handle, EffekseerUrho3D::ToEfkMatrix43(node_->GetWorldTransform()/*get_global_transform()*/));
			manager->SetUserData(handle, this);

			if (m_paused) {
				manager->SetPaused(handle, true);
			}
			if (m_speed != 1.0f) {
				manager->SetSpeed(handle, m_speed);
			}
			if (m_color != Effekseer::Color(255, 255, 255, 255)) {
				manager->SetAllColor(handle, m_color);
			}

            if (mgr) {
                m_handles.push_back(handle);
			}
				
            return handle;
		}
	}
    return -1;
}

void EffekseerEmitter::stop()
{
	if (m_handles.empty()) {
		return;
	}
	
	auto system = EffekseerSystem::get_instance();
    const auto& manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->StopEffect(m_handles[i]);
	}
	
	m_handles.clear();
}

void EffekseerEmitter::stop_root()
{
	auto system = EffekseerSystem::get_instance();
    const auto& manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->StopRoot(m_handles[i]);
	}
}

bool EffekseerEmitter::is_playing()
{
	return !m_handles.empty();
}

void EffekseerEmitter::set_paused(bool paused)
{
	m_paused = paused;

	auto system = EffekseerSystem::get_instance();
    const auto& manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetPaused(m_handles[i], paused);
	}
}

bool EffekseerEmitter::is_paused() const
{
	return m_paused;
}

void EffekseerEmitter::set_speed(float speed)
{
	m_speed = speed;

	auto system = EffekseerSystem::get_instance();
    const auto& manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetSpeed(m_handles[i], speed);
	}
}

float EffekseerEmitter::get_speed() const
{
	return m_speed;
}

void EffekseerEmitter::set_color(Color color)
{
	m_color = EffekseerUrho3D::ToEfkColor(color);

	auto system = EffekseerSystem::get_instance();
    const auto& manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetAllColor(m_handles[i], m_color);
	}
}

Color EffekseerEmitter::get_color() const
{
	return EffekseerUrho3D::ToGdColor(m_color);
}

void EffekseerEmitter::SetEffect(EffekseerEffect* effect)
{
	m_effect = effect;
	m_effect->setup();
    Scene* scene = GetScene();
    if (scene) {
        SubscribeToEvent(scene, E_SCENEPOSTUPDATE, URHO3D_HANDLER(EffekseerEmitter, HandleScenePostUpdate));
    }
}

void EffekseerEmitter::set_visible(bool visible)
{
    auto system = EffekseerSystem::get_instance();
    const auto& manager = system->get_manager();

    for (int i = 0; i < m_handles.size(); i++)
    {
        manager->SetShown(m_handles[i], visible);
    }
}

bool EffekseerEmitter::get_visible()
{
    auto system = EffekseerSystem::get_instance();
    const auto& manager = system->get_manager();

    for (int i = 0; i < m_handles.size(); i++)
    {
        return manager->GetShown(m_handles[i]);
    }
    return false;
}

void EffekseerEmitter::SetCullBoundingBox(const BoundingBox& box)
{
    m_docull = true;
    boundingBox_ = box;
}

}
