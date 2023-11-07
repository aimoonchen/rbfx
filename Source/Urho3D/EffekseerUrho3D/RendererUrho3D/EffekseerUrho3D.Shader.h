
#ifndef __EFFEKSEERRENDERER_LLGI_SHADER_H__
#define __EFFEKSEERRENDERER_LLGI_SHADER_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.ShaderBase.h"
#include "GraphicsDevice.h"

namespace EffekseerUrho3D
{

class Shader : public ::EffekseerRenderer::ShaderBase
{
private:
	Backend::GraphicsDeviceRef graphicsDevice_;
	Backend::ShaderRef shader_;
	Backend::ShaderRef shaderOverride_;

	Backend::VertexLayoutRef vertexLayout_;
    int32_t vertexSize_{ 0 };
	void* m_vertexConstantBuffer;
	void* m_pixelConstantBuffer;
	int32_t vertexConstantBufferSize = 0;
	int32_t pixelConstantBufferSize = 0;

	Shader(Backend::GraphicsDeviceRef graphicsDevice,
		   Backend::ShaderRef shader,
		   Backend::VertexLayoutRef vertexLayout);

public:
	~Shader() override;

	static Shader* Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
						  Effekseer::Backend::ShaderRef shader,
						  Effekseer::Backend::VertexLayoutRef vertexLayout,
						  const char* name);

public:
	void OverrideShader(::Effekseer::Backend::ShaderRef shader) override
	{
		shaderOverride_ = shader.DownCast<Backend::Shader>();
	}

public:
    Urho3D::ShaderVariation* GetVertexShader() const
	{
		if (shaderOverride_ != nullptr)
		{
			shaderOverride_->GetVertexShader();
		}
		return shader_->GetVertexShader();
	}
	Urho3D::ShaderVariation* GetPixelShader() const
	{
		if (shaderOverride_ != nullptr)
		{
			shaderOverride_->GetPixelShader();
		}
		return shader_->GetPixelShader();
	}

	const Backend::VertexLayoutRef& GetVertexLayouts()
	{
		return vertexLayout_;
	}
    int32_t GetVertexSize() const { return vertexSize_; }
    const Effekseer::Backend::UniformLayoutRef& GetUniformLayout() const;
	void SetVertexConstantBufferSize(int32_t size);
	void SetPixelConstantBufferSize(int32_t size);
	int32_t GetVertexConstantBufferSize()
	{
		return vertexConstantBufferSize;
	}
	int32_t GetPixelConstantBufferSize()
	{
		return pixelConstantBufferSize;
	}

	void* GetVertexConstantBuffer()
	{
		return m_vertexConstantBuffer;
	}
	void* GetPixelConstantBuffer()
	{
		return m_pixelConstantBuffer;
	}

	void SetConstantBuffer();
};

} // namespace EffekseerUrho3D

#endif // __EFFEKSEERRENDERER_LLGI_SHADER_H__
