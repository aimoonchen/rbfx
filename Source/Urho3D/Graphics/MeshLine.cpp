#include "../Core/Context.h"
#include "../Core/CoreEvents.h"
#include "../Graphics/Camera.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/GraphicsUtils.h"
#include "../Graphics/IndexBuffer.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/VertexBuffer.h"
#include "../RenderAPI/RenderDevice.h"
#include "../RenderAPI/RenderScope.h"
#include "MeshLine.h"

#include "../DebugNew.h"

namespace Urho3D
{
MeshLine::MeshLine(Context* context)
    : Component(context)
{
    vertex_element_ = {
        Urho3D::VertexElement(Urho3D::TYPE_VECTOR3, Urho3D::SEM_POSITION),
        Urho3D::VertexElement(Urho3D::TYPE_VECTOR3, Urho3D::SEM_NORMAL),
        Urho3D::VertexElement(Urho3D::TYPE_VECTOR4, Urho3D::SEM_BINORMAL),
        Urho3D::VertexElement(Urho3D::TYPE_VECTOR4, Urho3D::SEM_TANGENT),
        Urho3D::VertexElement(Urho3D::TYPE_VECTOR2, Urho3D::SEM_TEXCOORD)
    };

    const unsigned numVertices = 512;
    vertex_data_.reserve(numVertices);
    vertex_buffer_ = MakeShared<VertexBuffer>(context_);
    vertex_buffer_->SetDebugName("MeshLine VertexBuffer");
    vertex_buffer_->SetShadowed(true);
    vertex_buffer_->SetSize(numVertices, vertex_element_);

    const unsigned numIndices = 1024;
    index_data_.reserve(numIndices);
    index_buffer_ = MakeShared<IndexBuffer>(context_);
    vertex_buffer_->SetDebugName("MeshLine IndexBuffer");
    index_buffer_->SetShadowed(true);
    index_buffer_->SetSize(numIndices, false);

    cache_vertex_data_.resize(1);

    SubscribeToEvent(E_ENDFRAME, URHO3D_HANDLER(MeshLine, HandleEndFrame));
}

MeshLine::~MeshLine() = default;

void MeshLine::RegisterObject(Context* context)
{
    context->AddFactoryReflection<MeshLine>(Category_Subsystem);
}

void MeshLine::HandleEndFrame(StringHash eventType, VariantMap& eventData)
{
    draw_calls_.clear();
    vertex_data_.clear();
    index_data_.clear();
}

void MeshLine::InitializePipelineStates()
{
    pipelineStatesInitialized_ = true;

    auto* renderer = GetSubsystem<Renderer>();
    auto* graphics = GetSubsystem<Graphics>();

    auto createPipelineState = [&](PrimitiveType primitiveType, BlendMode blendMode, CompareMode depthCompare,
                                   bool depthWriteEnabled, float bias)
    {
        GraphicsPipelineStateDesc desc;
        InitializeInputLayout(desc.inputLayout_, {vertex_buffer_});
        desc.colorWriteEnabled_ = true;
        desc.vertexShader_ = graphics->GetShader(VS, "v2/MeshLine", "");
        desc.pixelShader_ = graphics->GetShader(PS, "v2/MeshLine", "");
        desc.primitiveType_ = primitiveType;
        desc.depthCompareFunction_ = depthCompare;
        desc.depthWriteEnabled_ = depthWriteEnabled;
        desc.blendMode_ = blendMode;
        desc.constantDepthBias_ = bias;
        desc.debugName_ = "MeshLine PipelineState";

        return GetSubsystem<PipelineStateCache>()->GetGraphicsPipelineState(desc);
    };

    for (bool blend : {false, true})
    {
        const BlendMode blendMode = blend ? BLEND_ALPHA : BLEND_REPLACE;
        pipelineState_[blend] = createPipelineState(TRIANGLE_LIST, blendMode, CMP_LESSEQUAL, true, 0.0f);
        pipelineStateBias_[blend] = createPipelineState(TRIANGLE_LIST, blendMode, CMP_LESSEQUAL, true, depth_bias_);
        noDepthPipelineState_[blend] = createPipelineState(TRIANGLE_LIST, blendMode, CMP_ALWAYS, false, 0.0f);
    }
}

void MeshLine::SetView(Camera* camera)
{
    if (!camera)
        return;

    view_ = camera->GetView();
    projection_ = camera->GetProjection();
    gpuProjection_ = camera->GetGPUProjection();
    frustum_ = camera->GetFrustum();
    camera_ = camera;
}

void MeshLine::Render()
{
    static StringHash uModelMat("ModelMat");
    static StringHash uProjection("Projection");
    static StringHash uViewProjection("ViewProjection");
    static StringHash uResolution("Resolution");
    static StringHash uColor("Color");
    static StringHash uLineDesc0("LineDesc0");
    static StringHash uLineDesc1("LineDesc1");
    static StringHash uRepeat("Repeat");
    static StringHash uGradient1("Gradient1");
    static StringHash uGradient2("Gradient2");
    static StringHash baseMap("BaseMap");
    static StringHash alphaMap("AlphaMap");

    // test
//     MeshLine::LineDesc lineDesc;
//     lineDesc.width = 8.0f;
//     lineDesc.attenuation = false;
//     std::vector<Vector3> points;
//     points.reserve(100);
//     for (int j = 0; j <= 360; j += 1)
//     {
//         points.emplace_back(Urho3D::Cos((float)j) * 5.0f, Urho3D::Sin((float)j) * 5.0f, 0.0f);
//     }
//     AppendLine(points, lineDesc);
//     AppendLine({{-5.0f, -5.0f, 0.0f}, {5.0f, 5.0f, 0.0f}}, lineDesc);
//     AppendLine({{-5.0f, 5.0f, 0.0f}, {5.0f, -5.0f, 0.0f}}, lineDesc);
    // end test

    UpdateData();
    if (draw_calls_.empty())
    {
        return;
    }

    if (!pipelineStatesInitialized_)
    {
        InitializePipelineStates();
    }

    auto* renderDevice = GetSubsystem<RenderDevice>();
    auto* renderContext = renderDevice->GetRenderContext();
    const RenderScope renderScope(renderContext, "MeshLine::Render");

    DrawCommandQueue* drawQueue = renderDevice->GetDefaultQueue();
    drawQueue->Reset();

    const auto setDefaultConstants = [&]()
    {
        if (drawQueue->BeginShaderParameterGroup(SP_CAMERA))
        {
            drawQueue->AddShaderParameter(VSP_VIEW, view_);
            drawQueue->AddShaderParameter(VSP_VIEWINV, view_.Inverse());
            drawQueue->AddShaderParameter(VSP_VIEWPROJ, gpuProjection_ * view_);
            drawQueue->CommitShaderParameterGroup(SP_CAMERA);
        }

        //         if (drawQueue->BeginShaderParameterGroup(SP_MATERIAL, false))
        //         {
        //             drawQueue->AddShaderParameter(PSP_MATDIFFCOLOR, Color::WHITE.ToVector4());
        //             drawQueue->CommitShaderParameterGroup(SP_MATERIAL);
        //         }

        if (drawQueue->BeginShaderParameterGroup(SP_OBJECT))
        {
            drawQueue->AddShaderParameter(VSP_MODEL, Matrix3x4::IDENTITY);
            drawQueue->CommitShaderParameterGroup(SP_OBJECT);
        }
    };

    for (int i = 0; i < draw_calls_.size(); i++)
    {
        auto& draw_call = draw_calls_[i];
        auto& lineDesc = draw_call.line_desc;
        drawQueue->SetVertexBuffers({vertex_buffer_});
        drawQueue->SetIndexBuffer(index_buffer_);
        auto index = (lineDesc.color.a_ < 1.0f || !lineDesc.alpha_fade.Equals(Vector2::ZERO, 0.00001f)) ? 1 : 0;
        auto pipelineState = lineDesc.depth ? pipelineState_[index] : noDepthPipelineState_[index];
        // if (lineDesc.depth_bias > 0.0f) {
        //     pipelineState = pipelineStateBias_[index];
        // }
        drawQueue->SetPipelineState(pipelineState);
        if (lineDesc.tex)
        {
            drawQueue->AddShaderResource(baseMap, lineDesc.tex);
        }
        if (lineDesc.alphaTex)
        {
            drawQueue->AddShaderResource(alphaMap, lineDesc.alphaTex);
        }
        drawQueue->CommitShaderResources();
        if (drawQueue->BeginShaderParameterGroup(SP_CAMERA, true))
        {
            drawQueue->AddShaderParameter(uProjection, gpuProjection_);
            drawQueue->AddShaderParameter(uViewProjection, gpuProjection_ * view_);
            drawQueue->CommitShaderParameterGroup(SP_CAMERA);
        }
        if (drawQueue->BeginShaderParameterGroup(SP_OBJECT, true))
        {
            drawQueue->AddShaderParameter(uModelMat, lineDesc.model_mat);
            auto* graphics = GetSubsystem<Graphics>();
            Vector2 resolution{lineDesc.attenuation ? 1.0f : graphics->GetWidth(),
                lineDesc.attenuation ? 1.0f : graphics->GetHeight()};
            drawQueue->AddShaderParameter(
                uResolution, Vector4{resolution.x_, resolution.y_, lineDesc.width, lineDesc.attenuation ? 1.0f : 0.0f});
            drawQueue->AddShaderParameter(
                uColor, Vector4{lineDesc.color.r_, lineDesc.color.g_, lineDesc.color.b_, lineDesc.color.a_});
            drawQueue->CommitShaderParameterGroup(SP_OBJECT);
        }
        if (drawQueue->BeginShaderParameterGroup(SP_CUSTOM, true))
        {
            drawQueue->AddShaderParameter(uLineDesc0,
                Vector4{lineDesc.tex ? 1.0f : 0.0f, lineDesc.alphaTex ? 1.0f : 0.0f, lineDesc.visibility,
                    lineDesc.alpha_test});
            drawQueue->AddShaderParameter(uLineDesc1,
                Vector4{
                    lineDesc.use_dash ? 1.0f : 0.0f, lineDesc.dash_array, lineDesc.dash_offset, lineDesc.dash_ratio});
            drawQueue->AddShaderParameter(uRepeat,
                Vector4{lineDesc.repeat.x_, lineDesc.repeat.y_, lineDesc.alpha_fade.x_,
                    lineDesc.useGradient ? 1.0f : 0.0f});
            drawQueue->AddShaderParameter(uGradient1, lineDesc.gradient[0]);
            drawQueue->AddShaderParameter(uGradient1, lineDesc.gradient[1]);
            drawQueue->CommitShaderParameterGroup(SP_CUSTOM);
        }
        drawQueue->DrawIndexed(draw_call.start_index, draw_call.count);
    }
    renderContext->Execute(drawQueue);
}

void MeshLine::UpdateData()
{
    for (const auto& [id, lines] : cache_line_map_)
    {
        if (lines.line_desc.visible)
        {
            auto& dc = draw_calls_.emplace_back();
            dc.line_desc = lines.line_desc;
            dc.start_index = index_data_.size();
            for (const auto& vertex_data : lines.vertex_data)
            {
                auto triCount = (vertex_data.size() - 2);
                dc.count += triCount * 3;
                uint16_t start_index = vertex_data_.size();
                for (int i = 0; i < triCount; i += 2)
                {
                    // tri1
                    index_data_.emplace_back(start_index + i);
                    index_data_.emplace_back(start_index + i + 1);
                    index_data_.emplace_back(start_index + i + 2);
                    // tri2
                    index_data_.emplace_back(start_index + i + 3);
                    index_data_.emplace_back(start_index + i + 2);
                    index_data_.emplace_back(start_index + i + 1);
                }
                vertex_data_.insert(vertex_data_.end(), vertex_data.begin(), vertex_data.end());
            }
        }
    }
    if (vertex_data_.size() > vertex_buffer_->GetVertexCount())
    {
        vertex_buffer_->SetSize(vertex_data_.size(), vertex_element_);
    }
    vertex_buffer_->Update(vertex_data_.data());

    if (index_data_.size() > index_buffer_->GetIndexCount())
    {
        index_buffer_->SetSize(index_data_.size(), false);
    }
    index_buffer_->Update(index_data_.data());
}

void MeshLine::BeginLines()
{
    temp_vertex_data_.clear();
}

void MeshLine::AppendLine(std::vector<Vector3>&& points)
{
    temp_vertex_data_.emplace_back(ea::move(points));
}

MeshLine::LineDesc* MeshLine::EndLines(const LineDesc& lineDesc)
{
    return AppendLine(temp_vertex_data_, lineDesc);
}

MeshLine::LineDesc* MeshLine::AddGrid(
    uint32_t row, uint32_t col, float size, float gap, float round, const LineDesc& lineDesc)
{
    assert(lineDesc.cache);
    std::vector<std::vector<Vector3>> lines;
    float center_x = -0.5f * col * size;
    float center_z = -0.5f * row * size;
    if (gap > 0.0f || round > 0.0f)
    {
        center_x = center_x + 0.5f * size;
        center_z = center_z + 0.5f * size;
    }
    auto half_grid_size = 0.5f * (size - gap);
    auto bias = lineDesc.depth_bias;
    if (round > 0.0f)
    {
        lines.reserve(row * col);
        for (auto r = 0; r < row; r++)
        {
            auto cur_z = center_z + r * size;
            for (auto c = 0; c < col; c++)
            {
                auto cur_x = center_x + c * size;
                lines.emplace_back(
                    std::vector<Vector3>{{cur_x - half_grid_size, 0.0f + bias, cur_z - half_grid_size + round},
                        {cur_x - half_grid_size, 0.0f + bias, cur_z + half_grid_size - round},
                        {cur_x - half_grid_size + round, 0.0f + bias, cur_z + half_grid_size},
                        {cur_x + half_grid_size - round, 0.0f + bias, cur_z + half_grid_size},
                        {cur_x + half_grid_size, 0.0f + bias, cur_z + half_grid_size - round},
                        {cur_x + half_grid_size, 0.0f + bias, cur_z - half_grid_size + round},
                        {cur_x + half_grid_size - round, 0.0f + bias, cur_z - half_grid_size},
                        {cur_x - half_grid_size + round, 0.0f + bias, cur_z - half_grid_size},
                        {cur_x - half_grid_size, 0.0f + bias, cur_z - half_grid_size + round}});
            }
        }
    }
    else
    {
        if (gap > 0.0f)
        {
            lines.reserve(row * col);
            for (auto r = 0; r < row; r++)
            {
                auto cur_z = center_z + r * size;
                for (auto c = 0; c < col; c++)
                {
                    auto cur_x = center_x + c * size;
                    lines.emplace_back(
                        std::vector<Vector3>{{cur_x - half_grid_size, 0.0f + bias, cur_z - half_grid_size},
                            {cur_x - half_grid_size, 0.0f + bias, cur_z + half_grid_size},
                            {cur_x + half_grid_size, 0.0f + bias, cur_z + half_grid_size},
                            {cur_x + half_grid_size, 0.0f + bias, cur_z - half_grid_size},
                            {cur_x - half_grid_size, 0.0f + bias, cur_z - half_grid_size}});
                }
            }
        }
        else
        {
            lines.reserve(row + col + 2);
            auto half_height = 0.5f * size * row;
            auto half_width = 0.5f * size * col;
            for (auto c = 0; c <= col; c++)
            {
                lines.emplace_back(std::vector<Vector3>{
                    {center_x + c * size, 0.0f + bias, -half_height}, {center_x + c * size, 0.0f + bias, half_height}});
            }
            for (auto r = 0; r <= row; r++)
            {
                lines.emplace_back(std::vector<Vector3>{
                    {-half_width, 0.0f + bias, center_z + r * size}, {half_width, 0.0f + bias, center_z + r * size}});
            }
        }
    }
    return AppendLine(lines, lineDesc);
}

MeshLine::LineDesc* MeshLine::AppendLine(const Vector3& start, const Vector3& end, const LineDesc& lineDesc)
{
    return AppendLine(std::vector<Vector3>{start, end}, lineDesc);
}

MeshLine::LineDesc* MeshLine::AppendLine(const std::vector<std::vector<Vector3>>& points, const LineDesc& lineDesc)
{
    if (points.empty())
    {
        return nullptr;
    }
    if (!lineDesc.cache)
    {
        auto& dc = draw_calls_.emplace_back();
        dc.line_desc = lineDesc;
        dc.start_index = index_data_.size();
        current_dc_ = &dc;
    }
    else
    {
        cache_vertex_data_.resize(points.size());
        cache_index_data_.clear();
    }
    disable_cache_ = true;
    for (const auto& it : points)
    {
        AppendLine(it, lineDesc);
        if (lineDesc.cache)
        {
            cvd_index_++;
        }
    }
    disable_cache_ = false;
    cvd_index_ = 0;
    current_dc_ = nullptr;
    if (lineDesc.cache)
    {
        auto result = cache_line_map_.insert({++cache_id_, {lineDesc, cache_vertex_data_, cache_index_data_}});
        auto line = &result.first->second.line_desc;
        line->id = cache_id_;
        cache_vertex_data_.resize(1);
        return line;
    }
    return nullptr;
}

MeshLine::LineDesc* MeshLine::AppendLine(const std::vector<Vector3>& points, const LineDesc& lineDesc)
{
    if (points.empty())
    {
        return nullptr;
    }
    auto n = points.size();
    auto triCount = (2 * n - 2);
    if (lineDesc.cache)
    {
        cache_vertex_data_[cvd_index_].clear();
        cache_index_data_.clear();
    }
    else
    {
        if (!current_dc_)
        {
            auto& dc = draw_calls_.emplace_back();
            dc.line_desc = lineDesc;
            dc.start_index = index_data_.size();
            dc.count = triCount * 3;
        }
        else
        {
            current_dc_->count += triCount * 3;
        }
        uint16_t start_index = vertex_data_.size();
        for (int i = 0; i < triCount; i += 2)
        {
            // tri1
            index_data_.emplace_back(start_index + i);
            index_data_.emplace_back(start_index + i + 1);
            index_data_.emplace_back(start_index + i + 2);
            // tri2
            index_data_.emplace_back(start_index + i + 3);
            index_data_.emplace_back(start_index + i + 2);
            index_data_.emplace_back(start_index + i + 1);
        }
    }
    ea::vector<MeshLineVertex>& vertex_data = lineDesc.cache ? cache_vertex_data_[cvd_index_] : vertex_data_;
    ea::vector<float> D(n - 1);
    D[0] = points[0].DistanceToPoint(points[1]);
    for (int i = 0; i < n; ++i)
    {
        if (i > 0 && i < n - 1)
        {
            D[i] = D[i - 1] + points[i].DistanceToPoint(points[i + 1]);
        }
    }
    auto length = D.back();
    auto isclosed = points[0].Equals(points[n - 1], 0.0001f);
    auto start_point = isclosed ? points[n - 2] : (points[0] - (points[1] - points[0]));
    auto end_point = isclosed ? points[1] : (points[n - 1] + (points[n - 1] - points[n - 2]));
    float top_alpha = (lineDesc.alpha_fade.y_ > 0.5f) ? 0.0f : 1.0f;
    float bottom_alpha = (lineDesc.alpha_fade.y_ < -0.5f) ? 0.0f : 1.0f;

    auto& v0 = vertex_data.emplace_back();
    v0.prev = start_point;
    v0.position = points[0];
    v0.next = {points[1], 0.0f};
    v0.texcoord0 = {0.0f, 0.0f};
    v0.param = {1.0f, 1.0f, 0.0f, top_alpha};
    auto& v1 = vertex_data.emplace_back();
    v1 = vertex_data[vertex_data.size() - 2];
    v1.param.x_ = -1.0f;
    v1.param.w_ = bottom_alpha;
    v1.texcoord0 = {0.0f, 1.0f};
    for (int i = 1; i < n - 1; ++i)
    {
        const auto& point = points[i];
        auto& nv0 = vertex_data.emplace_back();
        nv0.prev = points[i - 1];
        nv0.position = point;
        nv0.next = {points[i + 1], 0.0f};
        nv0.texcoord0 = {D[i - 1] / length, 0.0f};
        nv0.param = {1.0f, 1.0f, (float)i / (float)(n - 1), top_alpha};
        auto& nv1 = vertex_data.emplace_back();
        nv1 = vertex_data[vertex_data.size() - 2];
        nv1.param.x_ = -1.0f;
        nv1.param.w_ = bottom_alpha;
        nv1.texcoord0 = {D[i - 1] / length, 1.0f};
    }
    auto& v2 = vertex_data.emplace_back();
    v2.prev = points[n - 2];
    v2.position = points[n - 1];
    v2.next = {end_point, 0.0f};
    v2.texcoord0 = {1.0f, 0.0f};
    v2.param = {1.0f, 1.0f, 1.0f, top_alpha};
    auto& v3 = vertex_data.emplace_back();
    v3 = vertex_data[vertex_data.size() - 2];
    v3.param.x_ = -1.0f;
    v3.param.w_ = bottom_alpha;
    v3.texcoord0 = {1.0f, 1.0f};

    if (lineDesc.cache && !disable_cache_)
    {
        auto result = cache_line_map_.insert({++cache_id_, {lineDesc, cache_vertex_data_, cache_index_data_}});
        auto line = &result.first->second.line_desc;
        line->id = cache_id_;
        return line;
    }
    return nullptr;
}

void MeshLine::RemoveLine(MeshLine::LineDesc* line)
{
    if (auto it = cache_line_map_.find(line->id); it != cache_line_map_.end())
    {
        cache_line_map_.erase(it);
    }
}

} // namespace Urho3D
