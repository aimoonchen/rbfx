/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

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
 
#include "Program.h"
#include <map>
#include "ProgramCache.h"
#include "base/CCDirector.h"
#include "renderer/CCRenderer.h"
#include "renderer/ccShaders.h"
#include <Diligent/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Diligent/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include "../../../RenderAPI/RenderDevice.h"
#include "../../../Resource/ResourceCache.h"
CC_BACKEND_BEGIN

static Diligent::RefCntAutoPtr<Diligent::IBuffer> create_uniform_buffer(const char* name, int32_t size)
{
    Diligent::BufferDesc CBDesc;
    CBDesc.Name = name;
    CBDesc.Size = size;
    CBDesc.Usage = Diligent::USAGE_DYNAMIC;
    CBDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
    CBDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;

    auto device = Director::getInstance()->getRenderer()->GetRenderDevice();
    Diligent::RefCntAutoPtr<Diligent::IBuffer> pBuffer;
    device->GetRenderDevice()->CreateBuffer(CBDesc, nullptr, &pBuffer);
    return pBuffer;
}

std::unordered_map<const char*, Diligent::IShader*> Program::_cachedShaders;

Program::Program(const char* vsfile, const char* fsfile, ProgramType programType)
    : _programType{ programType }
{
    vsName = vsfile;
    fsName = fsfile;
    auto renderer = Director::getInstance()->getRenderer();
    auto device = renderer->GetRenderDevice();
    auto cache = device->GetSubsystem<Urho3D::ResourceCache>();
    auto get_source_code = [cache](const char* filename, ea::string& sourceCode) {
        auto shaderPath = ea::string{ "Shaders/HLSL/fairygui/" } + filename;
        auto pFile = cache->GetFile(shaderPath);
        unsigned dataSize = pFile->GetSize();
        sourceCode.resize(dataSize + 1);
        sourceCode[dataSize] = '\0';
        if (pFile->Read(sourceCode.data(), dataSize) != dataSize)
            return false;
        return true;
        };

    Diligent::ShaderCreateInfo ShaderCI;
    ShaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
    ShaderCI.Desc.UseCombinedTextureSamplers = true;
    ea::string sourceCode;
    // Create a vertex shader
    if (auto it = _cachedShaders.find(vsfile); it == _cachedShaders.end()) {
        if (get_source_code(vsfile, sourceCode)) {
            ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_VERTEX;
            ShaderCI.EntryPoint = "main";
            ShaderCI.Desc.Name = vsfile;
            ShaderCI.Source = sourceCode.data();
            device->GetRenderDevice()->CreateShader(ShaderCI, &_cachedShaders[vsfile]);
        }
    }
    _vsShader = _cachedShaders[vsfile];

    // Create a pixel shader
    if (auto it = _cachedShaders.find(fsfile); it == _cachedShaders.end()) {
        if (get_source_code(fsfile, sourceCode)) {
            ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
            ShaderCI.EntryPoint = "main";
            ShaderCI.Desc.Name = fsfile;
            ShaderCI.Source = sourceCode.data();
            device->GetRenderDevice()->CreateShader(ShaderCI, &_cachedShaders[fsfile]);
        }
    }
    _fsShader = _cachedShaders[fsfile];

    if (fsfile == positionColor_frag
        || fsfile == positionUColor_frag
        || fsfile == lineColor3D_frag
        || fsfile == layer_radialGradient_frag
        || fsfile == positionColorLengthTexture_frag)
    {
        _textureCount = 0;
    }
    else if (fsfile == etc1_frag
        || fsfile == etc1Gray_frag)
    {
        _textureCount = 2;
    }
    else {
        _textureCount = 1;
    }
    /// u_MVPMatrix
    _builtinUniformLocation[Uniform::MVP_MATRIX].location[0] = 0;
    /// u_texture
    _builtinUniformLocation[Uniform::TEXTURE].location[1] = 0;
    _builtinUniformLocation[Uniform::TEXTURE].shaderStage = ShaderStage::FRAGMENT;
    /// u_texture1
    _builtinUniformLocation[Uniform::TEXTURE1].location[1] = 1;
    _builtinUniformLocation[Uniform::TEXTURE1].shaderStage = ShaderStage::FRAGMENT;
    _builtinUniformLocation[Uniform::EFFECT_COLOR].shaderStage = ShaderStage::FRAGMENT;
    _builtinUniformLocation[Uniform::TEXT_COLOR].shaderStage = ShaderStage::FRAGMENT;
    _builtinUniformLocation[Uniform::EFFECT_TYPE].shaderStage = ShaderStage::FRAGMENT;

    if (vsfile == positionColorLengthTexture_vert || vsfile == positionColorTextureAsPointsize_vert) {
        // float4x4 u_MVPMatrix;
        // float u_alpha;
        _vsConstants = create_uniform_buffer("VSConstants", sizeof(float) * 17);
        auto ret = _customUniform.insert({ "u_alpha", {} });
        ret.first->second.location[0] = sizeof(float) * 16;
    } else if (vsfile == positionUColor_vert) {
        // float4x4 u_MVPMatrix;
        // float4 u_color;
        _vsConstants = create_uniform_buffer("VSConstants", sizeof(float) * 20);
        auto ret = _customUniform.insert({ "u_color", {} });
        ret.first->second.location[0] = sizeof(float) * 16;
    } else {
        // float4x4 u_MVPMatrix;
        _vsConstants = create_uniform_buffer("VSConstants", sizeof(float) * 16);
    }
    
    if (fsfile == positionTextureColorAlphaTest_frag) {
        // float u_alpha_value;
        _fsConstants = create_uniform_buffer("PSConstants", sizeof(float));
        auto ret = _customUniform.insert({ "u_alpha_value", {} });
        ret.first->second.shaderStage = ShaderStage::FRAGMENT;
        ret.first->second.location[1] = 0;
    } else if (fsfile == label_normal_frag || fsfile == label_distanceNormal_frag) {
        // float4 u_textColor;
        _fsConstants = create_uniform_buffer("PSConstants", sizeof(float) * 4);
        _builtinUniformLocation[Uniform::TEXT_COLOR].location[1] = 0;
    } else if (fsfile == labelOutline_frag) {
        // float4 u_effectColor;
        // float4 u_textColor;
        // int u_effectType;
        _fsConstants = create_uniform_buffer("PSConstants", sizeof(float) * 8 + sizeof(int));
        _builtinUniformLocation[Uniform::EFFECT_COLOR].location[1] = 0;
        _builtinUniformLocation[Uniform::TEXT_COLOR].location[1] = sizeof(float) * 4;
        _builtinUniformLocation[Uniform::EFFECT_TYPE].location[1] = sizeof(float) * 8;
    } else if (fsfile == labelDistanceFieldGlow_frag) {
        // float4 u_effectColor;
        // float4 u_textColor;
        _fsConstants = create_uniform_buffer("PSConstants", sizeof(float) * 8);
        _builtinUniformLocation[Uniform::EFFECT_COLOR].location[1] = 0;
        _builtinUniformLocation[Uniform::TEXT_COLOR].location[1] = sizeof(float) * 4;
    } else if (fsfile == layer_radialGradient_frag) {
        // float4 u_startColor;
        // float4 u_endColor;
        // float2 u_center;
        // float u_radius;
        // float u_expand;
        _fsConstants = create_uniform_buffer("PSConstants", sizeof(float) * 12);
        auto ret = _customUniform.insert({ "u_startColor", {} });
        auto& u_startColor = ret.first->second;
        u_startColor.shaderStage = ShaderStage::FRAGMENT;
        u_startColor.location[1] = 0;
        ret = _customUniform.insert({ "u_endColor", {} });
        auto& u_endColor = ret.first->second;
        u_endColor.shaderStage = ShaderStage::FRAGMENT;
        u_endColor.location[1] = sizeof(float) * 4;
        ret = _customUniform.insert({ "u_center", {} });
        auto& u_center = ret.first->second;
        u_center.shaderStage = ShaderStage::FRAGMENT;
        u_center.location[1] = sizeof(float) * 8;
        ret = _customUniform.insert({ "u_radius", {} });
        auto& u_radius = ret.first->second;
        u_radius.shaderStage = ShaderStage::FRAGMENT;
        u_radius.location[1] = sizeof(float) * 10;
        ret = _customUniform.insert({ "u_expand", {} });
        auto& u_expand = ret.first->second;
        u_expand.shaderStage = ShaderStage::FRAGMENT;
        u_expand.location[1] = sizeof(float) * 11;
    } else if (fsfile == positionTextureUColor_frag) {
        // float4 u_color;
        _fsConstants = create_uniform_buffer("PSConstants", sizeof(float) * 4);
        auto ret = _customUniform.insert({ "u_color", {} });
        ret.first->second.shaderStage = ShaderStage::FRAGMENT;
        ret.first->second.location[1] = 0;
    }
}

