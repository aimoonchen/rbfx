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
#include "renderer/CCRenderer.h"
//#include <algorithm>

#include "renderer/CCTrianglesCommand.h"
#include "renderer/CCCustomCommand.h"
#include "renderer/CCCallbackCommand.h"
#include "renderer/CCGroupCommand.h"
#include "renderer/ccShaders.h"

#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCEventType.h"
#include "2d/CCCamera.h"
#include "2d/CCScene.h"
#include "external/xxhash/xxhash.h"

#include <Diligent/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Diligent/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include "../../Core/Context.h"
#include "../../RenderAPI/RenderDevice.h"
#include "../../Graphics/Texture2D.h"
#include "../../Graphics/ShaderVariation.h"

#include "Urho3DContext.h"

NS_CC_BEGIN

// helper
static bool compareRenderCommand(RenderCommand* a, RenderCommand* b)
{
    return a->getGlobalOrder() < b->getGlobalOrder();
}

static bool compare3DCommand(RenderCommand* a, RenderCommand* b)
{
    return  a->getDepth() > b->getDepth();
}

// queue
RenderQueue::RenderQueue()
{
}

void RenderQueue::push_back(RenderCommand* command)
{
    float z = command->getGlobalOrder();
    if(z < 0)
    {
        _commands[QUEUE_GROUP::GLOBALZ_NEG].push_back(command);
    }
    else if(z > 0)
    {
        _commands[QUEUE_GROUP::GLOBALZ_POS].push_back(command);
    }
    else
    {
        if(command->is3D())
        {
            if(command->isTransparent())
            {
                _commands[QUEUE_GROUP::TRANSPARENT_3D].push_back(command);
            }
            else
            {
                _commands[QUEUE_GROUP::OPAQUE_3D].push_back(command);
            }
        }
        else
        {
            _commands[QUEUE_GROUP::GLOBALZ_ZERO].push_back(command);
        }
    }
}

ssize_t RenderQueue::size() const
{
    ssize_t result(0);
    for(int index = 0; index < QUEUE_GROUP::QUEUE_COUNT; ++index)
    {
        result += _commands[index].size();
    }
    
    return result;
}

void RenderQueue::sort()
{
    // Don't sort _queue0, it already comes sorted
    std::stable_sort(std::begin(_commands[QUEUE_GROUP::TRANSPARENT_3D]), std::end(_commands[QUEUE_GROUP::TRANSPARENT_3D]), compare3DCommand);
    std::stable_sort(std::begin(_commands[QUEUE_GROUP::GLOBALZ_NEG]), std::end(_commands[QUEUE_GROUP::GLOBALZ_NEG]), compareRenderCommand);
    std::stable_sort(std::begin(_commands[QUEUE_GROUP::GLOBALZ_POS]), std::end(_commands[QUEUE_GROUP::GLOBALZ_POS]), compareRenderCommand);
}

RenderCommand* RenderQueue::operator[](ssize_t index) const
{
    for(int queIndex = 0; queIndex < QUEUE_GROUP::QUEUE_COUNT; ++queIndex)
    {
        if(index < static_cast<ssize_t>(_commands[queIndex].size()))
            return _commands[queIndex][index];
        else
        {
            index -= _commands[queIndex].size();
        }
    }
    
    CCASSERT(false, "invalid index");
    return nullptr;
}

void RenderQueue::clear()
{
    for(int i = 0; i < QUEUE_GROUP::QUEUE_COUNT; ++i)
    {
        _commands[i].clear();
    }
}

void RenderQueue::realloc(size_t reserveSize)
{
    for(int i = 0; i < QUEUE_GROUP::QUEUE_COUNT; ++i)
    {
        _commands[i] = std::vector<RenderCommand*>();
        _commands[i].reserve(reserveSize);
    }
}

//
//
//
static const int DEFAULT_RENDER_QUEUE = 0;

//
// constructors, destructor, init
//
Renderer::Renderer()
{
    _device = GetUrho3DContext()->GetSubsystem<Urho3D::RenderDevice>();
    _groupCommandManager = new (std::nothrow) GroupCommandManager();
    
    _commandGroupStack.push(DEFAULT_RENDER_QUEUE);
    
    RenderQueue defaultRenderQueue;
    _renderGroups.push_back(defaultRenderQueue);
    _queuedTriangleCommands.reserve(BATCH_TRIAGCOMMAND_RESERVED_SIZE);

    // for the batched TriangleCommand
    _triBatchesToDraw = (TriBatchToDraw*) malloc(sizeof(_triBatchesToDraw[0]) * _triBatchesToDrawCapacity);
}

Renderer::~Renderer()
{
    _renderGroups.clear();
    _groupCommandManager->release();
    
    free(_triBatchesToDraw);
    
//     CC_SAFE_RELEASE(_commandBuffer);
//     CC_SAFE_RELEASE(_renderPipeline);
}

void Renderer::init()
{
    // Should invoke _triangleCommandBufferManager.init() first.
    _triangleCommandBufferManager.init(_device);
    _vertexBuffer = _triangleCommandBufferManager.getVertexBuffer();
    _indexBuffer = _triangleCommandBufferManager.getIndexBuffer();
    _triangleCommandVertexLayout = {
        Diligent::LayoutElement{0, 0, 3, Diligent::VT_FLOAT32, false},
        Diligent::LayoutElement{1, 0, 4, Diligent::VT_UINT8, true},
        Diligent::LayoutElement{2, 0, 2, Diligent::VT_FLOAT32, false}
    };
//     auto device = backend::Device::getInstance();
//     _commandBuffer = device->newCommandBuffer();
//     _renderPipeline = device->newRenderPipeline();
//     _commandBuffer->setRenderPipeline(_renderPipeline);
}

void Renderer::addCommand(RenderCommand* command)
{
    int renderQueueID =_commandGroupStack.top();
    addCommand(command, renderQueueID);
}

void Renderer::addCommand(RenderCommand* command, int renderQueueID)
{
    CCASSERT(!_isRendering, "Cannot add command while rendering");
    CCASSERT(renderQueueID >=0, "Invalid render queue");
    CCASSERT(command->getType() != RenderCommand::Type::UNKNOWN_COMMAND, "Invalid Command Type");

    _renderGroups[renderQueueID].push_back(command);
}

void Renderer::pushGroup(int renderQueueID)
{
    CCASSERT(!_isRendering, "Cannot change render queue while rendering");
    _commandGroupStack.push(renderQueueID);
}

void Renderer::popGroup()
{
    CCASSERT(!_isRendering, "Cannot change render queue while rendering");
    _commandGroupStack.pop();
}

int Renderer::createRenderQueue()
{
    RenderQueue newRenderQueue;
    _renderGroups.push_back(newRenderQueue);
    return (int)_renderGroups.size() - 1;
}

