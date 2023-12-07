
#include "EffekseerUrho3D.Shader.h"
#include "EffekseerUrho3D.RendererImplemented.h"

namespace EffekseerUrho3D
{
Shader::Shader(Backend::GraphicsDeviceRef graphicsDevice,
			   Backend::ShaderRef shader,
			   Backend::VertexLayoutRef vertexLayout, const char* name)
	: graphicsDevice_(graphicsDevice)
	, shader_(shader)
	, vertexLayout_(vertexLayout)
	, m_vertexConstantBuffer(nullptr)
	, m_pixelConstantBuffer(nullptr)
    , name_{ name }
{
    const auto& elements = vertexLayout_->GetElements();
    for (size_t i = 0; i < elements.size(); i++) {
        const auto& element = elements[i];
        vertexSize_ += EFFEKSEER_ELEMENT_TYPESIZES[element.ValueType] * element.NumComponents;
    }
}

Shader::~Shader()
{
	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
}

Shader* Shader::Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
					   Effekseer::Backend::ShaderRef shader,
					   Effekseer::Backend::VertexLayoutRef vertexLayout,
					   const char* name)
{
	assert(graphicsDevice != nullptr);
	assert(shader != nullptr);

	return new Shader(graphicsDevice.DownCast<Backend::GraphicsDevice>(), shader.DownCast<Backend::Shader>(), vertexLayout.DownCast<Backend::VertexLayout>(), name);
}

const Effekseer::Backend::UniformLayoutRef& Shader::GetUniformLayout() const
{
    return shader_->GetUniformLayout();
}

void Shader::SetVertexConstantBufferSize(int32_t size)
{
	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	m_vertexConstantBuffer = new uint8_t[size];
	vertexConstantBufferSize = size;
    shader_->CreateVertexUniformBuffer(size);
}

void Shader::SetPixelConstantBufferSize(int32_t size)
{
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
	m_pixelConstantBuffer = new uint8_t[size];
	pixelConstantBufferSize = size;
    shader_->CreatePixelUniformBuffer(size);
}

void Shader::SetConstantBuffer()
{
}

} // namespace EffekseerUrho3D
