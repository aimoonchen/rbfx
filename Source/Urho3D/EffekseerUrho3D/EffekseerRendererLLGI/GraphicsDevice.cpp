#include "GraphicsDevice.h"
// #include <LLGI.Shader.h>
// #include <LLGI.Texture.h>
#include "../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h"
#include "../../Graphics/Graphics.h"
#include "../../Graphics/VertexBuffer.h"
#include "../../Graphics/IndexBuffer.h"
#include "../../Graphics/Texture2D.h"

namespace EffekseerRendererLLGI
{
namespace Backend
{

std::vector<uint8_t> Serialize(const std::vector<LLGI::DataStructure>& data)
{
	int32_t size = sizeof(int);
	for (const auto& d : data)
	{
		size += (sizeof(int) + d.Size);
	}

	std::vector<uint8_t> ret(size);

	int offset = 0;
	int32_t data_count = static_cast<int32_t>(data.size());
	memcpy(ret.data() + offset, &data_count, sizeof(int));
	offset += sizeof(int);

	for (const auto& d : data)
	{
		memcpy(ret.data() + offset, &d.Size, sizeof(int));
		offset += sizeof(int);

		memcpy(ret.data() + offset, d.Data, d.Size);
		size += d.Size;
	}

	return ret;
}

std::vector<LLGI::DataStructure> Deserialize(const void* data, int32_t size)
{
	std::vector<LLGI::DataStructure> ret;

	int offset = 0;
	uint8_t* p = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(data));
	int data_count = 0;
	memcpy(&data_count, p + offset, sizeof(int));
	offset += sizeof(int);

	for (int i = 0; i < data_count; i++)
	{
		int32_t data_size = 0;
		memcpy(&data_size, p + offset, sizeof(int));
		offset += sizeof(int);

		LLGI::DataStructure dataStructure;
		dataStructure.Data = p + offset;
		dataStructure.Size = data_size;
		offset += data_size;
		ret.emplace_back(dataStructure);
	}