void Renderer::processGroupCommand(GroupCommand* command)
{
    flush();

    int renderQueueID = ((GroupCommand*) command)->getRenderQueueID();

    pushStateBlock();
    //apply default state for all render queues
    setDepthTest(false);
    setDepthWrite(false);
    setCullMode(backend::CullMode::NONE);
    visitRenderQueue(_renderGroups[renderQueueID]);
    popStateBlock();
}

void Renderer::processRenderCommand(RenderCommand* command)
{
    auto commandType = command->getType();
    switch(commandType)
    {
        case RenderCommand::Type::TRIANGLES_COMMAND:
        {
            // flush other queues
            flush3D();
            
            auto cmd = static_cast<TrianglesCommand*>(command);
            
            // flush own queue when buffer is full
            if(_queuedTotalVertexCount + cmd->getVertexCount() > VBO_SIZE || _queuedTotalIndexCount + cmd->getIndexCount() > INDEX_VBO_SIZE)
            {
                CCASSERT(cmd->getVertexCount()>= 0 && cmd->getVertexCount() < VBO_SIZE, "VBO for vertex is not big enough, please break the data down or use customized render command");
                CCASSERT(cmd->getIndexCount()>= 0 && cmd->getIndexCount() < INDEX_VBO_SIZE, "VBO for index is not big enough, please break the data down or use customized render command");
                drawBatchedTriangles();

                _queuedTotalIndexCount = _queuedTotalVertexCount = 0;
//#ifdef CC_USE_METAL
                _queuedIndexCount = _queuedVertexCount = 0;
                _triangleCommandBufferManager.prepareNextBuffer();
                _vertexBuffer = _triangleCommandBufferManager.getVertexBuffer();
                _indexBuffer = _triangleCommandBufferManager.getIndexBuffer();
//#endif
            }
            
            // queue it
            _queuedTriangleCommands.push_back(cmd);
//#ifdef CC_USE_METAL
            _queuedIndexCount += cmd->getIndexCount();
            _queuedVertexCount += cmd->getVertexCount();
//#endif
            _queuedTotalVertexCount += cmd->getVertexCount();
            _queuedTotalIndexCount += cmd->getIndexCount();

        }
            break;
        case RenderCommand::Type::MESH_COMMAND:
            flush2D();
            drawMeshCommand(command);
            break;
        case RenderCommand::Type::GROUP_COMMAND:
            processGroupCommand(static_cast<GroupCommand*>(command));
            break;
        case RenderCommand::Type::CUSTOM_COMMAND:
            flush();
            drawCustomCommand(command);
            break;
        case RenderCommand::Type::CALLBACK_COMMAND:
            flush();
           static_cast<CallbackCommand*>(command)->execute();
            break;
//         case RenderCommand::Type::CAPTURE_SCREEN_COMMAND:
//             flush();
//             captureScreen(command);
//             break;
        default:
            assert(false);
            break;
    }
}

// void Renderer::captureScreen(RenderCommand *command)
// {
//     _commandBuffer->captureScreen(static_cast<CaptureScreenCallbackCommand*>(command)->func);
// }

void Renderer::visitRenderQueue(RenderQueue& queue)
{
    //
    //Process Global-Z < 0 Objects
    //
    doVisitRenderQueue(queue.getSubQueue(RenderQueue::QUEUE_GROUP::GLOBALZ_NEG));

    //
    //Process Opaque Object
    //
    pushStateBlock();
    setDepthTest(true); //enable depth test in 3D queue by default
    setDepthWrite(true);
    setCullMode(backend::CullMode::BACK);
    doVisitRenderQueue(queue.getSubQueue(RenderQueue::QUEUE_GROUP::OPAQUE_3D));
    
    //
    //Process 3D Transparent object
    //
    setDepthWrite(false);
    doVisitRenderQueue(queue.getSubQueue(RenderQueue::QUEUE_GROUP::TRANSPARENT_3D));
    popStateBlock();

    //
    //Process Global-Z = 0 Queue
    //
    doVisitRenderQueue(queue.getSubQueue(RenderQueue::QUEUE_GROUP::GLOBALZ_ZERO));
        
    //
    //Process Global-Z > 0 Queue
    //
    doVisitRenderQueue(queue.getSubQueue(RenderQueue::QUEUE_GROUP::GLOBALZ_POS));

}

void Renderer::doVisitRenderQueue(const std::vector<RenderCommand*>& renderCommands)
{
    for (const auto& command : renderCommands)
    {
        processRenderCommand(command);
    }
    flush();
}

void Renderer::render()
{
    //TODO: setup camera or MVP
    _isRendering = true;
//    if (_glViewAssigned)
    {
        //Process render commands
        //1. Sort render commands based on ID
        for (auto &renderqueue : _renderGroups)
        {
            renderqueue.sort();
        }
        visitRenderQueue(_renderGroups[0]);
    }
    clean();
    _isRendering = false;
}

void Renderer::beginFrame()
{
    //_commandBuffer->beginFrame();
}

void Renderer::endFrame()
{
    //_commandBuffer->endFrame();

// #ifdef CC_USE_METAL
    _triangleCommandBufferManager.putbackAllBuffers();
    _vertexBuffer = _triangleCommandBufferManager.getVertexBuffer();
    _indexBuffer = _triangleCommandBufferManager.getIndexBuffer();
// #endif
    _queuedTotalIndexCount = 0;
    _queuedTotalVertexCount = 0;
}

void Renderer::clean()
{
    // Clear render group
    for (size_t j = 0, size = _renderGroups.size() ; j < size; j++)
    {
        //commands are owned by nodes
        // for (const auto &cmd : _renderGroups[j])
        // {
        //     cmd->releaseToCommandPool();
        // }
        _renderGroups[j].clear();
    }

    // Clear batch commands
    _queuedTriangleCommands.clear();
}

void Renderer::setDepthTest(bool value)
{
    _depthStencilDescriptor.depthTestEnabled = value;
    _renderPassDescriptor.depthTestEnabled = value;
}

void Renderer::setDepthWrite(bool value)
{
    _depthStencilDescriptor.depthWriteEnabled = value;
}

void Renderer::setDepthCompareFunction(backend::CompareFunction func)
{
    _depthStencilDescriptor.depthCompareFunction = func;
}

backend::CompareFunction Renderer::getDepthCompareFunction() const
{
    return _depthStencilDescriptor.depthCompareFunction;
}

bool Renderer::Renderer::getDepthTest() const
{
    return _depthStencilDescriptor.depthTestEnabled;
}

bool Renderer::getDepthWrite() const
{
    return _depthStencilDescriptor.depthWriteEnabled;
}

void Renderer::setStencilTest(bool value)
{
    _depthStencilDescriptor.stencilTestEnabled = value;
    _renderPassDescriptor.stencilTestEnabled = value;
}

