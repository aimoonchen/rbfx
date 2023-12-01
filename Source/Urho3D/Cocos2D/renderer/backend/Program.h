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
 
#pragma once

#include "Macros.h"
#include "base/CCRef.h"
#include "platform/CCPlatformMacros.h"
#include "Types.h"
//#include "ShaderCache.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <Diligent/Common/interface/RefCntAutoPtr.hpp>
namespace Urho3D
{
class Texture2D;
class RenderDevice;
} // namespace Urho3D
namespace Diligent
{
struct IShader;
struct IBuffer;
struct IShaderResourceBinding;
} // namespace Diligent

CC_BACKEND_BEGIN

class ShaderModule;

/**
 * @addtogroup _backend
 * @{
 */

enum Uniform : uint32_t
{
    MVP_MATRIX,
    TEXTURE,
    TEXTURE1,
    TEXTURE2,
    TEXTURE3,
    TEXT_COLOR,
    EFFECT_TYPE,
    EFFECT_COLOR,
    UNIFORM_MAX // Maximum uniforms
};

enum Attribute : uint32_t
{
    POSITION,
    COLOR,
    TEXCOORD,
    TEXCOORD1,
    TEXCOORD2,
    TEXCOORD3,
    ATTRIBUTE_MAX // Maximum attributes
};

/**
 * A program.
 */
class Program : public Ref
{
public:
    /**
     * Get engine built-in program.
     * @param type Specifies the built-in program type.
     */
    static Program* getBuiltinProgram(ProgramType type);
    
    /**
     * Get uniform location by name.
     * @param uniform Specifies the uniform name.
     * @return The uniform location.
     */
    virtual UniformLocation getUniformLocation(const std::string& uniform) const;

    /**
     * Get uniform location by engine built-in uniform enum name.
     * @param name Specifies the engine built-in uniform enum name.
     * @return The uniform location.
     */
    virtual UniformLocation getUniformLocation(backend::Uniform name) const;

    /**
     * Get attribute location by attribute name.
     * @param name Specifies the attribute name.
     * @return The attribute location.
     */
    //virtual int getAttributeLocation(const std::string& name) const =  0;

    /**
     * Get attribute location by engine built-in attribute enum name.
     * @param name Specifies the engine built-in attribute enum name.
     * @return The attribute location.
     */
    //virtual int getAttributeLocation(backend::Attribute name) const =  0;
    
    /**
     * Get maximum vertex location.
     * @return Maximum vertex locaiton.
     */
    //virtual int getMaxVertexLocation() const = 0;

    /**
     * Get maximum fragment location.
     * @return Maximum fragment location.
     */
    //virtual int getMaxFragmentLocation() const = 0;

    /**
     * Get active vertex attributes.
     * @return Active vertex attributes. key is active attribute name, Value is corresponding attribute info.
     */
    //virtual const std::unordered_map<std::string, AttributeBindInfo> getActiveAttributes() const = 0;

    /**
     * Get vertex shader.
     * @return Vertex shader.
     */
    //const std::string& getVertexShader() const { return _vertexShader; }

    /**
     * Get fragment shader.
     * @ Fragment shader.
     */
    //const std::string& getFragmentShader() const { return _fragmentShader; }
    
    /**
     * Get engine built-in program type.
     * @return The built-in program type.
     */
    ProgramType getProgramType() const { return _programType; }

    /**
     * Get uniform buffer size in bytes that can hold all the uniforms.
     * @param stage Specifies the shader stage. The symbolic constant can be either VERTEX or FRAGMENT.
     * @return The uniform buffer size in bytes.
     */
    virtual std::size_t getUniformBufferSize(ShaderStage stage) const;

    /**
     * Get a uniformInfo in given location from the specific shader stage.
     * @param stage Specifies the shader stage. The symbolic constant can be either VERTEX or FRAGMENT.
     * @param location Specifies the uniform locaion.
     * @return The uniformInfo.
     */
    //virtual const UniformInfo& getActiveUniformInfo(ShaderStage stage, int location) const = 0;

    /**
     * Get all uniformInfos.
     * @return The uniformInfos.
     */
    //virtual const std::unordered_map<std::string, UniformInfo>& getAllActiveUniformInfo(ShaderStage stage) const = 0;
    void applyUniformBuffer(uint8_t* buffer, Urho3D::Texture2D* textures[]);
//protected:
    Program(const char* vsfile, const char* fsfile, ProgramType programType = ProgramType::CUSTOM_PROGRAM);
    // for debug
    const char* vsName = nullptr;
    const char* fsName = nullptr;
    //
    ProgramType _programType = ProgramType::CUSTOM_PROGRAM; ///< built-in program type, initial value is CUSTOM_PROGRAM.
    Urho3D::RenderDevice* _device{ nullptr };
    Diligent::IShader* _vsShader{ nullptr };
    Diligent::IShader* _fsShader{ nullptr };
    Diligent::RefCntAutoPtr<Diligent::IBuffer>  _vsConstants{ nullptr };
    Diligent::RefCntAutoPtr<Diligent::IBuffer>  _fsConstants{ nullptr };
    Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> _shaderResourceBinding{ nullptr };
    UniformLocation _builtinUniformLocation[UNIFORM_MAX];
    std::unordered_map<std::string, UniformLocation> _customUniform;
    std::size_t _textureCount = 0;
    static std::unordered_map<const char*, Diligent::IShader*> _cachedShaders;
    friend class ProgramCache;
};

//end of _backend group
/// @}
CC_BACKEND_END
