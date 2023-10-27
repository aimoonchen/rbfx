
#pragma once

#include "../EffekseerRendererCommon/EffekseerRenderer.IndexBufferBase.h"
#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"
#include "../../Graphics/IndexBuffer.h"

namespace EffekseerRendererLLGI
{

class IndexBuffer : public DeviceObject, public ::EffekseerRenderer::IndexBufferBase
{
private:
    ea::unique_ptr<Urho3D::IndexBuffer> indexBuffer = nullptr;

	IndexBuffer(Backend::GraphicsDevice* graphicsDevice, ea::unique_ptr<Urho3D::IndexBuffer> buffer, int maxCount, bool isDynamic, bool hasRefCount);

public:
	virtual ~IndexBuffer();

	static IndexBuffer* Create(Backend::GraphicsDevice* graphicsDevice, int maxCount, bool isDynamic, bool hasRefCount);

    Urho3D::IndexBuffer* GetIndexBuffer()
	{
		return indexBuffer.get();
	}

public:
	void Lock();
	void Unlock();
};

} // namespace EffekseerRendererLLGI