void Renderer::setStencilCompareFunction(backend::CompareFunction func, unsigned int ref, unsigned int readMask)
{
    _depthStencilDescriptor.frontFaceStencil.stencilCompareFunction = func;
    _depthStencilDescriptor.backFaceStencil.stencilCompareFunction = func;

    _depthStencilDescriptor.frontFaceStencil.readMask = readMask;
    _depthStencilDescriptor.backFaceStencil.readMask = readMask;

    _stencilRef = ref;
}

void Renderer::setStencilOperation(backend::StencilOperation stencilFailureOp,
                             backend::StencilOperation depthFailureOp,
                             backend::StencilOperation stencilDepthPassOp)
{
    _depthStencilDescriptor.frontFaceStencil.stencilFailureOperation = stencilFailureOp;
    _depthStencilDescriptor.backFaceStencil.stencilFailureOperation = stencilFailureOp;

    _depthStencilDescriptor.frontFaceStencil.depthFailureOperation = depthFailureOp;
    _depthStencilDescriptor.backFaceStencil.depthFailureOperation = depthFailureOp;

    _depthStencilDescriptor.frontFaceStencil.depthStencilPassOperation = stencilDepthPassOp;
    _depthStencilDescriptor.backFaceStencil.depthStencilPassOperation = stencilDepthPassOp;
}

void Renderer::setStencilWriteMask(unsigned int mask)
{
    _depthStencilDescriptor.frontFaceStencil.writeMask = mask;
    _depthStencilDescriptor.backFaceStencil.writeMask = mask;
}

bool Renderer::getStencilTest() const
{
    return _depthStencilDescriptor.stencilTestEnabled;
}

backend::StencilOperation Renderer::getStencilFailureOperation() const
{
    return _depthStencilDescriptor.frontFaceStencil.stencilFailureOperation;
}

backend::StencilOperation Renderer::getStencilPassDepthFailureOperation() const
{
    return _depthStencilDescriptor.frontFaceStencil.depthFailureOperation;
}

backend::StencilOperation Renderer::getStencilDepthPassOperation() const
{
    return _depthStencilDescriptor.frontFaceStencil.depthStencilPassOperation;
}

backend::CompareFunction Renderer::getStencilCompareFunction() const
{
    return _depthStencilDescriptor.depthCompareFunction;
}

unsigned int Renderer::getStencilReadMask() const
{
    return _depthStencilDescriptor.frontFaceStencil.readMask;
}

unsigned int Renderer::getStencilWriteMask() const
{
    return _depthStencilDescriptor.frontFaceStencil.writeMask;
}

unsigned int Renderer::getStencilReferenceValue() const
{
    return _stencilRef;
}

void Renderer::setViewPort(int x, int y, unsigned int w, unsigned int h)
{
    _viewport.x = x;
    _viewport.y = y;
    _viewport.w = w;
    _viewport.h = h;
}

void Renderer::fillVerticesAndIndices(const TrianglesCommand* cmd, unsigned int vertexBufferOffset)
{
    size_t vertexCount = cmd->getVertexCount();
    memcpy(&_verts[_filledVertex], cmd->getVertices(), sizeof(V3F_C4B_T2F) * vertexCount);
    
    // fill vertex, and convert them to world coordinates
    const Mat4& modelView = cmd->getModelView();
    for (size_t i=0; i < vertexCount; ++i)
    {
        modelView.transformPoint(&(_verts[i + _filledVertex].vertices));
    }
    
    // fill index
    const unsigned short* indices = cmd->getIndices();
    size_t indexCount = cmd->getIndexCount();
    for (size_t i = 0; i < indexCount; ++i)
    {
        _indices[_filledIndex + i] = vertexBufferOffset + _filledVertex + indices[i];
    }
    
    _filledVertex += vertexCount;
    _filledIndex += indexCount;
}

