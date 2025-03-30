
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
    const char* name_ = nullptr;
	Backend::VertexLayoutRef vertexLayout_;
    int32_t vertexSize_{ 0 };
    uint8_t* m_vertexConstantBuffer{ nullptr };
    uint8_t* m_pixelConstantBuffer{ nullptr };
	int32_t vertexConstantBufferSize = 0;
	int32_t pixelConstantBufferSize = 0;

	Shader(Backend::GraphicsDeviceRef graphicsDevice,
		   Backend::ShaderRef shader,
		   Backend::VertexLayoutRef vertexLayout, const char* name);

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
    Diligent::IShader* GetVertexShader() const
	{
		if (shaderOverride_ != nullptr)
		{
			shaderOverride_->GetVertexShader();
		}
		return shader_->GetVertexShader();
	}
    Diligent::IShader* GetPixelShader() const
	{
		if (shaderOverride_ != nullptr)
		{
			shaderOverride_->GetPixelShader();
		}
		return shader_->GetPixelShader();
	}
    Diligent::IBuffer* GetVertexUniformBuffer() const { return shader_->GetVertexUniformBuffer(); }
    Diligent::IBuffer* GetPixelUniformBuffer() const { return shader_->GetPixelUniformBuffer(); }
    //void SetShaderResourceBinding(Diligent::IShaderResourceBinding* srb) const { return shader_->SetShaderResourceBinding(srb); }
    //Diligent::IShaderResourceBinding* GetShaderResourceBinding() const { return shader_->GetShaderResourceBinding(); }
	const Backend::VertexLayoutRef& GetVertexLayouts() { return vertexLayout_; }
    int32_t GetVertexSize() const { return vertexSize_; }
    const Effekseer::Backend::UniformLayoutRef& GetUniformLayout() const;
	void SetVertexConstantBufferSize(int32_t size) override;
	void SetPixelConstantBufferSize(int32_t size) override;
	int32_t GetVertexConstantBufferSize() const { return vertexConstantBufferSize; }
	int32_t GetPixelConstantBufferSize() const { return pixelConstantBufferSize; }

	void* GetVertexConstantBuffer() override { return m_vertexConstantBuffer; }
	void* GetPixelConstantBuffer() override { return m_pixelConstantBuffer; }

	void SetConstantBuffer() override;
};

} // namespace EffekseerUrho3D

#endif // __EFFEKSEERRENDERER_LLGI_SHADER_H__
