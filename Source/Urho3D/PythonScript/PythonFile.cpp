#include "../Precompiled.h"
#include "../PythonScript/PythonFile.h"
#include "../Core/Context.h"
#include "../IO/Deserializer.h"
#include "../IO/FileSystem.h"
#include "../IO/Log.h"
#include "../Core/ProcessUtils.h"
#include "../IO/Serializer.h"

#include "../DebugNew.h"

namespace Urho3D
{

PythonFile::PythonFile(Context* context) :
    Resource(context),
    size_(0),
    hasLoaded_(false),
    hasExecuted_(false)
{
}

PythonFile::~PythonFile() = default;

void PythonFile::RegisterObject(Context* context)
{
    context->RegisterFactory<PythonFile>();
}

bool PythonFile::BeginLoad(Deserializer& source)
{
    size_ = source.GetSize();

    if (size_ == 0)
        return false;

    // Read all data.
    data_ = new char[size_];
    if (source.Read(data_.get(), size_) != size_)
        return false;

    SetMemoryUse(size_);

    return true;
}

bool PythonFile::Save(Serializer& dest) const
{
    if (size_ == 0)
        return false;

    dest.Write(data_.get(), size_);

    return true;
}


bool PythonFile::LoadChunk(lua_State* luaState)
{
    if (hasLoaded_)
        return true;

    if (size_ == 0 || !luaState)
        return false;

    // Get file base name
    ea::string name = GetName();
    unsigned extPos = name.find_last_of('.');
    if (extPos != ea::string::npos)
        name = name.substr(0, extPos);

//     if (luaL_loadbuffer(luaState, data_.get(), size_, name.c_str()))
//     {
//         const char* message = lua_tostring(luaState, -1);
//         URHO3D_LOGERRORF("Load Buffer failed for %s: %s", GetName().c_str(), message);
//         lua_pop(luaState, 1);
//         return false;
//     }

    URHO3D_LOGINFO("Loaded Lua script " + GetName());
    hasLoaded_ = true;

    return true;
}

bool PythonFile::LoadAndExecute(lua_State* luaState)
{
    if (hasExecuted_)
        return true;

    if (!LoadChunk(luaState))
        return false;

//     if (lua_pcall(luaState, 0, 0, 0))
//     {
//         const char* message = lua_tostring(luaState, -1);
//         URHO3D_LOGERRORF("Lua Execute failed for %s: %s", GetName().c_str(), message);
//         lua_pop(luaState, 1);
//         return false;
//     }

    URHO3D_LOGINFO("Executed Lua script " + GetName());
    hasExecuted_ = true;

    return true;
}

}
