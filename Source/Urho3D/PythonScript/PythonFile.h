#pragma once

#include <EASTL/shared_array.h>
#include "Urho3D/Resource/Resource.h"

struct lua_State;

namespace Urho3D
{

/// Python file.
class URHO3D_API PythonFile : public Resource
{
    URHO3D_OBJECT(PythonFile, Resource);

public:
    /// Construct.
    explicit PythonFile(Context* context);
    /// Destruct.
    ~PythonFile() override;
    /// Register object factory.
    /// @nobind
    static void RegisterObject(Context* context);

    /// Load resource from stream. May be called from a worker thread. Return true if successful.
    bool BeginLoad(Deserializer& source) override;
    /// Save resource. Return true if successful.
    bool Save(Serializer& dest) const override;

    /// Load buffer as chunk. Return true if the resource is successfully loaded as a Lua function on top of the stack.
    bool LoadChunk(lua_State* luaState);
    /// Load buffer as chunk and execute. Return true if successful.
    bool LoadAndExecute(lua_State* luaState);

private:
    /// File size.
    unsigned size_;
    /// File data.
    ea::shared_array<char> data_;
    /// Has loaded.
    bool hasLoaded_;
    /// Has executed.
    bool hasExecuted_;
};

}