void Renderer::drawBatchedTriangles()
{
    if(_queuedTriangleCommands.empty())
        return;
    
    /************** 1: Setup up vertices/indices *************/
// #ifdef CC_USE_METAL
    unsigned int vertexBufferFillOffset = _queuedTotalVertexCount - _queuedVertexCount;
    unsigned int indexBufferFillOffset = _queuedTotalIndexCount - _queuedIndexCount;
// #else
//     unsigned int vertexBufferFillOffset = 0;
//     unsigned int indexBufferFillOffset = 0;
// #endif

    _triBatchesToDraw[0].offset = indexBufferFillOffset;
    _triBatchesToDraw[0].indicesToDraw = 0;
    _triBatchesToDraw[0].cmd = nullptr;
    
    int batchesTotal = 0;
    int prevMaterialID = -1;
    bool firstCommand = true;

    _filledVertex = 0;
    _filledIndex = 0;
    auto deviceContext = _device->GetImmediateContext();
    void* vertDst = nullptr;
//     deviceContext->MapBuffer(_vertexBuffer, Diligent::MAP_WRITE, vertexBufferFillOffset != 0 ? Diligent::MAP_FLAG_NO_OVERWRITE : Diligent::MAP_FLAG_DISCARD, vertDst);
//     _verts = (V3F_C4B_T2F*)vertDst;
    void* indexDst = nullptr;
//     deviceContext->MapBuffer(_indexBuffer, Diligent::MAP_WRITE, indexBufferFillOffset != 0 ? Diligent::MAP_FLAG_NO_OVERWRITE : Diligent::MAP_FLAG_DISCARD, indexDst);
//     _indices = (uint16_t*)indexDst;
    for (const auto& cmd : _queuedTriangleCommands)
    {
        auto currentMaterialID = cmd->getMaterialID();
        const bool batchable = !cmd->isSkipBatching();
        
        fillVerticesAndIndices(cmd, vertexBufferFillOffset);
        
        // in the same batch ?
        if (batchable && (prevMaterialID == currentMaterialID || firstCommand))
        {
            CC_ASSERT((firstCommand || _triBatchesToDraw[batchesTotal].cmd->getMaterialID() == cmd->getMaterialID()) && "argh... error in logic");
            _triBatchesToDraw[batchesTotal].indicesToDraw += cmd->getIndexCount();
            _triBatchesToDraw[batchesTotal].cmd = cmd;
        }
        else
        {
            // is this the first one?
            if (!firstCommand)
            {
                batchesTotal++;
                _triBatchesToDraw[batchesTotal].offset =
                    _triBatchesToDraw[batchesTotal-1].offset + _triBatchesToDraw[batchesTotal-1].indicesToDraw;
            }
            
            _triBatchesToDraw[batchesTotal].cmd = cmd;
            _triBatchesToDraw[batchesTotal].indicesToDraw = (int) cmd->getIndexCount();
            
            // is this a single batch ? Prevent creating a batch group then
            if (!batchable)
                currentMaterialID = -1;
        }
        
        // capacity full ?
        if (batchesTotal + 1 >= _triBatchesToDrawCapacity)
        {
            _triBatchesToDrawCapacity *= 1.4;
            _triBatchesToDraw = (TriBatchToDraw*) realloc(_triBatchesToDraw, sizeof(_triBatchesToDraw[0]) * _triBatchesToDrawCapacity);
        }
        
        prevMaterialID = currentMaterialID;
        firstCommand = false;
    }
    batchesTotal++;

#if defined(__EMSCRIPTEN__)
    deviceContext->UpdateBuffer(_vertexBuffer, vertexBufferFillOffset * sizeof(_verts[0]), _filledVertex * sizeof(_verts[0]), _verts, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    deviceContext->UpdateBuffer(_indexBuffer, indexBufferFillOffset * sizeof(_indices[0]), _filledIndex * sizeof(_indices[0]), _indices, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
#else
    deviceContext->MapBuffer(_vertexBuffer, Diligent::MAP_WRITE, vertexBufferFillOffset != 0 ? Diligent::MAP_FLAG_NO_OVERWRITE : Diligent::MAP_FLAG_DISCARD, vertDst);
    if (vertDst) {
        memcpy((uint8_t*)vertDst + vertexBufferFillOffset * sizeof(_verts[0]), _verts, _filledVertex * sizeof(_verts[0]));
        deviceContext->UnmapBuffer(_vertexBuffer, Diligent::MAP_WRITE);
    }
    deviceContext->MapBuffer(_indexBuffer, Diligent::MAP_WRITE, indexBufferFillOffset != 0 ? Diligent::MAP_FLAG_NO_OVERWRITE : Diligent::MAP_FLAG_DISCARD, indexDst);
    if (indexDst) {
        memcpy((uint8_t*)indexDst + indexBufferFillOffset * sizeof(_indices[0]), _indices, _filledIndex * sizeof(_indices[0]));
        deviceContext->UnmapBuffer(_indexBuffer, Diligent::MAP_WRITE);
    }
#endif
// #ifdef CC_USE_METAL
//     _vertexBuffer->updateSubData(_verts, vertexBufferFillOffset * sizeof(_verts[0]), _filledVertex * sizeof(_verts[0]));
//     _indexBuffer->updateSubData(_indices, indexBufferFillOffset * sizeof(_indices[0]), _filledIndex * sizeof(_indices[0]));
// #else
//     _vertexBuffer->updateData(_verts, _filledVertex * sizeof(_verts[0]));
//     _indexBuffer->updateData(_indices,  _filledIndex * sizeof(_indices[0]));
// #endif
    /************** 2: Draw *************/
    for (int i = 0; i < batchesTotal; ++i)
    {
        beginRenderPass(_triBatchesToDraw[i].cmd);
//         _commandBuffer->setVertexBuffer(_vertexBuffer);
//         _commandBuffer->setIndexBuffer(_indexBuffer);
        //const Diligent::Uint64 offset = 0;
        Diligent::IBuffer* pBuffs[] = { _vertexBuffer };
        deviceContext->SetVertexBuffers(0, 1, pBuffs, nullptr, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Diligent::SET_VERTEX_BUFFERS_FLAG_RESET);
        deviceContext->SetIndexBuffer(_indexBuffer, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        auto& pipelineDescriptor = _triBatchesToDraw[i].cmd->getPipelineDescriptor();
//        _commandBuffer->setProgramState(pipelineDescriptor.programState);
        Diligent::DrawIndexedAttribs DrawAttrs;
        DrawAttrs.IndexType = Diligent::VT_UINT16;
        DrawAttrs.FirstIndexLocation = _triBatchesToDraw[i].offset;// *sizeof(_indices[0]);
        DrawAttrs.NumIndices = _triBatchesToDraw[i].indicesToDraw;
//         _commandBuffer->drawElements(backend::PrimitiveType::TRIANGLE,
//                                      backend::IndexFormat::U_SHORT,
//                                      _triBatchesToDraw[i].indicesToDraw,
//                                      _triBatchesToDraw[i].offset * sizeof(_indices[0]));
//        _commandBuffer->endRenderPass();
        DrawAttrs.Flags = Diligent::DRAW_FLAG_VERIFY_ALL;
        deviceContext->DrawIndexed(DrawAttrs);
        _drawnBatches++;
        _drawnVertices += _triBatchesToDraw[i].indicesToDraw;
    }

    /************** 3: Cleanup *************/
    _queuedTriangleCommands.clear();

//#ifdef CC_USE_METAL
    _queuedIndexCount = 0;
    _queuedVertexCount = 0;
//#endif
}

void Renderer::drawCustomCommand(RenderCommand *command)
{
    auto cmd = static_cast<CustomCommand*>(command);

    if (cmd->getBeforeCallback()) cmd->getBeforeCallback()();

    beginRenderPass(command);
    //_commandBuffer->setVertexBuffer(cmd->getVertexBuffer());
    auto deviceContext = _device->GetImmediateContext();
    //const Diligent::Uint64 offset = 0;
    Diligent::IBuffer* pBuffs[] = { cmd->getVertexBuffer() };
    deviceContext->SetVertexBuffers(0, 1, pBuffs, nullptr, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Diligent::SET_VERTEX_BUFFERS_FLAG_RESET);
    
 //   _commandBuffer->setProgramState(cmd->getPipelineDescriptor().programState);
    
    auto drawType = cmd->getDrawType();
    //_commandBuffer->setLineWidth(cmd->getLineWidth());
    if (CustomCommand::DrawType::ELEMENT == drawType)
    {
        //_commandBuffer->setIndexBuffer(cmd->getIndexBuffer());
        deviceContext->SetIndexBuffer(cmd->getIndexBuffer(), 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        Diligent::DrawIndexedAttribs DrawAttrs;
        DrawAttrs.IndexType = (cmd->getIndexFormat() == backend::IndexFormat::U_INT) ? Diligent::VT_UINT32 : Diligent::VT_UINT16;
        DrawAttrs.FirstIndexLocation = cmd->getIndexDrawOffset();
        DrawAttrs.NumIndices = cmd->getIndexDrawCount();
        DrawAttrs.Flags = Diligent::DRAW_FLAG_VERIFY_ALL;
        deviceContext->DrawIndexed(DrawAttrs);
        //         _commandBuffer->drawElements(cmd->getPrimitiveType(),
//                                      cmd->getIndexFormat(),
//                                      cmd->getIndexDrawCount(),
//                                      cmd->getIndexDrawOffset());
        _drawnVertices += cmd->getIndexDrawCount();
    }
    else
    {
        Diligent::DrawAttribs DrawAttrs;
        DrawAttrs.StartVertexLocation = cmd->getVertexDrawStart();
        DrawAttrs.NumVertices = cmd->getVertexDrawCount();
        DrawAttrs.Flags = Diligent::DRAW_FLAG_VERIFY_ALL;
        deviceContext->Draw(DrawAttrs);
//         _commandBuffer->drawArrays(cmd->getPrimitiveType(),
//                                    cmd->getVertexDrawStart(),
//                                    cmd->getVertexDrawCount());
        _drawnVertices += cmd->getVertexDrawCount();
    }
    _drawnBatches++;
//    _commandBuffer->endRenderPass();

    if (cmd->getAfterCallback()) cmd->getAfterCallback()();
}

void Renderer::drawMeshCommand(RenderCommand *command)
{
    //CustomCommand are identical while rendering.
    drawCustomCommand(command);
}


void Renderer::flush()
{
    flush2D();
    flush3D();
}

void Renderer::flush2D()
{
    flushTriangles();
}

void Renderer::flush3D()
{
    //TODO 3d batch rendering
}

void Renderer::flushTriangles()
{
    drawBatchedTriangles();
}

// helpers
bool Renderer::checkVisibility(const Mat4 &transform, const Size &size)
{
    auto director = Director::getInstance();
    auto scene = director->getRunningScene();

    //If draw to Rendertexture, return true directly.
    // only cull the default camera. The culling algorithm is valid for default camera.
    if (!scene || (scene && scene->_defaultCamera != Camera::getVisitingCamera()))
        return true;

    Rect visibleRect(director->getVisibleOrigin(), director->getVisibleSize());

    // transform center point to screen space
    float hSizeX = size.width/2;
    float hSizeY = size.height/2;
    Vec3 v3p(hSizeX, hSizeY, 0);
    transform.transformPoint(&v3p);
    Vec2 v2p = Camera::getVisitingCamera()->projectGL(v3p);

    // convert content size to world coordinates
    float wshw = std::max(fabsf(hSizeX * transform.m[0] + hSizeY * transform.m[4]), fabsf(hSizeX * transform.m[0] - hSizeY * transform.m[4]));
    float wshh = std::max(fabsf(hSizeX * transform.m[1] + hSizeY * transform.m[5]), fabsf(hSizeX * transform.m[1] - hSizeY * transform.m[5]));

    // enlarge visible rect half size in screen coord
    visibleRect.origin.x -= wshw;
    visibleRect.origin.y -= wshh;
    visibleRect.size.width += wshw * 2;
    visibleRect.size.height += wshh * 2;
    bool ret = visibleRect.containsPoint(v2p);
    return ret;
}

bool Renderer::StateKey::operator<(const Renderer::StateKey& v) const
{
    if (program != v.program)
        return program < v.program;
    if (blendDescriptor.blendEnabled != v.blendDescriptor.blendEnabled)
        return v.blendDescriptor.blendEnabled;
    if (scissorEnable != v.scissorEnable)
        return v.scissorEnable;
    if (primitiveType != v.primitiveType)
        return primitiveType < v.primitiveType;
    if (cullMode != v.cullMode)
        return cullMode < v.cullMode;
    if (depthTestEnabled != v.depthTestEnabled)
        return v.depthTestEnabled;
    if (stencilTestEnabled != v.stencilTestEnabled)
        return v.stencilTestEnabled;
    if (needClearStencil != v.needClearStencil)
        return v.needClearStencil;
    if (clearStencilValue != v.clearStencilValue)
        return clearStencilValue < v.clearStencilValue;

//     for (int i = 0; i < 4; i++)
//     {
//         if (state.TextureFilterTypes[i] != v.state.TextureFilterTypes[i])
//             return state.TextureFilterTypes[i] < v.state.TextureFilterTypes[i];
//         if (state.TextureWrapTypes[i] != v.state.TextureWrapTypes[i])
//             return state.TextureWrapTypes[i] < v.state.TextureWrapTypes[i];
//     }
    return false;
}

void Renderer::commitUniformAndTextures(const PipelineDescriptor& pipelineDescriptor, Diligent::IPipelineState* pipelineState)
{
    auto programState = pipelineDescriptor.programState;
    auto currentProgram = pipelineDescriptor.programState->getProgram();
    auto deviceContext = _device->GetImmediateContext();
    if (programState->_vertexUniformBufferSize > 0) {
        Diligent::PVoid pMappedData{nullptr};
        auto buffer = currentProgram->_vsConstants;
        deviceContext->MapBuffer(buffer, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD, pMappedData);
        if (pMappedData) {
            memcpy(pMappedData, programState->_vertexUniformBuffer.get(), programState->_vertexUniformBufferSize);
            deviceContext->UnmapBuffer(buffer, Diligent::MAP_WRITE);
        }
    }

    if (programState->_fragmentUniformBufferSize > 0) {
        Diligent::PVoid pMappedData{nullptr};
        auto buffer = currentProgram->_fsConstants;
        deviceContext->MapBuffer(buffer, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD, pMappedData);
        if (pMappedData) {
            memcpy(pMappedData, programState->_fragmentUniformBuffer.get(), programState->_fragmentUniformBufferSize);
            deviceContext->UnmapBuffer(buffer, Diligent::MAP_WRITE);
        }
    }
    auto& shaderResourceVariables = shaderResourceBindings_[pipelineState].shaderResourceVariables;
    for (int32_t i = 0; i < currentProgram->_textureCount; i++) {
        auto texture = programState->_textures[i];
        shaderResourceVariables[i]->Set(texture ? texture->GetHandles().srv_ : (Diligent::ITextureView*)nullptr/*, Diligent::SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE*/);
    }
}
namespace {
ea::array<Diligent::PRIMITIVE_TOPOLOGY, (uint32_t)backend::PrimitiveType::TRIANGLE_STRIP + 1> primitiveTypeMap = {
    Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_POINT_LIST,
    Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_LINE_LIST,
    Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_LINE_STRIP,
    Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
};
ea::array<Diligent::COMPARISON_FUNCTION, (uint32_t)backend::CompareFunction::ALWAYS + 1> compareFunciontMap = {
    Diligent::COMPARISON_FUNCTION::COMPARISON_FUNC_NEVER,
    Diligent::COMPARISON_FUNCTION::COMPARISON_FUNC_LESS,
    Diligent::COMPARISON_FUNCTION::COMPARISON_FUNC_LESS_EQUAL,
    Diligent::COMPARISON_FUNCTION::COMPARISON_FUNC_GREATER,
    Diligent::COMPARISON_FUNCTION::COMPARISON_FUNC_GREATER_EQUAL,
    Diligent::COMPARISON_FUNCTION::COMPARISON_FUNC_EQUAL,
    Diligent::COMPARISON_FUNCTION::COMPARISON_FUNC_NOT_EQUAL,
    Diligent::COMPARISON_FUNCTION::COMPARISON_FUNC_ALWAYS };
ea::array<Diligent::STENCIL_OP, (uint32_t)backend::StencilOperation::DECREMENT_WRAP + 1> stencilOpMap = {
    Diligent::STENCIL_OP::STENCIL_OP_KEEP,
    Diligent::STENCIL_OP::STENCIL_OP_ZERO,
    Diligent::STENCIL_OP::STENCIL_OP_REPLACE,
    Diligent::STENCIL_OP::STENCIL_OP_INVERT,
    Diligent::STENCIL_OP::STENCIL_OP_INCR_WRAP,
    Diligent::STENCIL_OP::STENCIL_OP_DECR_WRAP,
};
ea::array<Diligent::BLEND_OPERATION, (uint32_t)backend::BlendOperation::RESERVE_SUBTRACT + 1> blendOpMap = {
    Diligent::BLEND_OPERATION::BLEND_OPERATION_ADD,
    Diligent::BLEND_OPERATION::BLEND_OPERATION_SUBTRACT,
    Diligent::BLEND_OPERATION::BLEND_OPERATION_REV_SUBTRACT,
};
ea::array<Diligent::BLEND_FACTOR, (uint32_t)backend::BlendFactor::BLEND_CLOLOR> blendFactorMap = {
    Diligent::BLEND_FACTOR::BLEND_FACTOR_ZERO,
    Diligent::BLEND_FACTOR::BLEND_FACTOR_ONE,
    Diligent::BLEND_FACTOR::BLEND_FACTOR_SRC_COLOR,
    Diligent::BLEND_FACTOR::BLEND_FACTOR_INV_SRC_COLOR,
    Diligent::BLEND_FACTOR::BLEND_FACTOR_SRC_ALPHA,
    Diligent::BLEND_FACTOR::BLEND_FACTOR_INV_SRC_ALPHA,
    Diligent::BLEND_FACTOR::BLEND_FACTOR_DEST_COLOR,
    Diligent::BLEND_FACTOR::BLEND_FACTOR_INV_DEST_COLOR,
    Diligent::BLEND_FACTOR::BLEND_FACTOR_DEST_ALPHA,
    Diligent::BLEND_FACTOR::BLEND_FACTOR_INV_DEST_ALPHA,
    Diligent::BLEND_FACTOR::BLEND_FACTOR_BLEND_FACTOR,
    Diligent::BLEND_FACTOR::BLEND_FACTOR_SRC_ALPHA_SAT,
    Diligent::BLEND_FACTOR::BLEND_FACTOR_INV_BLEND_FACTOR,
};
}
Diligent::IPipelineState* Renderer::getOrCreateRenderPipeline(RenderCommand* command, const backend::RenderPassDescriptor& renderPassDescriptor)
{
    const auto& pipelineDescriptor = command->getPipelineDescriptor();
    auto currentProgram = pipelineDescriptor.programState->getProgram();
    auto commandType = command->getType();
    auto primitiveType = (commandType == RenderCommand::Type::TRIANGLES_COMMAND)
        ? backend::PrimitiveType::TRIANGLE
        : static_cast<CustomCommand*>(command)->getPrimitiveType();
    StateKey key;
    key.program = currentProgram;
    key.blendDescriptor = pipelineDescriptor.blendDescriptor;
    key.depthTestEnabled = renderPassDescriptor.depthTestEnabled;
    key.stencilTestEnabled = renderPassDescriptor.stencilTestEnabled;
    key.scissorEnable = _scissorState.isEnabled;
    key.needClearStencil = renderPassDescriptor.needClearStencil;
    key.clearStencilValue = renderPassDescriptor.clearStencilValue;
    key.cullMode = _cullMode;
    key.primitiveType = primitiveType;
    Diligent::IPipelineState* pipelineState{ nullptr };
    auto it = piplineStates_.find(key);
    if (it != piplineStates_.end()) {
        pipelineState = it->second;
    } else {
        Diligent::GraphicsPipelineStateCreateInfo PSOCreateInfo;
        PSOCreateInfo.PSODesc.Name = "Effekseer PSO";
        PSOCreateInfo.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

        auto swapChain = _device->GetSwapChain();
        PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = swapChain->GetDesc().ColorBufferFormat;
        PSOCreateInfo.GraphicsPipeline.DSVFormat = swapChain->GetDesc().DepthBufferFormat;
        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = primitiveTypeMap[(uint32_t)primitiveType];

        auto& rasterizerDesc = PSOCreateInfo.GraphicsPipeline.RasterizerDesc;
        rasterizerDesc.ScissorEnable = key.scissorEnable;
        if (key.cullMode == backend::CullMode::BACK) {
            rasterizerDesc.CullMode = Diligent::CULL_MODE_BACK;
        } else if (key.cullMode == backend::CullMode::FRONT) {
            rasterizerDesc.CullMode = Diligent::CULL_MODE_FRONT;
        } else if (key.cullMode == backend::CullMode::NONE) {
            rasterizerDesc.CullMode = Diligent::CULL_MODE_NONE;
        }
        rasterizerDesc.FrontCounterClockwise = true;
        // Depth testing
        auto& depthStencilDesc = PSOCreateInfo.GraphicsPipeline.DepthStencilDesc;
        depthStencilDesc.DepthWriteEnable = false;
//         depthStencilDesc.DepthEnable = true;
//         depthStencilDesc.DepthFunc = Diligent::COMPARISON_FUNCTION::COMPARISON_FUNC_ALWAYS;
        depthStencilDesc.DepthEnable = key.depthTestEnabled;
        if (key.depthTestEnabled) {
            depthStencilDesc.DepthFunc = compareFunciontMap[(uint32_t)_depthStencilDescriptor.depthCompareFunction];
            depthStencilDesc.DepthWriteEnable = _depthStencilDescriptor.depthWriteEnabled;
        }
        depthStencilDesc.StencilEnable = key.stencilTestEnabled;
        if (key.stencilTestEnabled) {
            const auto& frontFace = _depthStencilDescriptor.frontFaceStencil;
            depthStencilDesc.FrontFace = {
                stencilOpMap[(uint32_t)frontFace.stencilFailureOperation],
                stencilOpMap[(uint32_t)frontFace.depthFailureOperation],
                stencilOpMap[(uint32_t)frontFace.depthStencilPassOperation],
                compareFunciontMap[(uint32_t)frontFace.stencilCompareFunction]
            };
            const auto& backFace = _depthStencilDescriptor.frontFaceStencil;
            depthStencilDesc.BackFace = {
                stencilOpMap[(uint32_t)backFace.stencilFailureOperation],
                stencilOpMap[(uint32_t)backFace.depthFailureOperation],
                stencilOpMap[(uint32_t)backFace.depthStencilPassOperation],
                compareFunciontMap[(uint32_t)backFace.stencilCompareFunction]
            };
            depthStencilDesc.StencilReadMask = frontFace.readMask;
            depthStencilDesc.StencilWriteMask = frontFace.writeMask;
        }

        PSOCreateInfo.pVS = currentProgram->_vsShader->GetHandle();
        PSOCreateInfo.pPS = currentProgram->_fsShader->GetHandle();

        auto& inputLayout = PSOCreateInfo.GraphicsPipeline.InputLayout;
        if (commandType == RenderCommand::Type::TRIANGLES_COMMAND) {
            inputLayout.NumElements = _triangleCommandVertexLayout.size();
            inputLayout.LayoutElements = _triangleCommandVertexLayout.data();
        } else if (commandType == RenderCommand::Type::CUSTOM_COMMAND) {
            const auto& elements = static_cast<CustomCommand*>(command)->getLayoutElement();
            inputLayout.NumElements = elements.size();
            inputLayout.LayoutElements = elements.data();
        } else {
            assert(false);
        }

        ea::vector<Diligent::ShaderResourceVariableDesc> Vars;
        ea::vector<Diligent::ImmutableSamplerDesc> ImtblSamplers;
        if (currentProgram->_textureCount != 0) {
    //         Diligent::TEXTURE_ADDRESS_MODE ws[2]{};
    //         ws[(int)Effekseer::TextureWrapType::Clamp] = Diligent::TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_CLAMP;
    //         ws[(int)Effekseer::TextureWrapType::Repeat] = Diligent::TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;
    //         Diligent::FILTER_TYPE fs[2]{};
    //         fs[(int)Effekseer::TextureFilterType::Linear] = Diligent::FILTER_TYPE::FILTER_TYPE_LINEAR;
    //         fs[(int)Effekseer::TextureFilterType::Nearest] = Diligent::FILTER_TYPE::FILTER_TYPE_POINT;
    //         auto filterMode = fs[(int)state.TextureFilterTypes[i]];
    //         auto addressMode = ws[(int)state.TextureWrapTypes[i]];
            Vars.reserve(2);
            ImtblSamplers.reserve(2);
            auto filterMode = Diligent::FILTER_TYPE::FILTER_TYPE_LINEAR;
            auto addressMode = Diligent::TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP; //Diligent::TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_CLAMP;
            Vars.emplace_back(Diligent::SHADER_TYPE_PIXEL, "u_texture", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC/*Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE*/);
            ImtblSamplers.emplace_back(Diligent::SHADER_TYPE_PIXEL, "u_texture", Diligent::SamplerDesc{filterMode, filterMode, filterMode, addressMode, addressMode, addressMode});
            if (currentProgram->_textureCount == 2) {
                Vars.emplace_back(Diligent::SHADER_TYPE_PIXEL, "u_texture1", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC/*Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE*/);
                ImtblSamplers.emplace_back(Diligent::SHADER_TYPE_PIXEL, "u_texture1", Diligent::SamplerDesc{ filterMode, filterMode, filterMode, addressMode, addressMode, addressMode });
            }
            auto& resourceLayout = PSOCreateInfo.PSODesc.ResourceLayout;
            resourceLayout.DefaultVariableType = Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC; 
            resourceLayout.Variables = Vars.data();
            resourceLayout.NumVariables = Vars.size();
            resourceLayout.ImmutableSamplers = ImtblSamplers.data();
            resourceLayout.NumImmutableSamplers = ImtblSamplers.size();
        }

        PSOCreateInfo.GraphicsPipeline.BlendDesc.IndependentBlendEnable = false;
        auto& blendDesc = PSOCreateInfo.GraphicsPipeline.BlendDesc.RenderTargets[0];
        blendDesc.BlendEnable = key.blendDescriptor.blendEnabled;
        if (key.blendDescriptor.blendEnabled) {
            blendDesc.SrcBlend = blendFactorMap[(uint32_t)key.blendDescriptor.sourceRGBBlendFactor];
            blendDesc.DestBlend = blendFactorMap[(uint32_t)key.blendDescriptor.destinationRGBBlendFactor];
            blendDesc.BlendOp = blendOpMap[(uint32_t)key.blendDescriptor.rgbBlendOperation];
            blendDesc.SrcBlendAlpha = blendFactorMap[(uint32_t)key.blendDescriptor.sourceAlphaBlendFactor];
            blendDesc.DestBlendAlpha = blendFactorMap[(uint32_t)key.blendDescriptor.destinationAlphaBlendFactor];
            blendDesc.BlendOpAlpha = blendOpMap[(uint32_t)key.blendDescriptor.alphaBlendOperation];
            blendDesc.RenderTargetWriteMask = (Diligent::COLOR_MASK)key.blendDescriptor.writeMask;
        }

        _device->GetRenderDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &pipelineState);
        auto srv = pipelineState->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "VSConstants");
        if (srv) {
            srv->Set(currentProgram->_vsConstants);
        }
        srv = pipelineState->GetStaticVariableByName(Diligent::SHADER_TYPE_PIXEL, "PSConstants");
        if (srv) {
            srv->Set(currentProgram->_fsConstants);
        }
        
        auto& srbinfo = shaderResourceBindings_[pipelineState];
        pipelineState->CreateShaderResourceBinding(&srbinfo.shaderResourceBinding, true);
        auto textureCount = currentProgram->_textureCount;
        if (textureCount > 0) {
            srbinfo.shaderResourceVariables.reserve(textureCount);
            for (int i = 0; i < textureCount; i++) {
                srbinfo.shaderResourceVariables.emplace_back(srbinfo.shaderResourceBinding->GetVariableByName(
                    Diligent::SHADER_TYPE_PIXEL, (i == 0) ? "u_texture" : "u_texture1"));
            }
        }
        piplineStates_.insert({ key, Diligent::RefCntAutoPtr<Diligent::IPipelineState>{pipelineState} });
    }

    commitUniformAndTextures(pipelineDescriptor, pipelineState);
    return pipelineState;
}

void Renderer::beginRenderPass(RenderCommand* cmd)
{
//      _commandBuffer->beginRenderPass(_renderPassDescriptor);
//      _commandBuffer->setViewport(_viewport.x, _viewport.y, _viewport.w, _viewport.h);
//      _commandBuffer->setCullMode(_cullMode);
//      _commandBuffer->setWinding(_winding);
//      _commandBuffer->setScissorRect(_scissorState.isEnabled, _scissorState.rect.x, _scissorState.rect.y, _scissorState.rect.width, _scissorState.rect.height);
//      _commandBuffer->setStencilReferenceValue(_stencilRef);
    auto* pipelineState = getOrCreateRenderPipeline(cmd, _renderPassDescriptor);

    auto pCtx = _device->GetImmediateContext();
    const auto& SCDesc = _device->GetSwapChain()->GetDesc();
    Diligent::Viewport vp{ (float)_viewport.x, (float)_viewport.y, (float)_viewport.w, (float)_viewport.h };
    pCtx->SetViewports(1, &vp, SCDesc.Width, SCDesc.Height);
    if (_scissorState.isEnabled) {
//         Diligent::Rect Scissor{
//             (int32_t)_scissorState.rect.x,
//             (int32_t)_scissorState.rect.y,
//             (int32_t)_scissorState.rect.x + (int32_t)_scissorState.rect.width,
//             (int32_t)_scissorState.rect.y + (int32_t)_scissorState.rect.height,
//         };
        // convert coord    *           to (0,0) * * *
        //                  *              *
        //                  *              *
        //                  (0,0) * * *    *
        Diligent::Rect Scissor{
            (int32_t)_scissorState.rect.x,
            (int32_t)SCDesc.Height - ((int32_t)_scissorState.rect.y + (int32_t)_scissorState.rect.height),
            (int32_t)_scissorState.rect.x + (int32_t)_scissorState.rect.width,
            (int32_t)SCDesc.Height - (int32_t)_scissorState.rect.y,
        };
        pCtx->SetScissorRects(1, &Scissor, SCDesc.Width, SCDesc.Height);
    }
    pCtx->SetStencilRef(_stencilRef);
    pCtx->SetPipelineState(pipelineState);
    Diligent::IShaderResourceBinding* srb = shaderResourceBindings_[pipelineState].shaderResourceBinding;
    if (srb) {
        pCtx->CommitShaderResources(srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }
}

float Renderer::getClearDepth() const
{
    return _renderPassDescriptor.clearDepthValue;
}

unsigned int Renderer::getClearStencil() const
{
    return _renderPassDescriptor.clearStencilValue;
}

void Renderer::setScissorTest(bool enabled)
{
    _scissorState.isEnabled = enabled;
}

bool Renderer::getScissorTest() const
{
    return _scissorState.isEnabled;
}

const ScissorRect& Renderer::getScissorRect() const
{
    return _scissorState.rect;
}

void Renderer::setScissorRect(float x, float y, float width, float height)
{
    _scissorState.rect.x = x;
    _scissorState.rect.y = y;
    _scissorState.rect.width = width;
    _scissorState.rect.height = height;
}

// TriangleCommandBufferManager
Renderer::TriangleCommandBufferManager::~TriangleCommandBufferManager()
{
//     for (auto& vertexBuffer : _vertexBufferPool)
//         vertexBuffer->release();
// 
//     for (auto& indexBuffer : _indexBufferPool)
//         indexBuffer->release();
}

void Renderer::TriangleCommandBufferManager::init(Urho3D::RenderDevice* device)
{
    _device = device;
    createBuffer();
}

void Renderer::TriangleCommandBufferManager::putbackAllBuffers()
{
    _currentBufferIndex = 0;
}

void Renderer::TriangleCommandBufferManager::prepareNextBuffer()
{
    if (_currentBufferIndex < (int)_vertexBufferPool.size() - 1)
    {
        ++_currentBufferIndex;
        return;
    }

    createBuffer();
    ++_currentBufferIndex;
}

Diligent::IBuffer* Renderer::TriangleCommandBufferManager::getVertexBuffer() const
{
    return _vertexBufferPool[_currentBufferIndex];
}

Diligent::IBuffer* Renderer::TriangleCommandBufferManager::getIndexBuffer() const
{
    return _indexBufferPool[_currentBufferIndex];
}

void Renderer::TriangleCommandBufferManager::createBuffer()
{
//     auto device = backend::Device::getInstance();
// 
// #ifdef CC_USE_METAL
//     // Metal doesn't need to update buffer to make sure it has the correct size.
//     auto vertexBuffer = device->newBuffer(Renderer::VBO_SIZE * sizeof(_verts[0]), backend::BufferType::VERTEX, backend::BufferUsage::DYNAMIC);
//     if (!vertexBuffer)
//         return;
// 
//     auto indexBuffer = device->newBuffer(Renderer::INDEX_VBO_SIZE * sizeof(_indices[0]), backend::BufferType::INDEX, backend::BufferUsage::DYNAMIC);
//     if (!indexBuffer)
//     {
//         vertexBuffer->release();
//         return;
//     }
// #else
//     auto tmpData = malloc(Renderer::VBO_SIZE * sizeof(V3F_C4B_T2F));
//     if (!tmpData)
//         return;

    //auto vertexBuffer = device->newBuffer(Renderer::VBO_SIZE * sizeof(V3F_C4B_T2F), backend::BufferType::VERTEX, backend::BufferUsage::DYNAMIC);
    Diligent::IBuffer* vertexBuffer{ nullptr };
    Diligent::BufferDesc VertBuffDesc;
    VertBuffDesc.Name = "Cocos vertex buffer";
    VertBuffDesc.BindFlags = Diligent::BIND_VERTEX_BUFFER;
    VertBuffDesc.Usage = Diligent::USAGE_DYNAMIC;
    VertBuffDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
    VertBuffDesc.Size = Renderer::VBO_SIZE * sizeof(V3F_C4B_T2F);
    _device->GetRenderDevice()->CreateBuffer(VertBuffDesc, nullptr, &vertexBuffer);
    if (!vertexBuffer) {
//        free(tmpData);
        return;
    }
    //vertexBuffer->updateData(tmpData, Renderer::VBO_SIZE * sizeof(V3F_C4B_T2F));
    Diligent::BufferDesc IndBuffDesc;
    IndBuffDesc.Name = "Cocos index buffer";
    IndBuffDesc.BindFlags = Diligent::BIND_INDEX_BUFFER;
    IndBuffDesc.Usage = Diligent::USAGE_DYNAMIC;
    IndBuffDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
    IndBuffDesc.Size = Renderer::INDEX_VBO_SIZE * sizeof(uint16_t);
    //auto indexBuffer = device->newBuffer(Renderer::INDEX_VBO_SIZE * sizeof(unsigned short), backend::BufferType::INDEX, backend::BufferUsage::DYNAMIC);
    Diligent::IBuffer* indexBuffer{ nullptr };
    _device->GetRenderDevice()->CreateBuffer(IndBuffDesc, nullptr, &indexBuffer);
    if (!indexBuffer) {
//         free(tmpData);
//         vertexBuffer->release();
        return;
    }
//     indexBuffer->updateData(tmpData, Renderer::INDEX_VBO_SIZE * sizeof(unsigned short));
// 
//     free(tmpData);
// #endif

    _vertexBufferPool.push_back(vertexBuffer);
    _indexBufferPool.push_back(indexBuffer);
}

void Renderer::pushStateBlock()
{
    StateBlock block;
    block.depthTest = getDepthTest();
    block.depthWrite = getDepthWrite();
    block.cullMode = getCullMode();
    _stateBlockStack.emplace_back(block);
}

void Renderer::popStateBlock()
{
    auto & block = _stateBlockStack.back();
    setDepthTest(block.depthTest);
    setDepthWrite(block.depthWrite);
    setCullMode(block.cullMode);
    _stateBlockStack.pop_back();
}

NS_CC_END