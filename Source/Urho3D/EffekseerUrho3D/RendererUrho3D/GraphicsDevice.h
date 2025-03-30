
#ifndef __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__
#define __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h"
#include <Effekseer.h>
// #include <LLGI.Buffer.h>
// #include <LLGI.CommandList.h>
// #include <LLGI.Graphics.h>
#include <assert.h>
#include <set>
#include <Diligent/Common/interface/RefCntAutoPtr.hpp>
#include "../../Graphics/GraphicsDefs.h"
#include "../LoaderUrho3D/EffekseerUrho3D.TextureLoader.h"

namespace Diligent
{
    struct IShader;
    struct IBuffer;
    struct IShaderResourceBinding;
}
extern const unsigned EFFEKSEER_ELEMENT_TYPESIZES[];
namespace Urho3D
{
class Graphics;
class ShaderVariation;
class RenderDevice;
class Texture2D;
class DrawCommandQueue;
class PipelineState;
class StaticPipelineStateCache;
} // namespace

namespace LLGI
{
struct DataStructure
{
    const void* Data;
    int32_t Size;
};
}
namespace EffekseerUrho3D
{
namespace Backend
{

class GraphicsDevice;
class VertexBuffer;
class IndexBuffer;
class UniformBuffer;
class Shader;
class VertexLayout;
class FrameBuffer;
class Texture;
class RenderPass;
class PipelineState;
class UniformLayout;

using GraphicsDeviceRef = Effekseer::RefPtr<GraphicsDevice>;
using VertexBufferRef = Effekseer::RefPtr<VertexBuffer>;
using IndexBufferRef = Effekseer::RefPtr<IndexBuffer>;
using UniformBufferRef = Effekseer::RefPtr<UniformBuffer>;
using ShaderRef = Effekseer::RefPtr<Shader>;
using VertexLayoutRef = Effekseer::RefPtr<VertexLayout>;
using FrameBufferRef = Effekseer::RefPtr<FrameBuffer>;
using TextureRef = Effekseer::RefPtr<Texture>;
using RenderPassRef = Effekseer::RefPtr<RenderPass>;
using PipelineStateRef = Effekseer::RefPtr<PipelineState>;
using UniformLayoutRef = Effekseer::RefPtr<UniformLayout>;

std::vector<uint8_t> Serialize(const std::vector<LLGI::DataStructure>& data);

std::vector<LLGI::DataStructure> Deserialize(const void* data, int32_t size);

class VertexBuffer
	: public Effekseer::Backend::VertexBuffer
{
private:
    //EffekseerRenderer::DirtiedBlock blocks_;
    Diligent::RefCntAutoPtr<Diligent::IBuffer> buffer_;
    Urho3D::RenderDevice* renderDevice_ = nullptr;
	int32_t size_ = 0;
	bool isDynamic_ = false;

public:
	VertexBuffer(Urho3D::RenderDevice* device);

	~VertexBuffer() override;

	bool Allocate(int32_t size, bool isDynamic, const void* initData = nullptr, int32_t initSize = 0);

	void Deallocate();

	bool Init(int32_t size, bool isDynamic, const void* initData = nullptr, int32_t initSize = 0);

	void UpdateData(const void* src, int32_t size, int32_t offset) override;

    //void MakeAllDirtied();

    Diligent::IBuffer* GetBuffer()
	{
		return buffer_;
	}
};

class IndexBuffer
	: public Effekseer::Backend::IndexBuffer
{
private:
    Diligent::RefCntAutoPtr<Diligent::IBuffer> buffer_;
    Urho3D::RenderDevice* renderDevice_ = nullptr;
	int32_t stride_ = 0;

public:
	IndexBuffer(Urho3D::RenderDevice* device);

	~IndexBuffer() override;

	bool Allocate(int32_t elementCount, int32_t stride, const void* initData = nullptr, int32_t initSize = 0);

	void Deallocate();

	bool Init(int32_t elementCount, int32_t stride, const void* initData = nullptr, int32_t initSize = 0);

	void UpdateData(const void* src, int32_t size, int32_t offset) override;

    Diligent::IBuffer* GetBuffer()
	{
		return buffer_;
	}
};

class VertexLayout
	: public Effekseer::Backend::VertexLayout
{
public:
// 	class Element
// 	{
// 	public:
// 		//LLGI::VertexLayoutFormat Format;
//         Diligent::LayoutElement Format;
// // 		std::string Name;
// // 		int32_t Semantic;
// 	};

private:
	Effekseer::CustomVector<Diligent::LayoutElement> elements_;
    Effekseer::CustomVector<Effekseer::Backend::VertexLayoutElement> effekseerElements_;
public:
	VertexLayout() = default;
	~VertexLayout() = default;

	bool Init(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount);

	void MakeGenerated();

	const Effekseer::CustomVector<Diligent::LayoutElement>& GetElements() const
	{
		return elements_;
	}
};

class Shader
	: public Effekseer::Backend::Shader
{
private:
	GraphicsDevice* graphicsDevice_ = nullptr;
//     Diligent::RefCntAutoPtr<Diligent::IShader> vertexShader_;
//     Diligent::RefCntAutoPtr<Diligent::IShader> pixelShader_;
    Urho3D::SharedPtr<Urho3D::ShaderVariation> vertexShader_;
    Urho3D::SharedPtr<Urho3D::ShaderVariation> pixelShader_;
    Diligent::RefCntAutoPtr<Diligent::IBuffer> vertexUniformBuffer_;
    Diligent::RefCntAutoPtr<Diligent::IBuffer> pixelUniformBuffer_;
    //Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> shaderResourceBinding_;
    Effekseer::Backend::UniformLayoutRef uniformLayout_ = nullptr;
    Diligent::RefCntAutoPtr<Diligent::IBuffer> create_uniform_buffer(const char* name, int32_t size);
public:
	Shader(GraphicsDevice* graphicsDevice);
	~Shader() override;
	bool Init(const void* vertexShaderData, int32_t vertexShaderDataSize, const void* pixelShaderData, int32_t pixelShaderDataSize, Effekseer::Backend::UniformLayoutRef& layout);
    bool Init(const Effekseer::CustomVector<Effekseer::StringView<char>>& vsCodes, const Effekseer::CustomVector<Effekseer::StringView<char>>& psCodes, Effekseer::Backend::UniformLayoutRef& layout);
    bool Init(const char* vertexFilename, const char* pixelFilename, Effekseer::Backend::UniformLayoutRef& layout);
    Diligent::IShader* GetVertexShader() const;
    Diligent::IShader* GetPixelShader() const;
    Diligent::IBuffer* GetVertexUniformBuffer() const { return vertexUniformBuffer_; }
    Diligent::IBuffer* GetPixelUniformBuffer() const { return pixelUniformBuffer_; }
    //void SetShaderResourceBinding(Diligent::IShaderResourceBinding* srb);
    //Diligent::IShaderResourceBinding* GetShaderResourceBinding() const { return shaderResourceBinding_; }
    void CreateVertexUniformBuffer(int32_t size);
    void CreatePixelUniformBuffer(int32_t size);
    const Effekseer::Backend::UniformLayoutRef& GetUniformLayout() const;
};

class GraphicsDevice
	: public Effekseer::Backend::GraphicsDevice
{
private:
    Urho3D::RenderDevice* renderDevice_;

public:
	GraphicsDevice(Urho3D::RenderDevice* renderDevice);

	~GraphicsDevice() override;

	Urho3D::RenderDevice* GetRenderDevice();

	Effekseer::Backend::VertexBufferRef CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic) override;

