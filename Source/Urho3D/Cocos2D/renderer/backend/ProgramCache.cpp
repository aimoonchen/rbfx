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
 
#include "ProgramCache.h"
#include "renderer/ccShaders.h"
#include "renderer/CCRenderer.h"
#include "base/ccMacros.h"
#include "base/CCDirector.h"
namespace std
{
    template <>
    struct hash<cocos2d::backend::ProgramType>
    {
        typedef cocos2d::backend::ProgramType argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& v) const
        {
            return hash<int>()(static_cast<int>(v));
        }
    };
}

CC_BACKEND_BEGIN

std::unordered_map<backend::ProgramType, backend::Program*>  ProgramCache::_cachedPrograms;
std::unordered_map<uint64_t, backend::Program*> ProgramCache::_cachedCustomPrograms;

ProgramCache* ProgramCache::_sharedProgramCache = nullptr;

ProgramCache* ProgramCache::getInstance()
{
    if(!_sharedProgramCache)
    {
        _sharedProgramCache = new (std::nothrow) ProgramCache();
        if(!_sharedProgramCache->init())
        {
            CC_SAFE_RELEASE(_sharedProgramCache);
        }
    }
    return _sharedProgramCache;
}

void ProgramCache::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedProgramCache);
}

ProgramCache::~ProgramCache()
{
    for(auto& program : _cachedPrograms)
    {
        CC_SAFE_RELEASE(program.second);
    }
    CCLOGINFO("deallocing ProgramCache: %p", this);
    //ShaderCache::destroyInstance();
}

bool ProgramCache::init()
{
    auto renderer = Director::getInstance()->getRenderer();
    _device = renderer->GetRenderDevice();
    addProgram(ProgramType::POSITION_TEXTURE_COLOR);
    addProgram(ProgramType::ETC1);
    addProgram(ProgramType::LABEL_DISTANCE_NORMAL);
    addProgram(ProgramType::LABEL_NORMAL);
    addProgram(ProgramType::LABLE_OUTLINE);
    addProgram(ProgramType::LABLE_DISTANCEFIELD_GLOW);
    addProgram(ProgramType::POSITION_COLOR_LENGTH_TEXTURE);
    addProgram(ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE);
    addProgram(ProgramType::POSITION_COLOR);
    addProgram(ProgramType::POSITION);
    addProgram(ProgramType::LAYER_RADIA_GRADIENT);
    addProgram(ProgramType::POSITION_TEXTURE);
    addProgram(ProgramType::POSITION_TEXTURE_COLOR_ALPHA_TEST);
    addProgram(ProgramType::POSITION_UCOLOR);
    addProgram(ProgramType::ETC1_GRAY);
    addProgram(ProgramType::GRAY_SCALE);
    addProgram(ProgramType::LINE_COLOR_3D);
//     addProgram(ProgramType::CAMERA_CLEAR);
//     addProgram(ProgramType::SKYBOX_3D);
//     addProgram(ProgramType::SKINPOSITION_TEXTURE_3D);
//     addProgram(ProgramType::SKINPOSITION_NORMAL_TEXTURE_3D);
//     addProgram(ProgramType::POSITION_NORMAL_TEXTURE_3D);
//     addProgram(ProgramType::POSITION_TEXTURE_3D);
//     addProgram(ProgramType::POSITION_3D);
//     addProgram(ProgramType::POSITION_NORMAL_3D);
//     addProgram(ProgramType::POSITION_BUMPEDNORMAL_TEXTURE_3D);
//     addProgram(ProgramType::SKINPOSITION_BUMPEDNORMAL_TEXTURE_3D);
//     addProgram(ProgramType::TERRAIN_3D);
//     addProgram(ProgramType::PARTICLE_TEXTURE_3D);
//     addProgram(ProgramType::PARTICLE_COLOR_3D);
    return true;
}

