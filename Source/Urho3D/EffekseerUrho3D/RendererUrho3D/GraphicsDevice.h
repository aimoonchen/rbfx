
#ifndef __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__
#define __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__

#include <Effekseer.h>
// #include <LLGI.Buffer.h>
// #include <LLGI.CommandList.h>
// #include <LLGI.Graphics.h>
#include <assert.h>
#include <set>
#include "../../Graphics/GraphicsDefs.h"
#include "../LoaderUrho3D/EffekseerUrho3D.TextureLoader.h"

namespace Urho3D
{
class Graphics;
class ShaderVariation;
class VertexBuffer;
class IndexBuffer;
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
	ea::shared_ptr<Urho3D::VertexBuffer> buffer_;
    Urho3D::Context* context_ = nullptr;
	//int32_t size_ = 0;
	bool isDynamic_ = false;
    int32_t stride_ = 0;

public:
	VertexBuffer(Urho3D::Context* context);

	~VertexBuffer() override;

	bool Allocate(int32_t count, const ea::vector<Urho3D::VertexElement>& elements, bool isDynamic);

	void Deallocate();

	bool Init(int32_t count, const ea::vector<Urho3D::VertexElement>& elements, bool isDynamic);

	void UpdateData(const void* src, int32_t size, int32_t offset) override;

    Urho3D::VertexBuffer* GetBuffer()
	{
		return buffer_.get();
	}
};

class IndexBuffer
	: public Effekseer::Backend::IndexBuffer
{
private:
	ea::shared_ptr<Urho3D::IndexBuffer> buffer_;
    Urho3D::Context* context_ = nullptr;
	int32_t stride_ = 0;

public:
	IndexBuffer(Urho3D::Context* context);

	~IndexBuffer() override;

	bool Allocate(int32_t elementCount, int32_t stride);

	void Deallocate();

	bool Init(int32_t elementCount, int32_t stride);

	void UpdateData(const void* src, int32_t size, int32_t offset) override;

    Urho3D::IndexBuffer* GetBuffer()
	{
		return buffer_.get();
	}
};

class VertexLayout
	: public Effekseer::Backend::VertexLayout
{
public:
	class Element
	{
	public:
		//LLGI::VertexLayoutFormat Format;
        Urho3D::VertexElement Format;
// 		std::string Name;
// 		int32_t Semantic;
	};

private:
	Effekseer::CustomVector<Element> elements_;

public:
	VertexLayout() = default;
	~VertexLayout() = default;

	bool Init(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount);

	void MakeGenerated();

	const Effekseer::CustomVector<Element>& GetElements() const
	{
		return elements_;
	}
};

class Shader
	: public Effekseer::Backend::Shader
{
private:
	GraphicsDevice* graphicsDevice_ = nullptr;
	Urho3D::ShaderVariation* vertexShader_ = nullptr;
    Urho3D::ShaderVariation* pixelShader_ = nullptr;

    Effekseer::Backend::UniformLayoutRef uniformLayout_ = nullptr;

public:
	Shader(GraphicsDevice* graphicsDevice);
	~Shader() override;
	bool Init(const void* vertexShaderData, int32_t vertexShaderDataSize, const void* pixelShaderData, int32_t pixelShaderDataSize);
    bool Init(const Effekseer::CustomVector<Effekseer::StringView<char>>& vsCodes, const Effekseer::CustomVector<Effekseer::StringView<char>>& psCodes, Effekseer::Backend::UniformLayoutRef& layout);
    bool Init(const char* vertexFilename, const char* pixelFilename, Effekseer::Backend::UniformLayoutRef& layout);
    Urho3D::ShaderVariation* GetVertexShader() const
	{
		return vertexShader_;
	}
    Urho3D::ShaderVariation* GetPixelShader() const
	{
		return pixelShader_;
	}
    const Effekseer::Backend::UniformLayoutRef& GetUniformLayout() const;
};

class GraphicsDevice
	: public Effekseer::Backend::GraphicsDevice
{
private:
    Urho3D::Graphics* graphics_;

public:
	GraphicsDevice(Urho3D::Graphics* graphics);

	~GraphicsDevice() override;

	Urho3D::Graphics* GetGraphics();

	Effekseer::Backend::VertexBufferRef CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic) override;

	Effekseer::Backend::IndexBufferRef CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride) override;

	Effekseer::Backend::TextureRef CreateTexture(const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData) override;

	Effekseer::Backend::TextureRef CreateTexture(uint64_t id, const std::function<void()>& onDisposed);

	Effekseer::Backend::TextureRef CreateTexture(Urho3D::Texture2D* texture);

	Effekseer::Backend::VertexLayoutRef CreateVertexLayout(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount) override;

	Effekseer::Backend::ShaderRef CreateShaderFromBinary(const void* vsData, int32_t vsDataSize, const void* psData, int32_t psDataSize) override;

    Effekseer::Backend::ShaderRef CreateShaderFromCodes(const Effekseer::CustomVector<Effekseer::StringView<char>>& vsCodes, const Effekseer::CustomVector<Effekseer::StringView<char>>& psCodes, Effekseer::Backend::UniformLayoutRef layout) override;
    Effekseer::Backend::ShaderRef CreateShaderFromFile(const char* vsFilename, const char* psFilename, Effekseer::Backend::UniformLayoutRef uniformLayout);
};

} // namespace Backend

} // namespace EffekseerUrho3D

#endif