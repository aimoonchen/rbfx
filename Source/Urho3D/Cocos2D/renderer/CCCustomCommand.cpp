/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "renderer/CCCustomCommand.h"
#include "renderer/CCTextureAtlas.h"
// #include "renderer/backend/Buffer.h"
// #include "renderer/backend/Device.h"
#include "base/ccUtils.h"
#include "base/CCDirector.h"
#include "renderer/CCRenderer.h"

#include <Diligent/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Diligent/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include "../../RenderAPI/RenderDevice.h"
NS_CC_BEGIN

CustomCommand::CustomCommand()
{
    _type = RenderCommand::Type::CUSTOM_COMMAND;
    auto renderer = Director::getInstance()->getRenderer();
    _device = renderer->GetRenderDevice();
}

CustomCommand::~CustomCommand()
{
//     CC_SAFE_RELEASE(_vertexBuffer);
//     CC_SAFE_RELEASE(_indexBuffer);
}

void CustomCommand::init(float depth, const cocos2d::Mat4 &modelViewTransform, unsigned int flags)
{
    RenderCommand::init(depth, modelViewTransform, flags);
}

void CustomCommand::init(float globalZOrder)
{
    _globalOrder = globalZOrder;
}

void CustomCommand::init(float globalZOrder, const BlendFunc& blendFunc)
{
    _globalOrder = globalZOrder;

    auto& blendDescriptor = _pipelineDescriptor.blendDescriptor;
    blendDescriptor.blendEnabled = true;
    blendDescriptor.sourceRGBBlendFactor = blendDescriptor.sourceAlphaBlendFactor = blendFunc.src;
    blendDescriptor.destinationRGBBlendFactor = blendDescriptor.destinationAlphaBlendFactor = blendFunc.dst;
}

void CustomCommand::createVertexBuffer(std::size_t vertexSize, std::size_t capacity, BufferUsage usage, ea::vector<Diligent::LayoutElement> layout)
{
    _vertexBuffUsage = usage;
//    CC_SAFE_RELEASE(_vertexBuffer);
    _vertexLayout = std::move(layout);
    _vertexCapacity = capacity;
    _vertexDrawCount = capacity;
    
//     auto device = backend::Device::getInstance();
//     _vertexBuffer = device->newBuffer(vertexSize * capacity, backend::BufferType::VERTEX, usage);
    bool isDynamic = (usage == backend::BufferUsage::DYNAMIC);
    Diligent::BufferDesc VertBuffDesc;
    VertBuffDesc.Name = "CustomCommand vertex buffer";
    VertBuffDesc.BindFlags = Diligent::BIND_VERTEX_BUFFER;
    VertBuffDesc.Usage = isDynamic ? Diligent::USAGE_DYNAMIC : Diligent::USAGE_DEFAULT;
    if (isDynamic) {
        VertBuffDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
    }
    VertBuffDesc.Size = vertexSize * capacity;
    _device->GetRenderDevice()->CreateBuffer(VertBuffDesc, nullptr, &_vertexBuffer);
}

void CustomCommand::createIndexBuffer(IndexFormat format, std::size_t capacity, BufferUsage usage)
{
    _indexBuffUsage = usage;
//    CC_SAFE_RELEASE(_indexBuffer);
    
    _indexFormat = format;
    _indexSize = computeIndexSize();
    _indexCapacity = capacity;
    _indexDrawCount = capacity;
    
//     auto device = backend::Device::getInstance();
//     _indexBuffer = device->newBuffer(_indexSize * capacity, backend::BufferType::INDEX, usage);
    bool isDynamic = (usage == backend::BufferUsage::DYNAMIC);
    Diligent::BufferDesc IndBuffDesc;
    IndBuffDesc.Name = "CustomCommand index buffer";
    IndBuffDesc.Usage = isDynamic ? Diligent::USAGE_DYNAMIC : Diligent::USAGE_DEFAULT; // Diligent::USAGE_IMMUTABLE;
    IndBuffDesc.BindFlags = Diligent::BIND_INDEX_BUFFER;
    IndBuffDesc.Size = _indexSize * capacity;
    if (isDynamic) {
        IndBuffDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
    }
    _device->GetRenderDevice()->CreateBuffer(IndBuffDesc, nullptr, &_indexBuffer);
}

void CustomCommand::updateVertexBuffer(void* data, std::size_t offset, std::size_t length)
{   
    assert(_vertexBuffer);
    if (_vertexBuffUsage == BufferUsage::STATIC) {
        _device->GetImmediateContext()->UpdateBuffer(_vertexBuffer, offset, length, data, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    } else {
        void* dst = nullptr;
        _device->GetImmediateContext()->MapBuffer(_vertexBuffer, Diligent::MAP_WRITE, Diligent::MAP_FLAG_NO_OVERWRITE /*Diligent::MAP_FLAG_DISCARD*/, dst);
        if (dst) {
            memcpy((uint8_t*)dst + offset, data, length);
            _device->GetImmediateContext()->UnmapBuffer(_vertexBuffer, Diligent::MAP_WRITE);
        }
    }
}

void CustomCommand::updateIndexBuffer(void* data, std::size_t offset, std::size_t length)
{
    assert(_indexBuffer);
    if (_indexBuffUsage == BufferUsage::STATIC) {
        _device->GetImmediateContext()->UpdateBuffer(_indexBuffer, offset, length, data, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    } else {
        void* dst = nullptr;
        _device->GetImmediateContext()->MapBuffer(_indexBuffer, Diligent::MAP_WRITE, Diligent::MAP_FLAG_NO_OVERWRITE /*Diligent::MAP_FLAG_DISCARD*/, dst);
        if (dst) {
            memcpy((uint8_t*)dst + offset, data, length);
            _device->GetImmediateContext()->UnmapBuffer(_indexBuffer, Diligent::MAP_WRITE);
        }
    }
}

void CustomCommand::setVertexBuffer(Diligent::IBuffer *vertexBuffer)
{
    if (_vertexBuffer == vertexBuffer)
        return;

    _vertexBuffer = vertexBuffer;
}

void CustomCommand::setIndexBuffer(Diligent::IBuffer*indexBuffer, IndexFormat format)
{
    if (_indexBuffer == indexBuffer && _indexFormat == format)
        return;

    _indexBuffer = indexBuffer;

    _indexFormat = format;
    _indexSize = computeIndexSize();
}

void CustomCommand::updateVertexBuffer(void* data, std::size_t length)
{
    assert(_vertexBuffer);
    updateVertexBuffer(data, 0, length);
}

void CustomCommand::updateIndexBuffer(void* data, std::size_t length)
{
    assert(_indexBuffer);
    updateIndexBuffer(data, 0, length);
}

std::size_t CustomCommand::computeIndexSize() const
{
if (IndexFormat::U_SHORT == _indexFormat)
    return sizeof(unsigned short);
else
    return sizeof(unsigned int);
}

NS_CC_END