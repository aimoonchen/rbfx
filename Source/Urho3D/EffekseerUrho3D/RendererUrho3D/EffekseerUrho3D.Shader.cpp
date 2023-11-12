
#include "EffekseerUrho3D.Shader.h"
#include "EffekseerUrho3D.RendererImplemented.h"

namespace EffekseerUrho3D
{
    static const unsigned ELEMENT_TYPESIZES[] = {
        0,                  //VT_UNDEFINED = 0, ///< Undefined type
        1,                  //VT_INT8, ///< Signed 8-bit integer
        2,                  //VT_INT16, ///< Signed 16-bit integer
        sizeof(int),        //VT_INT32, ///< Signed 32-bit integer
        1,                  //VT_UINT8, ///< Unsigned 8-bit integer
        2,                  //VT_UINT16, ///< Unsigned 16-bit integer
        sizeof(unsigned),   //VT_UINT32, ///< Unsigned 32-bit integer
        2,                  //VT_FLOAT16, ///< Half-precision 16-bit floating point
        sizeof(float),      //VT_FLOAT32, ///< Full-precision 32-bit floating point
        sizeof(double),     //VT_FLOAT64, ///< Double-precision 64-bit floating point
        0,                  //VT_NUM_TYPES ///< Helper value storing total number of types in the enumeration
    };
Shader::Shader(Backend::GraphicsDeviceRef graphicsDevice,
			   Backend::ShaderRef shader,
			   Backend::VertexLayoutRef vertexLayout)
	: graphicsDevice_(graphicsDevice)
	, shader_(shader)
	, vertexLayout_(vertexLayout)
	, m_vertexConstantBuffer(nullptr)
	, m_pixelConstantBuffer(nullptr)
{
    const auto& elements = vertexLayout_->GetElements();
    for (size_t i = 0; i < elements.size(); i++) {
        const auto& element = elements[i];
        vertexSize_ += ELEMENT_TYPESIZES[element.ValueType] * element.NumComponents;
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

	return new Shader(graphicsDevice.DownCast<Backend::GraphicsDevice>(), shader.DownCast<Backend::Shader>(), vertexLayout.DownCast<Backend::VertexLayout>());
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
