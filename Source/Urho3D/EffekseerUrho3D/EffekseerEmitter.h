#pragma once

// #include <Godot.hpp>
// #include <Spatial.hpp>
#include "EffekseerEffect.h"
#include "../Graphics/Drawable.h"
namespace Urho3D {

class URHO3D_API EffekseerEmitter : public Drawable// : public Spatial
{
	//GODOT_CLASS(EffekseerEmitter, Spatial)
	URHO3D_OBJECT(EffekseerEmitter, Drawable)
public:
	static void _register_methods();

	EffekseerEmitter(Urho3D::Context* context);

	~EffekseerEmitter();
	static void RegisterObject(Context* context);
	//void Update(const FrameInfo& frame) override;

	void _init();

	void _ready();

	void _enter_tree();

	void _exit_tree();

	void _process(float delta);

	void _update_draw();

	Effekseer::Handle play(int32_t startFrame = 0, bool manager = true);

	void stop();

	void stop_root();

	bool is_playing();

	void set_paused(bool paused);

	bool is_paused() const;

	void set_speed(float speed);

	float get_speed() const;

	void set_color(Color color);

	Color get_color() const;

	void SetEffect(EffekseerEffect* effect);

	EffekseerEffect* get_effect() const { return m_effect; }

	void set_autoplay(bool autoplay) { m_autoplay = autoplay; }

	bool is_autoplay() const { return m_autoplay; }

	bool is_looping() { return m_looping; }

    void set_looping(bool value) { m_looping = value; }

protected:
    //void OnNodeSet(Urho3D::Node* node) override;
    /// Recalculate the world-space bounding box.
    void OnWorldBoundingBoxUpdate() override;

private:
    void HandleScenePostUpdate(StringHash eventType, VariantMap& eventData);
	//Ref<EffekseerEffect> m_effect;
    EffekseerEffect* m_effect{ nullptr };
	bool m_autoplay = true;
	//Array m_handles;
	std::vector<Effekseer::Handle> m_handles;
	bool m_paused = false;
	float m_speed = 1.0f;
    bool m_looping = false;
	Effekseer::Color m_color = {255, 255, 255, 255};
};

}