	Effekseer::Backend::IndexBufferRef CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride) override;

	Effekseer::Backend::TextureRef CreateTexture(const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData) override;

	Effekseer::Backend::TextureRef CreateTexture(uint64_t id, const std::function<void()>& onDisposed);

	Effekseer::Backend::TextureRef CreateTexture(Urho3D::Texture2D* texture);

	Effekseer::Backend::VertexLayoutRef CreateVertexLayout(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount) override;

    Effekseer::Backend::ShaderRef CreateShaderFromBinary(const void* vsData, int32_t vsDataSize, const void* psData, int32_t psDataSize) override { return nullptr; }
    Effekseer::Backend::ShaderRef CreateShaderFromCodes(const char* vsCode, const char* psCode, Effekseer::Backend::UniformLayoutRef uniformLayout);

    Effekseer::Backend::ShaderRef CreateShaderFromCodes(const Effekseer::CustomVector<Effekseer::StringView<char>>& vsCodes, const Effekseer::CustomVector<Effekseer::StringView<char>>& psCodes, Effekseer::Backend::UniformLayoutRef layout) override;
    Effekseer::Backend::ShaderRef CreateShaderFromFile(const char* vsFilename, const char* psFilename, Effekseer::Backend::UniformLayoutRef uniformLayout);
};

} // namespace Backend

} // namespace EffekseerUrho3D

#endif
