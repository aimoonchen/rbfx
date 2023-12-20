#pragma once
#include <map>
#include "../Graphics/StaticModel.h"

namespace Urho3D
{
    class Scene;
    class ImageCube;
    class Texture2D;
    class TextureCube;
    class ReflectionProbe;
class ShaderProgram;
class ShaderVariation;

class DynamicValueController;
class SunController;

// Renders a screen-space grid of triangles.
// Because of performance reasons, and because sky color is smooth, sky color is computed in vertex shader.
// 32x32 is a reasonable size for the grid to have smooth enough colors.
class URHO3D_API ProceduralSky : public StaticModel
{
    URHO3D_OBJECT(ProceduralSky, StaticModel);

public:
    enum Month : int
    {
        January = 0,
        February,
        March,
        April,
        May,
        June,
        July,
        August,
        September,
        October,
        November,
        December
    };

    /// Construct.
    explicit ProceduralSky(Context* context);
    /// Destruct.
    ~ProceduralSky() override;
    /// Register object factory. StaticModel must be registered first.
    /// @nobind
    static void RegisterObject(Context* context);

    /// Process octree raycast. May be called from a worker thread.
    void ProcessRayQuery(const RayOctreeQuery& query, ea::vector<RayQueryResult>& results) override;
    void Init(uint32_t verticalCount, uint32_t horizontalCount, Month month, float time, uint32_t cubemapSize = 256, const Vector3& northDir = Vector3{ 1.0f, 0.0f, 0.0f });
    void SetTime(float hour);
    void SetTimeScale(float ts);
    void SetMonth(Month m);
    //void UpdateUniformTo(Material* mtl);
    // TODO: remove this
    Color GetSunLuminanceGamma();
    Vector3 GetSunLuminance();
    Color GetSkyLuminanceGamma();
    Vector3 GetSkyLuminance();
    Vector3 GetSunDirection();
    Vector4 GetExposition();
    
    void OnCubemapRendered(TextureCube* texture);
    void HandleBeginRendering(StringHash eventType, VariantMap& eventData);

protected:
    /// Recalculate the world-space bounding box.
    void OnWorldBoundingBoxUpdate() override;

private:
    void update_environment_texture();
    void update_uniform();
    Material* sky_{ nullptr };
    Material* sky_color_banding_fix_{ nullptr };
    std::unique_ptr<DynamicValueController> m_sunLuminanceXYZ;
    std::unique_ptr<DynamicValueController> m_skyLuminanceXYZ;
    std::unique_ptr<SunController> m_sun;

    bool m_preventBanding;
    float m_time;
    float m_timeScale;
    int64_t m_timeOffset;
    float m_turbidity;

    static constexpr uint16_t MaxRTCount{ 4 };
    uint16_t m_freeRenderIndex{ 0 };
    // dummy render texture, for trigger probe renderer
    SharedPtr<Texture2D> m_renderTexture;
    struct RenderTask {
        uint32_t                wait_frame_count{ 0 };
        TextureCube*            texture{ nullptr };
        ReflectionProbe*        probe{ nullptr };
        SharedPtr<ImageCube>    cube_image;
    };
    RenderTask m_renderTask[MaxRTCount];
    ea::deque<RenderTask*> m_readyReadTask;
    ea::unordered_map<TextureCube*, RenderTask*> m_lookupMap;

    SharedPtr<Scene> m_scene;
    ProceduralSky* m_internalSky{ nullptr };
    bool m_configDirty{ false };
    uint32_t m_verticalCount{ 32 };
    uint32_t m_horizontalCount{ 32 };
    uint32_t m_cubemapSize{ 256 };
};
}