void ProgramCache::addProgram(ProgramType type)
{
    Program* program = nullptr;
    switch (type) {
        case ProgramType::POSITION_TEXTURE_COLOR:
            program = new Program(positionTextureColor_vert, positionTextureColor_frag, type);
            break;
        case ProgramType::ETC1:
            program = new Program(positionTextureColor_vert, etc1_frag, type);
            break;
        case ProgramType::LABEL_DISTANCE_NORMAL:
            program = new Program(positionTextureColor_vert, label_distanceNormal_frag, type);
            break;
        case ProgramType::LABEL_NORMAL:
            program = new Program(positionTextureColor_vert, label_normal_frag, type);
            break;
        case ProgramType::LABLE_OUTLINE:
            program = new Program(positionTextureColor_vert, labelOutline_frag, type);
            break;
        case ProgramType::LABLE_DISTANCEFIELD_GLOW:
            program = new Program(positionTextureColor_vert, labelDistanceFieldGlow_frag, type);
            break;
        case ProgramType::POSITION_COLOR_LENGTH_TEXTURE:
            program = new Program(positionColorLengthTexture_vert, positionColorLengthTexture_frag, type);
            break;
        case ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE:
            program = new Program(positionColorTextureAsPointsize_vert, positionColor_frag, type);
            break;
        case ProgramType::POSITION_COLOR:
            program = new Program(positionColor_vert, positionColor_frag, type);
            break;
        case ProgramType::POSITION:
            program = new Program(position_vert, positionColor_frag, type);
            break;
        case ProgramType::LAYER_RADIA_GRADIENT:
            program = new Program(position_vert, layer_radialGradient_frag, type);
            break;
        case ProgramType::POSITION_TEXTURE:
            program = new Program(positionTexture_vert, positionTexture_frag, type);
            break;
        case ProgramType::POSITION_TEXTURE_COLOR_ALPHA_TEST:
            program = new Program(positionTextureColor_vert, positionTextureColorAlphaTest_frag, type);
            break;
        case ProgramType::POSITION_UCOLOR:
            program = new Program(positionUColor_vert, positionUColor_frag, type);
            break;
        case ProgramType::ETC1_GRAY:
            program = new Program(positionTextureColor_vert, etc1Gray_frag, type);
            break;
        case ProgramType::GRAY_SCALE:
            program = new Program(positionTextureColor_vert, grayScale_frag, type);
            break;
        case ProgramType::LINE_COLOR_3D:
            program = new Program(lineColor3D_vert, lineColor3D_frag, type);
            break;
        default:
            CCASSERT(false, "Not built-in program type.");
            break;
    }
    ProgramCache::_cachedPrograms.emplace(type, program);
}

backend::Program* ProgramCache::getBuiltinProgram(ProgramType type) const
{
    const auto& iter = ProgramCache::_cachedPrograms.find(type);
    if (ProgramCache::_cachedPrograms.end() != iter)
    {
        return iter->second;
    }
    return nullptr;
}

void ProgramCache::removeProgram(backend::Program* program)
{
    if (!program)
    {
        return;
    }
    
    for (auto it = _cachedPrograms.cbegin(); it != _cachedPrograms.cend();)
    {
        if (it->second == program)
        {
            it->second->release();
            it = _cachedPrograms.erase(it);
            break;
        }
        else
            ++it;
    }
}

void ProgramCache::removeUnusedProgram()
{
    for (auto iter = _cachedPrograms.cbegin(); iter != _cachedPrograms.cend();)
    {
        auto program = iter->second;
        if (program->getReferenceCount() == 1)
        {
//            CCLOG("cocos2d: TextureCache: removing unused program");
            program->release();
            iter = _cachedPrograms.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void ProgramCache::removeAllPrograms()
{
    for (auto& program : _cachedPrograms)
    {
        program.second->release();
    }
    _cachedPrograms.clear();
    
    for (auto& program : _cachedCustomPrograms)
    {
        program.second->release();
    }
    _cachedCustomPrograms.clear();
}

backend::Program* ProgramCache::getCustomProgram(const char* vsfile, const char* fsfile) const
{
    uint64_t key = (uint64_t)vsfile + (uint64_t)fsfile;
    const auto& iter = ProgramCache::_cachedCustomPrograms.find(key);
    if (ProgramCache::_cachedCustomPrograms.end() != iter) {
        return iter->second;
    }
    else {
        auto program = new Program(vsfile, fsfile);
        auto ret = ProgramCache::_cachedCustomPrograms.insert({key, program});
        return program;
    }
}

CC_BACKEND_END
