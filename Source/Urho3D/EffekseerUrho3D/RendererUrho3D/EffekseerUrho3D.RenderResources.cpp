
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "../Utils/EffekseerUrho3D.Utils.h"
#include "EffekseerUrho3D.RenderResources.h"
#include "../../Core/Context.h"
#include "../../Graphics/Texture2D.h"
#include "../../RenderAPI/RenderAPIDefs.h"
#include "../../RenderAPI/RenderDevice.h"
#include "../EffekseerSystem.h"
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerUrho3D
{
Texture::~Texture()
{
    if (onDisposed_)
    {
        onDisposed_();
    }
}

bool Texture::Init(Urho3D::Context* context, const Effekseer::Backend::TextureParameter& param,
    const Effekseer::CustomVector<uint8_t>& initialData)
{
    int mw = std::max(param.Size[0], param.Size[1]);
    int count = 1;

    while (mw > 1)
    {
        mw = mw << 1;
        count++;
    }

    // 	LLGI::TextureInitializationParameter texParam;
    // 	texParam.Size = LLGI::Vec2I(param.Size[0], param.Size[1]);
    // 	texParam.MipMapCount = param.MipLevelCount < 1 ? count : param.MipLevelCount;
    //
    // 	// TODO : Fix it
    // 	texParam.MipMapCount = 1;
    auto MipMapCount = 1;
    auto format = Urho3D::TextureFormat::TEX_FORMAT_RGBA8_UNORM;

    if (param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_RGBA8_UNORM;
    }
    else if (param.Format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_BGRA8_UNORM;
    }
    else if (param.Format == Effekseer::Backend::TextureFormatType::R8_UNORM)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_R8_UNORM;
    }
    else if (param.Format == Effekseer::Backend::TextureFormatType::R16G16_FLOAT)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_RG16_FLOAT;
    }
    else if (param.Format == Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_RGBA16_FLOAT;
    }
    else if (param.Format == Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_RGBA32_FLOAT;
    }
    else if (param.Format == Effekseer::Backend::TextureFormatType::BC1)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_BC1_UNORM;
    }
    else if (param.Format == Effekseer::Backend::TextureFormatType::BC2)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_BC2_UNORM;
    }
    else if (param.Format == Effekseer::Backend::TextureFormatType::BC3)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_BC3_UNORM;
    }
    else if (param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_RGBA8_UNORM_SRGB;
    }
    else if (param.Format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM_SRGB)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_BGRA8_UNORM_SRGB;
    }
    else if (param.Format == Effekseer::Backend::TextureFormatType::BC1_SRGB)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_BC1_UNORM_SRGB;
    }
    else if (param.Format == Effekseer::Backend::TextureFormatType::BC2_SRGB)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_BC2_UNORM_SRGB;
    }
    else if (param.Format == Effekseer::Backend::TextureFormatType::BC3_SRGB)
    {
        format = Urho3D::TextureFormat::TEX_FORMAT_BC3_UNORM_SRGB;
    }
    else
    {
        // not supported
        Effekseer::Log(Effekseer::LogType::Error,
            "The format is not supported.(" + std::to_string(static_cast<int>(param.Format)) + ")");
        return false;
    }

    // 	auto texture = graphicsDevice_->GetGraphics()->CreateTexture(texParam);
    // 	auto buf = texture->Lock();
    //
    // 	if (initialData.size() > 0)
    // 	{
    // 		memcpy(buf, initialData.data(), initialData.size());
    // 	}
    //
    // 	texture->Unlock();
    //
    // 	texture_ = LLGI::CreateSharedPtr(texture);
    auto w = param.Size[0];
    auto h = param.Size[1];
    texture_ = ea::make_shared<Urho3D::Texture2D>(context);
    texture_->SetSize(w, h, format);
    texture_->SetNumLevels(MipMapCount);
    if (initialData.size() > 0)
    {
        texture_->SetData(0, 0, 0, w, h, initialData.data());
    }
    param_ = param;
    return true;
}

bool Texture::Init(uint64_t id, std::function<void()> onDisposed)
{
    // 	auto texture = graphicsDevice_->GetGraphics()->CreateTexture(id);
    // 	if (texture == nullptr)
    // 	{
    // 		return false;
    // 	}
    //
    // 	texture_ = LLGI::CreateSharedPtr(texture);
    // 	onDisposed_ = onDisposed;
    //
    // 	param_.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
    // 	param_.Dimension = 2;
    // 	param_.Size = {
    // 		texture->GetSizeAs2D().X,
    // 		texture->GetSizeAs2D().Y,
    // 		0};
    // 	param_.MipLevelCount = texture_->GetMipmapCount();
    // 	param_.SampleCount = texture_->GetSamplingCount();
    // 	param_.Usage = Effekseer::Backend::TextureUsageType::External;
    //	return true;
    return false;
}

bool Texture::Init(Urho3D::Texture2D* texture)
{
    // 	LLGI::SafeAddRef(texture);
    // 	texture_ = LLGI::CreateSharedPtr(texture);
    texture_.reset(texture);

    param_.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
    param_.Dimension = 2;
    param_.Size = {texture->GetWidth(), texture->GetHeight(), 0};
    param_.MipLevelCount = texture_->GetLevels();
    param_.SampleCount = texture_->GetMultiSample();
    param_.Usage = Effekseer::Backend::TextureUsageType::External;

    return true;
}

