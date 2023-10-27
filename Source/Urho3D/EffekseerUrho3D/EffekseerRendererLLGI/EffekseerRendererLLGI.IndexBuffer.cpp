
#include "EffekseerRendererLLGI.IndexBuffer.h"
#include "../../Graphics/Graphics.h"
namespace EffekseerRendererLLGI
{

IndexBuffer::IndexBuffer(Backend::GraphicsDevice* graphicsDevice, ea::unique_ptr<Urho3D::IndexBuffer> buffer, int maxCount, bool isDynamic, bool hasRefCount)
	: DeviceObject(graphicsDevice, hasRefCount)
	, IndexBufferBase(maxCount, isDynamic)
	, indexBuffer(ea::move(buffer))
{
    m_resource = new uint8_t[m_indexMaxCount * stride_];
    indexBuffer->SetSize(maxCount, stride_ > 2, isDynamic);
}

IndexBuffer::~IndexBuffer()
{
	indexBuffer = nullptr;
}

IndexBuffer* IndexBuffer::Create(Backend::GraphicsDevice* graphicsDevice, int maxCount, bool isDynamic, bool hasRefCount)
{
// 	auto indexBuffer = graphicsDevice->GetGraphics()->CreateBuffer(LLGI::BufferUsageType::Index | LLGI::BufferUsageType::MapWrite, 2 * maxCount);
//     
//     if (indexBuffer == nullptr)
// 		return nullptr;

	return new IndexBuffer(graphicsDevice, ea::make_unique<Urho3D::IndexBuffer>(graphicsDevice->GetGraphics()->GetContext()), maxCount, isDynamic, hasRefCount);
}

void IndexBuffer::Lock()
{
	assert(!m_isLock);

	m_isLock = true;
	//m_resource = (uint8_t*)indexBuffer->Lock();
	m_indexCount = 0;
}

void IndexBuffer::Unlock()
{
	assert(m_isLock);
// 	indexBuffer->Unlock();
// 	m_resource = nullptr;
    indexBuffer->SetUnpackedData((unsigned int*)m_resource);
	m_isLock = false;
}

} // namespace EffekseerRendererLLGI
