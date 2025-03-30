#include "GraphicsDevice.h"
#include <Diligent/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Diligent/Graphics/GraphicsEngine/interface/DeviceContext.h>
// #include <LLGI.Shader.h>
// #include <LLGI.Texture.h>
#include "../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h"
#include "../RendererUrho3D/EffekseerUrho3D.RenderResources.h"
#include "../../Graphics/Graphics.h"
#include "../../Resource/ResourceCache.h"
#include "../../Graphics/Texture2D.h"
#include "../../RenderAPI/RenderDevice.h"


const unsigned EFFEKSEER_ELEMENT_TYPESIZES[] = {
    0, // VT_UNDEFINED = 0, ///< Undefined type
    1, // VT_INT8, ///< Signed 8-bit integer
    2, // VT_INT16, ///< Signed 16-bit integer
    sizeof(int), // VT_INT32, ///< Signed 32-bit integer
    1, // VT_UINT8, ///< Unsigned 8-bit integer
    2, // VT_UINT16, ///< Unsigned 16-bit integer
    sizeof(unsigned), // VT_UINT32, ///< Unsigned 32-bit integer
    2, // VT_FLOAT16, ///< Half-precision 16-bit floating point
    sizeof(float), // VT_FLOAT32, ///< Full-precision 32-bit floating point
    sizeof(double), // VT_FLOAT64, ///< Double-precision 64-bit floating point
    0, // VT_NUM_TYPES ///< Helper value storing total number of types in the enumeration
};
namespace EffekseerUrho3D
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

VertexBuffer::VertexBuffer(Urho3D::RenderDevice* device)
    : renderDevice_{ device }
{
}

VertexBuffer::~VertexBuffer()
{
}

bool VertexBuffer::Allocate(int32_t size, bool isDynamic, const void* initData, int32_t initSize)
{
	//buffer_ = LLGI::CreateSharedPtr(graphicsDevice_->GetGraphics()->CreateBuffer(LLGI::BufferUsageType::Vertex | LLGI::BufferUsageType::MapWrite, size));
    Diligent::BufferDesc VertBuffDesc;
    VertBuffDesc.Name = "Effekseer vertex buffer";
    VertBuffDesc.BindFlags = Diligent::BIND_VERTEX_BUFFER;
    VertBuffDesc.Usage = isDynamic ? Diligent::USAGE_DYNAMIC : Diligent::USAGE_DEFAULT;
    if (isDynamic) {
        VertBuffDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
    }
    VertBuffDesc.Size = size;
    Diligent::BufferData VBData;
    VBData.pData = initData;
    VBData.DataSize = initSize;
    renderDevice_->GetRenderDevice()->CreateBuffer(VertBuffDesc, initData ? &VBData : nullptr, &buffer_);

//     if (isDynamic_) {
//         blocks_.Allocate(size, 0);
//     }
//     if (isDynamic) {
//         std::vector<Diligent::StateTransitionDesc> Barriers;
//         Barriers.emplace_back(buffer_, Diligent::RESOURCE_STATE_UNKNOWN, Diligent::RESOURCE_STATE_VERTEX_BUFFER, Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE);
//         renderDevice_->GetImmediateContext()->TransitionResourceStates(static_cast<Diligent::Uint32>(Barriers.size()), Barriers.data());
//     }
    return buffer_ != nullptr;
}

void VertexBuffer::Deallocate()
{
	buffer_ = nullptr;
}

bool VertexBuffer::Init(int32_t size, bool isDynamic, const void* initData, int32_t initSize)
{
	size_ = size;
	isDynamic_ = isDynamic;

	return Allocate(size_, isDynamic_, initData, initSize);
}

void VertexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
// 	if (auto dst = static_cast<uint8_t*>(buffer_->Lock()))
// 	{
// 		memcpy(dst + offset, src, size);
// 		buffer_->Unlock();
// 	}
//  auto dirtied = blocks_.Allocate(size, offset);
#if defined(__EMSCRIPTEN__)
    renderDevice_->GetImmediateContext()->UpdateBuffer(buffer_, offset, size, src, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
#else
    // TOOD: map a buffer_ for VertexBufferRing writing
    if (isDynamic_) {
        void* dst = nullptr;
        renderDevice_->GetImmediateContext()->MapBuffer(buffer_, Diligent::MAP_WRITE, /*dirtied*/ (offset == 0) ? Diligent::MAP_FLAG_DISCARD : Diligent::MAP_FLAG_NO_OVERWRITE, dst);
        if (dst) {
            memcpy((uint8_t*)dst + offset, src, size);
            renderDevice_->GetImmediateContext()->UnmapBuffer(buffer_, Diligent::MAP_WRITE);
        }
    } else {
        renderDevice_->GetImmediateContext()->UpdateBuffer(buffer_, offset, size, src, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }
#endif
}

// void VertexBuffer::MakeAllDirtied()
// {
//     blocks_.Allocate(size_, 0);
// }

IndexBuffer::IndexBuffer(Urho3D::RenderDevice* device)
    : renderDevice_{ device }
{
}

IndexBuffer::~IndexBuffer()
{
}

bool IndexBuffer::Allocate(int32_t elementCount, int32_t stride, const void* initData, int32_t initSize)
{
	//buffer_ = LLGI::CreateSharedPtr(graphicsDevice_->GetGraphics()->CreateBuffer(LLGI::BufferUsageType::Index | LLGI::BufferUsageType::MapWrite, stride * elementCount));
    Diligent::BufferDesc IndBuffDesc;
    IndBuffDesc.Name = "Effekseer index buffer";
    IndBuffDesc.Usage = initData ? Diligent::USAGE_IMMUTABLE : Diligent::USAGE_DEFAULT;
    IndBuffDesc.BindFlags = Diligent::BIND_INDEX_BUFFER;
    IndBuffDesc.Size = stride * elementCount;
    Diligent::BufferData IBData;
    IBData.pData = initData;
    IBData.DataSize = initSize;
    renderDevice_->GetRenderDevice()->CreateBuffer(IndBuffDesc, initData ? &IBData : nullptr, &buffer_);
	elementCount_ = elementCount;
	strideType_ = (stride == 4) ? Effekseer::Backend::IndexBufferStrideType::Stride4 : Effekseer::Backend::IndexBufferStrideType::Stride2;

	return buffer_ != nullptr;
}

void IndexBuffer::Deallocate()
{
	buffer_ = nullptr;
}

bool IndexBuffer::Init(int32_t elementCount, int32_t stride, const void* initData, int32_t initSize)
{
	elementCount_ = elementCount;
	stride_ = stride;

	return Allocate(elementCount_, stride_, initData, initSize);
}

void IndexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
// 	if (auto dst = static_cast<uint8_t*>(buffer_->Lock()))
// 	{
// 		memcpy(dst + offset, src, size);
// 		buffer_->Unlock();
// 	}
    renderDevice_->GetImmediateContext()->UpdateBuffer(buffer_, offset, size, src, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

bool VertexLayout::Init(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount)
{
    ea::array<ea::pair<int32_t, Diligent::VALUE_TYPE>, 6> formatMap{};
    formatMap[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32_FLOAT)]          = { 1, Diligent::VALUE_TYPE::VT_FLOAT32 };
    formatMap[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT)]       = { 2, Diligent::VALUE_TYPE::VT_FLOAT32 };
    formatMap[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT)]    = { 3, Diligent::VALUE_TYPE::VT_FLOAT32 };
    formatMap[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT)] = { 4, Diligent::VALUE_TYPE::VT_FLOAT32 };
	formatMap[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM)]     = { 4, Diligent::VALUE_TYPE::VT_UINT8 };
    formatMap[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UINT)]      = { 1, Diligent::VALUE_TYPE::VT_UINT32 };

	elements_.resize(elementCount);
    effekseerElements_.resize(elementCount);
    uint32_t offset = 0;
    uint32_t stride = 0;
    for (int32_t i = 0; i < elementCount; i++) {
        const auto& e = elements[i];
        const auto& type = formatMap[static_cast<int32_t>(e.Format)];
        stride += EFFEKSEER_ELEMENT_TYPESIZES[type.second] * type.first;
        effekseerElements_[i] = elements[i];
    }
	for (int32_t i = 0; i < elementCount; i++) {
        const auto& e = effekseerElements_[i];
        const auto& type = formatMap[static_cast<int32_t>(e.Format)];
        elements_[i] = Diligent::LayoutElement("ATTRIB", i, 0, type.first, type.second, (e.Format == Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM), offset, stride, Diligent::INPUT_ELEMENT_FREQUENCY_PER_VERTEX, 0);
        offset += EFFEKSEER_ELEMENT_TYPESIZES[type.second] * type.first;
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
}

