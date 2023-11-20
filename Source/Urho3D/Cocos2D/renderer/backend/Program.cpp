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
#include <Diligent/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Diligent/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include "../../../RenderAPI/RenderDevice.h"
#include "../../../Resource/ResourceCache.h"
CC_BACKEND_BEGIN

Program::Program(Urho3D::RenderDevice* renderDevice, const ea::string& vs, const ea::string& fs)
    : _device{ renderDevice }
    , _vertexShader(vs)
    , _fragmentShader(fs)
{
    auto cache = _device->GetSubsystem<Urho3D::ResourceCache>();
    Diligent::ShaderCreateInfo ShaderCI;
    ShaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_GLSL;
    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;
    ea::string shaderName = fs.substr(0, fs.find_last_of("_frag"));
    // Create a vertex shader
    ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_VERTEX;
    ShaderCI.EntryPoint = "main";
    ShaderCI.Desc.Name = shaderName.data();
    ShaderCI.Source = vs.data();
    renderDevice->GetRenderDevice()->CreateShader(ShaderCI, &_vsShader);
    // Create a pixel shader
    ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
    ShaderCI.EntryPoint = "main";
    ShaderCI.Desc.Name = shaderName.data();
    ShaderCI.Source = fs.data();
    renderDevice->GetRenderDevice()->CreateShader(ShaderCI, &_fsShader);
}

UniformLocation Program::getUniformLocation(backend::Uniform name) const
{
    if (_builtinUniformLocation[name])
    {
        return *_builtinUniformLocation[name];
    }
    else
    {
        // assert(false);
        return {};
    }
}

void Program::applyUniformBuffer(uint8_t* buffer, Urho3D::Texture2D* textures[])
{
    for (auto& it : _activeUniform)
    {
        const auto& uniform = it.second;
//         if (uniform.sampler)
//         {
//             graphics_->SetTexture(uniform.stage, textures[uniform.stage]);
//         }
//         else
//         {
//             graphics_->SetShaderParameter(uniform.handle, (const float*)(buffer + uniform.offset), uniform.count);
//         }
    }
}

void Program::setProgramType(ProgramType type)
{
    _programType = type;
}

Program* Program::getBuiltinProgram(ProgramType type)
{
    return ProgramCache::getInstance()->getBuiltinProgram(type);
}

CC_BACKEND_END
