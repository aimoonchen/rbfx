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
#include "ProgramCache.h"
#include "base/CCDirector.h"
#include "renderer/CCRenderer.h"
#include <Diligent/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Diligent/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include "../../../RenderAPI/RenderDevice.h"
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

Program::Program(const std::string& vs, const std::string& fs, ProgramType programType)
    : _programType{ programType }
{
    auto renderer = Director::getInstance()->getRenderer();
    auto device = renderer->GetRenderDevice();
    Diligent::ShaderCreateInfo ShaderCI;
    ShaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
    ShaderCI.Desc.UseCombinedTextureSamplers = true;
    //auto shaderName = fs.substr(0, fs.find_last_of("_frag"));
    auto shaderName = std::string{ "FairyGUI VS" } + std::to_string((int)programType);
    // Create a vertex shader
    ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_VERTEX;
    ShaderCI.EntryPoint = "main";
    ShaderCI.Desc.Name = shaderName.c_str();
    ShaderCI.Source = vs.data();
    device->GetRenderDevice()->CreateShader(ShaderCI, &_vsShader);
    shaderName = std::string{ "FairyGUI PS" } + std::to_string((int)programType);
    // Create a pixel shader
    ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
    ShaderCI.EntryPoint = "main";
    ShaderCI.Desc.Name = shaderName.c_str();
    ShaderCI.Source = fs.data();
    device->GetRenderDevice()->CreateShader(ShaderCI, &_fsShader);

    
    if (programType == ProgramType::POSITION
        || programType == ProgramType::POSITION_COLOR
        || programType == ProgramType::POSITION_UCOLOR
        || programType == ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE
        || programType == ProgramType::POSITION_COLOR_LENGTH_TEXTURE)
    {
        _textureCount = 0;
    } else if (programType == ProgramType::ETC1
        || programType == ProgramType::ETC1_GRAY)
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

    if (programType != ProgramType::POSITION_COLOR_LENGTH_TEXTURE
        && programType != ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE
        && programType != ProgramType::POSITION_UCOLOR) {
        // float4x4 u_MVPMatrix;
        _vsConstants = create_uniform_buffer("VSConstants", sizeof(float) * 16);
    }
    switch (programType) {
    case ProgramType::POSITION_COLOR_LENGTH_TEXTURE:
    case ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE:
        // float4x4 u_MVPMatrix;
        // float u_alpha;
        _vsConstants = create_uniform_buffer("VSConstants", sizeof(float) * 17);
        _customUniform.insert({ "u_alpha", {} });
        break;
    case ProgramType::POSITION_UCOLOR:
        // float4x4 u_MVPMatrix;
        // float4 u_color;
        _vsConstants = create_uniform_buffer("VSConstants", sizeof(float) * 20);
        _customUniform.insert({ "u_color", {} });
        break;
    case ProgramType::POSITION_TEXTURE_COLOR_ALPHA_TEST:
        // float u_alpha_value;
        _fsConstants = create_uniform_buffer("PSConstants", sizeof(float));
        _customUniform.insert({ "u_alpha_value", {} });
        break;
    case ProgramType::LABEL_NORMAL:
    case ProgramType::LABEL_DISTANCE_NORMAL:
        // float4 u_textColor;
        _fsConstants = create_uniform_buffer("PSConstants", sizeof(float) * 4);
        _builtinUniformLocation[Uniform::TEXT_COLOR].location[1] = 0;
        break;
    case ProgramType::LABLE_OUTLINE:
        // float4 u_effectColor;
        // float4 u_textColor;
        // int u_effectType;
        _fsConstants = create_uniform_buffer("PSConstants", sizeof(float) * 8 + sizeof(int));
        _builtinUniformLocation[Uniform::EFFECT_COLOR].location[1] = 0;
        _builtinUniformLocation[Uniform::TEXT_COLOR].location[1] = sizeof(float) * 4;
        _builtinUniformLocation[Uniform::EFFECT_TYPE].location[1] = sizeof(float) * 8;
        break;
    case ProgramType::LABLE_DISTANCEFIELD_GLOW:
        // float4 u_effectColor;
        // float4 u_textColor;
        _fsConstants = create_uniform_buffer("PSConstants", sizeof(float) * 8);
        _builtinUniformLocation[Uniform::EFFECT_COLOR].location[1] = 0;
        _builtinUniformLocation[Uniform::TEXT_COLOR].location[1] = sizeof(float) * 4;
        break;
    case ProgramType::LAYER_RADIA_GRADIENT:
        // float4 u_startColor;
        // float4 u_endColor;
        // float2 u_center;
        // float u_radius;
        // float u_expand;
        _fsConstants = create_uniform_buffer("PSConstants", sizeof(float) * 12);
        _customUniform.insert({ "u_startColor", {} });
        _customUniform.insert({ "u_endColor", {} });
        _customUniform.insert({ "u_center", {} });
        _customUniform.insert({ "u_radius", {} });
        _customUniform.insert({ "u_expand", {} });
        break;
    case ProgramType::CAMERA_CLEAR:
        _builtinUniformLocation[Uniform::MVP_MATRIX].location[0] = -1;
        break;
    default:
        break;
    }
}

std::size_t Program::getUniformBufferSize(ShaderStage stage) const
{
    if (stage == ShaderStage::VERTEX) {
        if (_programType == ProgramType::POSITION_COLOR_LENGTH_TEXTURE
            || _programType == ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE) {
            // float4x4 u_MVPMatrix;
            // float u_alpha;
            return sizeof(float) * 17;
        } else if (_programType == ProgramType::POSITION_UCOLOR) {
            // float4x4 u_MVPMatrix;
            // float4 u_color;
            return sizeof(float) * 20;
        } else if (_programType == ProgramType::CAMERA_CLEAR) {
            // float depth;
            return sizeof(float);
        } else {
            // float4x4 u_MVPMatrix;
            return sizeof(float) * 16;
        }
    } else if (stage == ShaderStage::FRAGMENT) {
        if (_programType == ProgramType::LABLE_OUTLINE) {
            // float4 u_effectColor;
            // float4 u_textColor;
            // int u_effectType;
            return sizeof(float) * 8 + sizeof(int);
        } else if (_programType == ProgramType::LABLE_DISTANCEFIELD_GLOW) {
            // float4 u_effectColor;
            // float4 u_textColor;
            return sizeof(float) * 8;
        } else if (_programType == ProgramType::LABEL_NORMAL
            || _programType == ProgramType::LABEL_DISTANCE_NORMAL) {
            // float4 u_textColor;
            return sizeof(float) * 4;
        } else if (_programType == ProgramType::LAYER_RADIA_GRADIENT) {
            // float4 u_startColor;
            // float4 u_endColor;
            // float2 u_center;
            // float u_radius;
            // float u_expand;
            return sizeof(float) * 12;
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

CC_BACKEND_END