	return ret;
}

void DeviceObject::OnLostDevice()
{
}

void DeviceObject::OnResetDevice()
{
}

VertexBuffer::VertexBuffer(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	graphicsDevice_->Register(this);
}

VertexBuffer::~VertexBuffer()
{
	graphicsDevice_->Unregister(this);
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool VertexBuffer::Allocate(int32_t count, const ea::vector<Urho3D::VertexElement>& elements, bool isDynamic)
{
	//buffer_ = LLGI::CreateSharedPtr(graphicsDevice_->GetGraphics()->CreateBuffer(LLGI::BufferUsageType::Vertex | LLGI::BufferUsageType::MapWrite, size));
    buffer_ = ea::make_shared<Urho3D::VertexBuffer>(graphicsDevice_->GetGraphics()->GetContext());
    buffer_->SetSize(count, elements, isDynamic);
    stride_ = buffer_->GetVertexSize();
    return true;
}

void VertexBuffer::Deallocate()
{
	buffer_.reset();
}

void VertexBuffer::OnLostDevice()
{
	//Deallocate();
}

void VertexBuffer::OnResetDevice()
{
	//Allocate(size_, isDynamic_);
}

bool VertexBuffer::Init(int32_t count, const ea::vector<Urho3D::VertexElement>& elements, bool isDynamic)
{
	//size_ = size;
	isDynamic_ = isDynamic;

	return Allocate(count, elements, isDynamic_);
}

void VertexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
// 	if (auto dst = static_cast<uint8_t*>(buffer_->Lock()))
// 	{
// 		memcpy(dst + offset, src, size);
// 		buffer_->Unlock();
// 	}
    buffer_->SetUnpackedData((const Urho3D::Vector4*)src, offset / stride_, size / stride_);
}

IndexBuffer::IndexBuffer(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	graphicsDevice_->Register(this);
}

IndexBuffer::~IndexBuffer()
{
	graphicsDevice_->Unregister(this);
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool IndexBuffer::Allocate(int32_t elementCount, int32_t stride)
{
	//buffer_ = LLGI::CreateSharedPtr(graphicsDevice_->GetGraphics()->CreateBuffer(LLGI::BufferUsageType::Index | LLGI::BufferUsageType::MapWrite, stride * elementCount));
    buffer_ = ea::make_shared<Urho3D::IndexBuffer>(graphicsDevice_->GetGraphics()->GetContext());
    buffer_->SetSize(elementCount, stride > 2/*, isDynamic*/);
	elementCount_ = elementCount;
	strideType_ = stride == 4 ? Effekseer::Backend::IndexBufferStrideType::Stride4 : Effekseer::Backend::IndexBufferStrideType::Stride2;

	return buffer_ != nullptr;
}

void IndexBuffer::Deallocate()
{
	buffer_.reset();
}

void IndexBuffer::OnLostDevice()
{
	//Deallocate();
}

void IndexBuffer::OnResetDevice()
{
	//Allocate(elementCount_, stride_);
}

bool IndexBuffer::Init(int32_t elementCount, int32_t stride)
{
	elementCount_ = elementCount;
	stride_ = stride;

	return Allocate(elementCount_, stride_);
}

void IndexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
// 	if (auto dst = static_cast<uint8_t*>(buffer_->Lock()))
// 	{
// 		memcpy(dst + offset, src, size);
// 		buffer_->Unlock();
// 	}
    buffer_->SetUnpackedData((const unsigned int*)src, offset / stride_, size / stride_);
}

Texture::Texture(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	graphicsDevice_->Register(this);
}

Texture::~Texture()
{
	if (onDisposed_)
	{
		onDisposed_();
	}
	graphicsDevice_->Unregister(this);
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool Texture::Init(const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData)
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
		Effekseer::Log(Effekseer::LogType::Error, "The format is not supported.(" + std::to_string(static_cast<int>(param.Format)) + ")");
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
    texture_ = ea::make_shared<Urho3D::Texture2D>(graphicsDevice_->GetGraphics()->GetContext());
    texture_->SetSize(w, h, format);
    texture_->SetNumLevels(MipMapCount);
    if (initialData.size() > 0) {
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
	param_.Size = {
		texture->GetWidth(),
		texture->GetHeight(),
		0};
	param_.MipLevelCount = texture_->GetLevels();
	param_.SampleCount = texture_->GetMultiSample();
	param_.Usage = Effekseer::Backend::TextureUsageType::External;

	return true;
}

bool VertexLayout::Init(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount)
{
    std::array<Urho3D::VertexElementType, 6> formatMap{};
	formatMap[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32_FLOAT)]          = Urho3D::VertexElementType::TYPE_FLOAT;
	formatMap[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT)]       = Urho3D::VertexElementType::TYPE_VECTOR2;
	formatMap[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT)]    = Urho3D::VertexElementType::TYPE_VECTOR3;
	formatMap[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT)] = Urho3D::VertexElementType::TYPE_VECTOR4;
	formatMap[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM)]     = Urho3D::VertexElementType::TYPE_UBYTE4_NORM;
	formatMap[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UINT)]      = Urho3D::VertexElementType::TYPE_UBYTE4;

    ea::unordered_map<ea::string, Urho3D::VertexElementSemantic> semanticMap{
        {"Input_Pos",                   Urho3D::SEM_POSITION},
        {"Input_Color",                 Urho3D::SEM_COLOR},
        {"Input_Normal",                Urho3D::SEM_NORMAL},
        {"Input_Binormal",              Urho3D::SEM_BINORMAL},
        {"Input_Tangent",               Urho3D::SEM_TANGENT},
        {"Input_UV",                    Urho3D::SEM_TEXCOORD},
        {"Input_UV1",                   Urho3D::SEM_TEXCOORD},
        {"Input_UV2",                   Urho3D::SEM_TEXCOORD},
        {"Input_Alpha_Dist_UV",         Urho3D::SEM_TEXCOORD},
        {"Input_BlendUV",               Urho3D::SEM_TEXCOORD},
        {"Input_Blend_Alpha_Dist_UV",   Urho3D::SEM_TEXCOORD},
        {"Input_FlipbookIndex",         Urho3D::SEM_TEXCOORD},
        {"Input_AlphaThreshold",        Urho3D::SEM_TEXCOORD},
    };
	elements_.resize(elementCount);

	for (int32_t i = 0; i < elementCount; i++)
	{
        const auto& e = elements[i];
        Urho3D::VertexElementType type = formatMap[static_cast<int32_t>(elements[i].Format)];
        Urho3D::VertexElementSemantic attrib = semanticMap[e.Name.c_str()];
        unsigned char index = (e.SemanticName == "TEXCOORD") ? e.SemanticIndex : 0;
//         if (e.SemanticName == "POSITION") {
//             attrib = Urho3D::SEM_POSITION;
//         } else if (e.SemanticName == "NORMAL") {
//             switch (e.SemanticIndex) {
//             case 0: attrib = Urho3D::SEM_COLOR; break;
//             case 1: attrib = Urho3D::SEM_NORMAL; break;
//             case 2: attrib = Urho3D::SEM_TANGENT; break;
//             case 3: attrib = Urho3D::SEM_BINORMAL; break;
//             default: assert(0); break;
//             }
//         } else if (e.SemanticName == "TEXCOORD") {
//             attrib = Urho3D::SEM_TEXCOORD;
//             index = e.SemanticIndex;
//         }
        elements_[i].Format = Urho3D::VertexElement(type, attrib, index, false);
// 		elements_[i].Name = elements[i].SemanticName;
// 		elements_[i].Semantic = elements[i].SemanticIndex;
	}

	return true;
}