std::size_t Program::getUniformBufferSize(ShaderStage stage) const
{
    if (stage == ShaderStage::VERTEX) {
        if (vsName == positionColorLengthTexture_vert
            || vsName == positionColorTextureAsPointsize_vert) {
            // float4x4 u_MVPMatrix;
            // float u_alpha;
            return sizeof(float) * 17;
        } else if (vsName == positionUColor_vert) {
            // float4x4 u_MVPMatrix;
            // float4 u_color;
            return sizeof(float) * 20;
        } else {
            // float4x4 u_MVPMatrix;
            return sizeof(float) * 16;
        }
    } else if (stage == ShaderStage::FRAGMENT) {
        if (fsName == labelOutline_frag) {
            // float4 u_effectColor;
            // float4 u_textColor;
            // int u_effectType;
            return sizeof(float) * 8 + sizeof(int);
        } else if (fsName == labelDistanceFieldGlow_frag) {
            // float4 u_effectColor;
            // float4 u_textColor;
            return sizeof(float) * 8;
        } else if (fsName == label_normal_frag
            || fsName == label_distanceNormal_frag) {
            // float4 u_textColor;
            return sizeof(float) * 4;
        } else if (fsName == layer_radialGradient_frag) {
            // float4 u_startColor;
            // float4 u_endColor;
            // float2 u_center;
            // float u_radius;
            // float u_expand;
            return sizeof(float) * 12;
        } else if (fsName == positionTextureUColor_frag) {
            // float4 u_color;
            return sizeof(float) * 4;
        } else if (fsName == positionTextureColorAlphaTest_frag) {
            // float u_alpha_value;
            return sizeof(float);
        }
    }
    return 0;
}