Model::Model(const Effekseer::CustomVector<Vertex>& vertecies, const Effekseer::CustomVector<Face>& faces)
	: Effekseer::Model(vertecies, faces)
{
	UploadToEngine();
}

Model::Model(const void* data, int32_t size)
	: Effekseer::Model(data, size)
{
	UploadToEngine();
}

Model::~Model()
{
}

void Model::UploadToEngine()
{
    return;
    const auto context = Urho3D::EffekseerSystem::get_instance()->GetContext();
    for (int32_t f = 0; f < GetFrameCount(); f++) {
        auto& modelFrame = models_[f];
        auto vb = Effekseer::MakeRefPtr<Backend::VertexBuffer>(context->GetSubsystem<Urho3D::RenderDevice>());
        vb->Init(GetVertexCount(f), false);
        if (vb != nullptr) {
            vb->UpdateData(modelFrame.vertexes.data(), modelFrame.vertexes.size() * sizeof(Effekseer::Model::Vertex), 0);
        }
        modelFrame.vertexBuffer = vb;
        auto ib = Effekseer::MakeRefPtr<Backend::IndexBuffer>(context->GetSubsystem<Urho3D::RenderDevice>());
        int32_t stride = 4;
        ib->Init(3 * GetFaceCount(f), stride);
        if (ib != nullptr) {
            ib->UpdateData(modelFrame.faces.data(), modelFrame.faces.size() * stride, 0);
        }
        modelFrame.indexBuffer = ib;
    }

    isBufferStoredOnGPU_ = true;

// 	using namespace Effekseer;
// 	using namespace Effekseer::SIMD;
// 	using namespace EffekseerGodot;
// 	using namespace godot;
// 
// 	const int32_t vertexCount = GetVertexCount();
// 	const int32_t faceCount = GetFaceCount();
// 	const int32_t indexCount = faceCount * 3;
// 
// 	Vec3f aabbMin{}, aabbMax{};
// 
// 	uint32_t format = RenderingServer::ARRAY_FORMAT_VERTEX |
// 		RenderingServer::ARRAY_FORMAT_NORMAL |
// 		RenderingServer::ARRAY_FORMAT_TANGENT |
// 		RenderingServer::ARRAY_FORMAT_COLOR |
// 		RenderingServer::ARRAY_FORMAT_TEX_UV;
// 
// 	PackedByteArray vertexData;
// 	PackedByteArray attributeData;
// 
// 	vertexData.resize(vertexCount * sizeof(GdLitVertex));
// 	attributeData.resize(vertexCount * sizeof(GdAttribute));
// 
// 	GdLitVertex* dstVertex = (GdLitVertex*)vertexData.ptrw();
// 	GdAttribute* dstAttribute = (GdAttribute*)attributeData.ptrw();
// 
// 	const Vertex* srcVertex = GetVertexes();
// 
// 	aabbMin = aabbMax = srcVertex[0].Position;
// 	for (int32_t i = 0; i < vertexCount; i++)
// 	{
// 		auto& v = *srcVertex++;
// 		*dstVertex++ = GdLitVertex{ v.Position, ToGdNormal(v.Normal), ToGdTangent(v.Tangent) };
// 		*dstAttribute++ = GdAttribute{ v.VColor, v.UV };
// 
// 		aabbMin = Vec3f::Min(aabbMin, v.Position);
// 		aabbMax = Vec3f::Max(aabbMax, v.Position);
// 	}
// 
// 	PackedByteArray indexData;
// 	indexData.resize(indexCount * sizeof(uint16_t));
// 	uint16_t* dstIndex = (uint16_t*)indexData.ptrw();
// 
// 	const Face* srcFaces = GetFaces();
// 	for (size_t i = 0; i < faceCount; i++)
// 	{
// 		auto& f = srcFaces[i];
// 		dstIndex[0] = (uint16_t)(f.Indexes[0]);
// 		dstIndex[1] = (uint16_t)(f.Indexes[1]);
// 		dstIndex[2] = (uint16_t)(f.Indexes[2]);
// 		dstIndex += 3;
// 	}
// 
// 	AABB aabb;
// 	Vec3f::Store(&aabb.position, aabbMin);
// 	Vec3f::Store(&aabb.size, aabbMax - aabbMin);
// 
// 	Dictionary surface;
// 	surface["primitive"] = RenderingServer::PRIMITIVE_TRIANGLES;
// 	surface["format"] = format;
// 	surface["vertex_data"] = vertexData;
// 	surface["attribute_data"] = attributeData;
// 	surface["vertex_count"] = (int)vertexCount;
// 	surface["index_data"] = indexData;
// 	surface["index_count"] = (int)indexCount;
// 	surface["aabb"] = aabb;
// 
// 	auto rs = RenderingServer::get_singleton();
// 	meshRid_ = rs->mesh_create();
// 	rs->mesh_add_surface(meshRid_, surface);
}

} // namespace EffekseerGodot