void VertexLayout::MakeGenerated()
{
// 	for (size_t i = 0; i < elements_.size(); i++)
// 	{
// 		elements_[i].Name = "TEXCOORD";
// 		elements_[i].Semantic = static_cast<int32_t>(i);
// 	}
}

Shader::Shader(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	graphicsDevice_->Register(this);
}

Shader ::~Shader()
{
	graphicsDevice_->Unregister(this);
	Effekseer::SafeRelease(graphicsDevice_);
}

bool Shader::Init(const void* vertexShaderData, int32_t vertexShaderDataSize, const void* pixelShaderData, int32_t pixelShaderDataSize)
{
// 	auto vsd = Deserialize(vertexShaderData, vertexShaderDataSize);
// 	auto psd = Deserialize(pixelShaderData, pixelShaderDataSize);
// 
//     vertexShader_ = LLGI::CreateSharedPtr(graphicsDevice_->GetGraphics()->CreateShader(vsd.data(), static_cast<int32_t>(vsd.size())));
//     pixelShader_ = LLGI::CreateSharedPtr(graphicsDevice_->GetGraphics()->CreateShader(psd.data(), static_cast<int32_t>(psd.size())));
//     
// 	return vertexShader_ != nullptr && pixelShader_ != nullptr;
    assert(false);
    return false;
}

bool Shader::Init(const Effekseer::CustomVector<Effekseer::StringView<char>>& vsCodes, const Effekseer::CustomVector<Effekseer::StringView<char>>& psCodes, Effekseer::Backend::UniformLayoutRef& layout)
{
    uniformLayout_ = layout;

    assert(false);
    return false;
}

bool Shader::Init(const char* vertexFilename, const char* pixelFilename, Effekseer::Backend::UniformLayoutRef& layout)
{
    uniformLayout_ = layout;
    vertexShader_ = graphicsDevice_->GetGraphics()->GetShader(Urho3D::VS, vertexFilename, "");
    pixelShader_ = graphicsDevice_->GetGraphics()->GetShader(Urho3D::PS, pixelFilename, "");
    return vertexShader_ != nullptr && pixelShader_ != nullptr;
}

const Effekseer::Backend::UniformLayoutRef& Shader::GetUniformLayout() const
{
    return uniformLayout_;
}

GraphicsDevice::GraphicsDevice(Urho3D::Graphics* graphics)
	: graphics_(graphics)
{
	//ES_SAFE_ADDREF(graphics_);
}

GraphicsDevice::~GraphicsDevice()
{
	//ES_SAFE_RELEASE(graphics_);
}

void GraphicsDevice::LostDevice()
{
	for (auto& o : objects_)
	{
		o->OnLostDevice();
	}
}

void GraphicsDevice::ResetDevice()
{
	for (auto& o : objects_)
	{
		o->OnResetDevice();
	}
}

Urho3D::Graphics* GraphicsDevice::GetGraphics()
{
	return graphics_;
}

void GraphicsDevice::Register(DeviceObject* deviceObject)
{
	objects_.insert(deviceObject);
}

void GraphicsDevice::Unregister(DeviceObject* deviceObject)
{
	objects_.erase(deviceObject);
}