UniformLocation Program::getUniformLocation(const std::string& uniformName) const {
    static std::map<std::string, Uniform> uniformMap {
        {"u_MVPMatrix", Uniform::MVP_MATRIX},
        {"u_texture", Uniform::TEXTURE},
        {"u_texture1", Uniform::TEXTURE1},
        {"u_textColor", Uniform::TEXT_COLOR},
        {"u_effectType", Uniform::EFFECT_TYPE},
        {"u_effectColor", Uniform::EFFECT_COLOR},
    };
    auto mapit = uniformMap.find(uniformName);
    if (mapit != uniformMap.end()) {
        return getUniformLocation(mapit->second);
    }
    auto it = _customUniform.find(uniformName);
    if (it != _customUniform.end()) {
        return it->second;
    }
    assert(false);
    return {};
}

UniformLocation Program::getUniformLocation(backend::Uniform name) const
{
    return _builtinUniformLocation[name];
}

void Program::applyUniformBuffer(uint8_t* buffer, Urho3D::Texture2D* textures[])
{
//     for (auto& it : _activeUniform)
//     {
//         const auto& uniform = it.second;
//         if (uniform.sampler)
//         {
//             graphics_->SetTexture(uniform.stage, textures[uniform.stage]);
//         }
//         else
//         {
//             graphics_->SetShaderParameter(uniform.handle, (const float*)(buffer + uniform.offset), uniform.count);
//         }
//     }
}

Program* Program::getBuiltinProgram(ProgramType type)
{
    return ProgramCache::getInstance()->getBuiltinProgram(type);
}

Program* Program::getCustomProgram(const char* vsfile, const char* fsfile)
{
    return ProgramCache::getInstance()->getCustomProgram(vsfile, fsfile);
}

CC_BACKEND_END
