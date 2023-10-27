
#ifndef __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__
#define __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__

#include <Effekseer.h>
// #include <LLGI.Buffer.h>
// #include <LLGI.CommandList.h>
// #include <LLGI.Graphics.h>
#include <assert.h>
#include <set>

namespace Urho3D
{
class Graphics;
class VertexBuffer;
class IndexBuffer;
class Texture2D;
class ShaderVariation;
class VertexElement;
}

namespace EffekseerRendererLLGI
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

class DeviceObject
{
private:
public:
	virtual void OnLostDevice();

	virtual void OnResetDevice();
};

class VertexBuffer
	: public DeviceObject,
	  public Effekseer::Backend::VertexBuffer
{
private:
	ea::shared_ptr<Urho3D::VertexBuffer> buffer_;
	GraphicsDevice* graphicsDevice_ = nullptr;
	int32_t size_ = 0;
	bool isDynamic_ = false;

public:
	VertexBuffer(GraphicsDevice* graphicsDevice);

	~VertexBuffer() override;

	bool Allocate(int32_t size, ea::vector<Urho3D::VertexElement> elements, bool isDynamic);

	void Deallocate();

	void OnLostDevice() override;

	void OnResetDevice() override;

	bool Init(int32_t size, ea::vector<Urho3D::VertexElement> elements, bool isDynamic);

	void UpdateData(const void* src, int32_t size, int32_t offset) override;

    Urho3D::VertexBuffer* GetBuffer()
	{
		return buffer_.get();
	}
};

class IndexBuffer
	: public DeviceObject,
	  public Effekseer::Backend::IndexBuffer
{
private:
	ea::shared_ptr<Urho3D::IndexBuffer> buffer_;
	GraphicsDevice* graphicsDevice_ = nullptr;
	int32_t stride_ = 0;

public:
	IndexBuffer(GraphicsDevice* graphicsDevice);

	~IndexBuffer() override;

	bool Allocate(int32_t elementCount, int32_t stride);

	void Deallocate();

	void OnLostDevice() override;

	void OnResetDevice() override;

	bool Init(int32_t elementCount, int32_t stride);

	void UpdateData(const void* src, int32_t size, int32_t offset) override;

    Urho3D::IndexBuffer* GetBuffer()
	{
		return buffer_.get();
	}
};

class Texture
	: public DeviceObject,
	  public Effekseer::Backend::Texture
{
	ea::shared_ptr<Urho3D::Texture2D> texture_;
	GraphicsDevice* graphicsDevice_ = nullptr;
	std::function<void()> onDisposed_;

public:
	Texture(GraphicsDevice* graphicsDevice);
	~Texture() override;

	bool Init(const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData);

	//bool Init(uint64_t id, std::function<void()> onDisposed);

	bool Init(Urho3D::Texture2D* texture);

	ea::shared_ptr<Urho3D::Texture2D>& GetTexture()
	{
		return texture_;
	}
};

class Shader
	: public DeviceObject,
	  public Effekseer::Backend::Shader
{
private:
	GraphicsDevice* graphicsDevice_ = nullptr;
	Urho3D::ShaderVariation* vertexShader_ = nullptr;
	Urho3D::ShaderVariation* pixelShader_ = nullptr;

public:
	Shader(GraphicsDevice* graphicsDevice);
	~Shader() override;
	bool Init(const void* vertexShaderData, int32_t vertexShaderDataSize, const void* pixelShaderData, int32_t pixelShaderDataSize);

	Urho3D::ShaderVariation* GetVertexShader() const
	{
		return vertexShader_;
	}
	Urho3D::ShaderVariation* GetPixelShader() const
	{
		return pixelShader_;
	}
};

class GraphicsDevice
	: public Effekseer::Backend::GraphicsDevice
{
private:
	std::set<DeviceObject*> objects_;
    Urho3D::Graphics* graphics_;

public:
	GraphicsDevice(Urho3D::Graphics* graphics);

	~GraphicsDevice() override;

	void LostDevice();

	void ResetDevice();

    Urho3D::Graphics* GetGraphics();

	void Register(DeviceObject* deviceObject);

	void Unregister(DeviceObject* deviceObject);

	Effekseer::Backend::VertexBufferRef CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic) override;

	Effekseer::Backend::IndexBufferRef CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride) override;

	Effekseer::Backend::TextureRef CreateTexture(const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData) override;

	//Effekseer::Backend::TextureRef CreateTexture(uint64_t id, const std::function<void()>& onDisposed);

	Effekseer::Backend::TextureRef CreateTexture(Urho3D::Texture2D* texture);

	Effekseer::Backend::ShaderRef CreateShaderFromBinary(const void* vsData, int32_t vsDataSize, const void* psData, int32_t psDataSize) override;
};

} // namespace Backend

} // namespace EffekseerRendererLLGI

#endif