Effekseer::Backend::VertexBufferRef GraphicsDevice::CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic)
{
    static ea::vector<Urho3D::VertexElement> modelLayout{
        {Urho3D::TYPE_VECTOR3, Urho3D::SEM_POSITION, 0, false},
        {Urho3D::TYPE_VECTOR3, Urho3D::SEM_NORMAL, 0, false},
        {Urho3D::TYPE_VECTOR3, Urho3D::SEM_BINORMAL, 0, false},
        {Urho3D::TYPE_VECTOR3, Urho3D::SEM_TANGENT, 0, false},
        {Urho3D::TYPE_VECTOR2, Urho3D::SEM_TEXCOORD, 0, false},
        {Urho3D::TYPE_UBYTE4_NORM, Urho3D::SEM_COLOR, 0, false}
    };

    static ea::vector<Urho3D::VertexElement> spriteLayout{
        {Urho3D::TYPE_VECTOR3, Urho3D::SEM_POSITION, 0, false},
        {Urho3D::TYPE_UBYTE4_NORM, Urho3D::SEM_NORMAL, 0, false},
        {Urho3D::TYPE_UBYTE4_NORM, Urho3D::SEM_BINORMAL, 0, false},
        {Urho3D::TYPE_UBYTE4_NORM, Urho3D::SEM_TANGENT, 0, false},
        {Urho3D::TYPE_VECTOR2, Urho3D::SEM_TEXCOORD, 0, false},
        {Urho3D::TYPE_VECTOR2, Urho3D::SEM_TEXCOORD, 1, false},
        {Urho3D::TYPE_VECTOR4, Urho3D::SEM_TEXCOORD, 2, false},
        {Urho3D::TYPE_VECTOR2, Urho3D::SEM_TEXCOORD, 3, false},
        {Urho3D::TYPE_VECTOR4, Urho3D::SEM_TEXCOORD, 4, false},
        {Urho3D::TYPE_FLOAT, Urho3D::SEM_TEXCOORD, 5, false},
        {Urho3D::TYPE_FLOAT, Urho3D::SEM_TEXCOORD, 6, false}
    };

	auto ret = Effekseer::MakeRefPtr<VertexBuffer>(this);
    auto stride = isDynamic ? EffekseerRenderer::GetMaximumVertexSizeInAllTypes() : (int32_t)sizeof(Effekseer::Model::Vertex);
	if (!ret->Init(size / stride, isDynamic ? spriteLayout : modelLayout, isDynamic))
	{
		return nullptr;
	}

	if (initialData != nullptr)
	{
		ret->UpdateData(initialData, size, 0);
	}

	return ret;
}

Effekseer::Backend::IndexBufferRef GraphicsDevice::CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride)
{
	auto ret = Effekseer::MakeRefPtr<IndexBuffer>(this);

	if (!ret->Init(elementCount, stride == Effekseer::Backend::IndexBufferStrideType::Stride4 ? 4 : 2))
	{
		return nullptr;
	}

	if (initialData != nullptr)
	{
		ret->UpdateData(initialData, elementCount * (stride == Effekseer::Backend::IndexBufferStrideType::Stride4 ? 4 : 2), 0);
	}

	return ret;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(param, initialData))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(uint64_t id, const std::function<void()>& onDisposed)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(id, onDisposed))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(Urho3D::Texture2D* texture)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(texture))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::VertexLayoutRef GraphicsDevice::CreateVertexLayout(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount)
{
	auto ret = Effekseer::MakeRefPtr<VertexLayout>();

	if (!ret->Init(elements, elementCount))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::ShaderRef GraphicsDevice::CreateShaderFromBinary(const void* vsData, int32_t vsDataSize, const void* psData, int32_t psDataSize)
{
	auto ret = Effekseer::MakeRefPtr<Shader>(this);

	if (!ret->Init(vsData, vsDataSize, psData, psDataSize))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::ShaderRef GraphicsDevice::CreateShaderFromCodes(
    const Effekseer::CustomVector<Effekseer::StringView<char>>& vsCodes,
    const Effekseer::CustomVector<Effekseer::StringView<char>>& psCodes, Effekseer::Backend::UniformLayoutRef layout)
{
    auto ret = Effekseer::MakeRefPtr<Shader>(this);

    if (!ret->Init(vsCodes, psCodes, layout))
    {
        return nullptr;
    }

    return ret;
}

Effekseer::Backend::ShaderRef GraphicsDevice::CreateShaderFromFile(const char* vsFilename, const char* psFilename, Effekseer::Backend::UniformLayoutRef uniformLayout)
{
    auto ret = Effekseer::MakeRefPtr<Shader>(this);

    if (!ret->Init(vsFilename, psFilename, uniformLayout))
    {
        return nullptr;
    }

    return ret;
}

} // namespace Backend
} // namespace EffekseerRendererLLGI
