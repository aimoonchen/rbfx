#pragma once
#include "../Math/Frustum.h"
#include "../Scene/Component.h"
#include "../RenderAPI/PipelineState.h"

#include <vector>
namespace Urho3D
{
class Camera;
class Texture;
class VertexBuffer;
class IndexBuffer;
class VertexElement;
class URHO3D_API MeshLine : public Component
{
    URHO3D_OBJECT(MeshLine, Component);

public:
    /// Construct.
    explicit MeshLine(Context* context);
    /// Destruct.
    ~MeshLine() override;
    /// Register object factory.
    /// @nobind
    static void RegisterObject(Context* context);

    struct LineDesc
    {
        // vs uniform
        Matrix3x4 model_mat;
        float width{1.0f};
        bool attenuation{false};
        Color color{1.0f, 1.0f, 1.0f, 1.0f};
        Vector3 gradient[2]{{}, {}};
        // fs uniform
        Texture* tex{nullptr};
        Texture* alphaTex{nullptr};
        bool use_dash{false};
        bool useGradient{false};
        float dash_array{0.0f};
        float dash_offset{0.0f};
        float dash_ratio{0.5f};
        float visibility{1.0f};
        float alpha_test{0.0f};
        Vector2 repeat{1.0f, 1.0f};
        Vector2 alpha_fade{0.0f, 0.0f};
        bool depth{false};
        float depth_bias{0.0f};
        // for cache line
        uint32_t id;
        bool cache{false};
        bool visible{true};
    };

    bool HasContent() const { return !cache_line_map_.empty() || !vertex_data_.empty(); }
    void UpdateData();
    void BeginLines();
    void AppendLine(std::vector<Vector3>&& points);
    LineDesc* EndLines(const LineDesc& lineDesc);
    LineDesc* AddGrid(uint32_t row, uint32_t col, float size, float gap, float round, const LineDesc& lineDesc);
    //
    LineDesc* AppendLine(const Vector3& start, const Vector3& end, const LineDesc& lineDesc);
    LineDesc* AppendLine(const std::vector<Vector3>& points, const LineDesc& lineDesc);
    LineDesc* AppendLine(const std::vector<std::vector<Vector3>>& points, const LineDesc& lineDesc);
    void RemoveLine(MeshLine::LineDesc* line);
    void SetDepthBias(float bias) { depth_bias_ = bias; }
    void SetSlopeScaledDepthBias(float bias) { slope_scaled_depth_bias_ = bias; }
    //
    void SetView(Camera* camera);
    void Render();

private:
    struct MeshLineVertex
    {
        Vector3 position; // curr point, SEM_POSITION
        Vector3 prev; // prev point, SEM_NORMAL
        Vector4 next; // next point, SEM_TANGENT
        Vector4 param{0, 1, 0, 1.0}; // param, SEM_BINORMAL(side, width, counters, alpha_fade)
        Vector2 texcoord0; // texcoord, SEM_TEXCOORD
    };
    struct DrawCall
    {
        uint16_t start_index{0};
        uint16_t count{0};
        LineDesc line_desc;
    };
    /// Handle end of frame. Clear debug geometry.
    void HandleEndFrame(StringHash eventType, VariantMap& eventData);
    /// Initialize pipeline states. Is expected to be called only once.
    void InitializePipelineStates();
    bool pipelineStatesInitialized_{};
    /// View transform.
    Matrix3x4 view_;
    /// Projection transform.
    Matrix4 projection_;
    /// Projection transform in API-specific format.
    Matrix4 gpuProjection_;
    Matrix4 gpuProjectionBias_;
    /// View frustum.
    Frustum frustum_;
    /// Active camera.
    WeakPtr<Camera> camera_;
    float depth_bias_{-0.001f};
    float slope_scaled_depth_bias_{};
    //
    SharedPtr<PipelineState> pipelineState_[2];
    SharedPtr<PipelineState> pipelineStateBias_[2];
    SharedPtr<PipelineState> noDepthPipelineState_[2];
    ea::vector<DrawCall> draw_calls_;
    SharedPtr<VertexBuffer> vertex_buffer_;
    SharedPtr<IndexBuffer> index_buffer_;
    ea::vector<VertexElement> vertex_element_;
    ea::vector<MeshLineVertex> vertex_data_;
    ea::vector<uint16_t> index_data_;
    uint32_t cache_id_{0};
    struct LineCache
    {
        LineDesc line_desc;
        ea::vector<ea::vector<MeshLineVertex>> vertex_data;
        ea::vector<uint16_t> index_data;
    };
    ea::vector<ea::vector<MeshLineVertex>> cache_vertex_data_;
    DrawCall* current_dc_{nullptr};
    uint32_t cvd_index_{0};
    bool disable_cache_{false};
    ea::vector<uint16_t> cache_index_data_;
    ea::unordered_map<uint32_t, LineCache> cache_line_map_;
    //
    std::vector<std::vector<Vector3>> temp_vertex_data_;
};
} // namespace Urho3D