Shader ::~Shader()
{
	Effekseer::SafeRelease(graphicsDevice_);
}

static Diligent::RefCntAutoPtr<Diligent::IShader> create_shader(Urho3D::RenderDevice* device,
    Diligent::SHADER_TYPE shaderType,
    const char* name,
    const void* source,
    size_t sourceLength = 0)
{
    Diligent::ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;
    ShaderCI.Desc.ShaderType = shaderType;
    ShaderCI.EntryPoint = "main";
    ShaderCI.Desc.Name = name;
    ShaderCI.GLSLVersion = Diligent::ShaderVersion{ 3, 3 };
    if (sourceLength > 0) {
        ShaderCI.ByteCode = source;
        ShaderCI.ByteCodeSize = sourceLength;
    }
    else {
        ShaderCI.Source = (const char*)source;
    }
    Diligent::RefCntAutoPtr<Diligent::IShader> shader;
    device->GetRenderDevice()->CreateShader(ShaderCI, &shader);
    return shader;
}

bool Shader::Init(const void* vertexShaderData, int32_t vertexShaderDataSize, const void* pixelShaderData, int32_t pixelShaderDataSize, Effekseer::Backend::UniformLayoutRef& layout)
{
//     auto renderDevice = graphicsDevice_->GetRenderDevice();
//     vertexShader_ = create_shader(renderDevice, Diligent::SHADER_TYPE_VERTEX, "mem vs shader", vertexShaderData, vertexShaderDataSize);
//     pixelShader_ = create_shader(renderDevice, Diligent::SHADER_TYPE_PIXEL, "mem ps shader", pixelShaderData, pixelShaderDataSize);
    return vertexShader_ != nullptr && pixelShader_ != nullptr;
}

bool Shader::Init(const Effekseer::CustomVector<Effekseer::StringView<char>>& vsCodes, const Effekseer::CustomVector<Effekseer::StringView<char>>& psCodes, Effekseer::Backend::UniformLayoutRef& layout)
{
    uniformLayout_ = layout;
    return false;
}

bool Shader::Init(const char* vertexFilename, const char* pixelFilename, Effekseer::Backend::UniformLayoutRef& layout)
{
    uniformLayout_ = layout;
    auto renderDevice = graphicsDevice_->GetRenderDevice();

    vertexShader_ = renderDevice->GetSubsystem<Urho3D::Graphics>()->GetShader(Urho3D::VS, vertexFilename, "");
    pixelShader_ = renderDevice->GetSubsystem<Urho3D::Graphics>()->GetShader(Urho3D::PS, pixelFilename, "");

//     auto cache = renderDevice->GetSubsystem<Urho3D::ResourceCache>();
//     auto get_source_code = [cache](const char* filename, ea::string& sourceCode, ea::string& shaderName) {
//         auto pFile = cache->GetFile(filename);
//         unsigned dataSize = pFile->GetSize();
//         sourceCode.resize(dataSize + 1);
//         sourceCode[dataSize] = '\0';
//         if (pFile->Read(sourceCode.data(), dataSize) != dataSize)
//             return false;
//         ea::string_view strView = filename;
//         auto p0 = strView.find_last_of('/');
//         auto p1 = strView.find('.');
//         shaderName = strView.substr(p0 + 1, p1 - p0 - 1);
//         return true;
//     };
//     
//     Diligent::ShaderCreateInfo ShaderCI;
//     // Tell the system that the shader source code is in HLSL.
//     // For OpenGL, the engine will convert this into GLSL under the hood.
//     ShaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
//     // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
//     ShaderCI.Desc.UseCombinedTextureSamplers = true;
//     ShaderCI.GLSLVersion = Diligent::ShaderVersion{ 3, 3 };
//     ShaderCI.EntryPoint = "main";
//     ea::string sourceCode;
//     ea::string shaderName;
//     // Create a vertex shader
//     if (get_source_code(vertexFilename, sourceCode, shaderName))
//     {
//         ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_VERTEX;
//         
//         ShaderCI.Desc.Name = shaderName.data();
//         ShaderCI.Source = sourceCode.data();
//         renderDevice->GetRenderDevice()->CreateShader(ShaderCI, &vertexShader_);
//     }
//     // Create a pixel shader
//     if (get_source_code(pixelFilename, sourceCode, shaderName))
//     {
//         ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
//         ShaderCI.Desc.Name = shaderName.data();
//         ShaderCI.Source = sourceCode.data();
//         renderDevice->GetRenderDevice()->CreateShader(ShaderCI, &pixelShader_);
//     }
    return vertexShader_ != nullptr && pixelShader_ != nullptr;
}

