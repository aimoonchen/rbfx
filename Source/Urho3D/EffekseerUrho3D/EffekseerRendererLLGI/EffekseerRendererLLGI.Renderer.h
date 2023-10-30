
#ifndef __EFFEKSEERRENDERER_LLGI_RENDERER_H__
#define __EFFEKSEERRENDERER_LLGI_RENDERER_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererLLGI.Base.h"

#include "GraphicsDevice.h"
#include "../../Graphics/Graphics.h"
// #include <LLGI.Buffer.h>
// #include <LLGI.CommandList.h>
// #include <LLGI.Graphics.h>

namespace Urho3D
{
class DrawCommandQueue;
}
namespace EffekseerRendererLLGI
{

struct FixedShader
{
	std::string_view SpriteUnlit_VS;
	std::string_view SpriteLit_VS;
	std::string_view SpriteDistortion_VS;
	std::string_view ModelUnlit_VS;
	std::string_view ModelLit_VS;
	std::string_view ModelDistortion_VS;

	std::string_view ModelUnlit_PS;
	std::string_view ModelLit_PS;
	std::string_view ModelDistortion_PS;

	std::string_view AdvancedSpriteUnlit_VS;
	std::string_view AdvancedSpriteLit_VS;
	std::string_view AdvancedSpriteDistortion_VS;
	std::string_view AdvancedModelUnlit_VS;
	std::string_view AdvancedModelLit_VS;
	std::string_view AdvancedModelDistortion_VS;

	std::string_view AdvancedModelUnlit_PS;
	std::string_view AdvancedModelLit_PS;
	std::string_view AdvancedModelDistortion_PS;
};

/**
	@brief	Renderer class
*/
class Renderer : public ::EffekseerRenderer::Renderer
{
protected:
	Renderer()
	{
	}
	virtual ~Renderer()
	{
	}

public:
	virtual Urho3D::Graphics* GetGraphics() const = 0;
};

// class SingleFrameMemoryPool : public ::EffekseerRenderer::SingleFrameMemoryPool, public ::Effekseer::ReferenceObject
// {
// 	LLGI::SingleFrameMemoryPool* memoryPool_ = nullptr;
// 
// public:
// 	SingleFrameMemoryPool(LLGI::SingleFrameMemoryPool* memoryPool)
// 	{
// 		memoryPool_ = memoryPool;
// 		ES_SAFE_ADDREF(memoryPool_);
// 	}
// 
// 	virtual ~SingleFrameMemoryPool()
// 	{
// 		ES_SAFE_RELEASE(memoryPool_);
// 	}
// 
// 	void NewFrame() override
// 	{
// 		memoryPool_->NewFrame();
// 	}
// 
// 	LLGI::SingleFrameMemoryPool* GetInternal()
// 	{
// 		return memoryPool_;
// 	}
// 
// 	virtual int GetRef() override
// 	{
// 		return ::Effekseer::ReferenceObject::GetRef();
// 	}
// 	virtual int AddRef() override
// 	{
// 		return ::Effekseer::ReferenceObject::AddRef();
// 	}
// 	virtual int Release() override
// 	{
// 		return ::Effekseer::ReferenceObject::Release();
// 	}
// };

enum class CommandListState
{
	Wait,
	Running,
	RunningWithPlatformCommandList,
};

class CommandList : public ::EffekseerRenderer::CommandList, public ::Effekseer::ReferenceObject
{
private:
	Urho3D::Graphics* graphics_ = nullptr;
// 	LLGI::CommandList* commandList_ = nullptr;
// 	LLGI::SingleFrameMemoryPool* memoryPool_ = nullptr;
    Urho3D::DrawCommandQueue* commandList_ = nullptr;
	CommandListState state_ = CommandListState::Wait;

public:
	CommandList(Urho3D::Graphics* graphics, Urho3D::DrawCommandQueue* commandList/*, LLGI::SingleFrameMemoryPool* memoryPool*/)
		: graphics_(graphics)
		, commandList_(commandList)
		//, memoryPool_(memoryPool)
	{
// 		ES_SAFE_ADDREF(graphics_);
// 		ES_SAFE_ADDREF(commandList_);
// 		ES_SAFE_ADDREF(memoryPool_);
	}

	virtual ~CommandList()
	{
// 		ES_SAFE_RELEASE(graphics_);
// 		ES_SAFE_RELEASE(commandList_);
// 		ES_SAFE_RELEASE(memoryPool_);
	}

    Urho3D::Graphics* GetGraphics()
	{
		return graphics_;
	}

    Urho3D::DrawCommandQueue* GetInternal()
	{
		return commandList_;
	}

// 	LLGI::SingleFrameMemoryPool* GetMemoryPool()
// 	{
// 		return memoryPool_;
// 	}

	CommandListState GetState() const
	{
		return state_;
	}

	void SetState(CommandListState state)
	{
		state_ = state;
	}

	virtual int GetRef() override
	{
		return ::Effekseer::ReferenceObject::GetRef();
	}
	virtual int AddRef() override
	{
		return ::Effekseer::ReferenceObject::AddRef();
	}
	virtual int Release() override
	{
		return ::Effekseer::ReferenceObject::Release();
	}
};

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_RENDERER_H__