// void Shader::SetShaderResourceBinding(Diligent::IShaderResourceBinding* srb)
// {
//     shaderResourceBinding_ = srb;
// }
Diligent::IShader* Shader::GetVertexShader() const
{
    return vertexShader_->GetHandle();
}
Diligent::IShader* Shader::GetPixelShader() const
{
    return pixelShader_->GetHandle();
}

const Effekseer::Backend::UniformLayoutRef& Shader::GetUniformLayout() const
{
    return uniformLayout_;
}

Diligent::RefCntAutoPtr<Diligent::IBuffer> Shader::create_uniform_buffer(const char* name, int32_t size)
{
    Diligent::BufferDesc CBDesc;
    CBDesc.Name = name;
    CBDesc.Size = size;
    CBDesc.Usage = Diligent::USAGE_DYNAMIC;
    CBDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
    CBDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;

    auto renderDevice = graphicsDevice_->GetRenderDevice();
    Diligent::RefCntAutoPtr<Diligent::IBuffer> pBuffer;
    renderDevice->GetRenderDevice()->CreateBuffer(CBDesc, nullptr, &pBuffer);
    return pBuffer;
}

void Shader::CreateVertexUniformBuffer(int32_t size)
{
    vertexUniformBuffer_ = create_uniform_buffer("VS constants CB", size);
}

void Shader::CreatePixelUniformBuffer(int32_t size)
{
    pixelUniformBuffer_ = create_uniform_buffer("PS constants CB", size);
}

GraphicsDevice::GraphicsDevice(Urho3D::RenderDevice* renderDevice)
	: renderDevice_(renderDevice)
{
	//ES_SAFE_ADDREF(graphics_);
}

GraphicsDevice::~GraphicsDevice()
{
	//ES_SAFE_RELEASE(graphics_);
}

Urho3D::RenderDevice* GraphicsDevice::GetRenderDevice()
{
	return renderDevice_;
}

Effekseer::Backend::VertexBufferRef GraphicsDevice::CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic)
{
	auto ret = Effekseer::MakeRefPtr<VertexBuffer>(renderDevice_);

    if (!ret->Init(size, isDynamic))
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
	auto ret = Effekseer::MakeRefPtr<IndexBuffer>(renderDevice_);

    int32_t strideSize = (stride == Effekseer::Backend::IndexBufferStrideType::Stride4) ? 4 : 2;
	if (!ret->Init(elementCount, strideSize/*, initialData, elementCount * strideSize*/))
	{
		return nullptr;
	}
    if (initialData != nullptr)
    {
        ret->UpdateData(initialData, elementCount * strideSize, 0);
    }
	return ret;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData)
{
	auto ret = Effekseer::MakeRefPtr<::EffekseerUrho3D::Texture>();
    
	if (!ret->Init(renderDevice_->GetContext(), param, initialData))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(uint64_t id, const std::function<void()>& onDisposed)
{
	auto ret = Effekseer::MakeRefPtr<::EffekseerUrho3D::Texture>();

	if (!ret->Init(id, onDisposed))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(Urho3D::Texture2D* texture)
{
	auto ret = Effekseer::MakeRefPtr<::EffekseerUrho3D::Texture>();

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

Effekseer::Backend::ShaderRef GraphicsDevice::CreateShaderFromCodes(const char* vsCode, const char* psCode, Effekseer::Backend::UniformLayoutRef uniformLayout)
{
	auto ret = Effekseer::MakeRefPtr<Shader>(this);

	if (!ret->Init(vsCode, 0, psCode, 0, uniformLayout))
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
} // namespace EffekseerUrho3D
