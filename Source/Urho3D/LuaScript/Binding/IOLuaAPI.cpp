#include "GetPush.h"
#include "../../Core/Context.h"
#include "../../IO/VectorBuffer.h"
#include "../../IO/MemoryBuffer.h"
#include "../../IO/File.h"
#include "../../IO/FileSystem.h"
#include "../../IO/VirtualFileSystem.h"

using namespace Urho3D;

Urho3D::Context* GetContext(lua_State* L);

int sol2_IOLuaAPI_open(sol::state& lua)
{
    auto context = GetContext(lua.lua_state());
    auto bindDeserializer = lua.new_usertype<Deserializer>("Deserializer");
    bindDeserializer["ReadString"]      = &Deserializer::ReadString;
    bindDeserializer["ReadInt64"]       = &Deserializer::ReadInt64;
    bindDeserializer["ReadInt"]         = &Deserializer::ReadInt;
    bindDeserializer["ReadShort"]       = &Deserializer::ReadShort;
    bindDeserializer["ReadByte"]        = &Deserializer::ReadByte;
    bindDeserializer["ReadUInt64"]      = &Deserializer::ReadUInt64;
    bindDeserializer["ReadUInt"]        = &Deserializer::ReadUInt;
    bindDeserializer["ReadUShort"]      = &Deserializer::ReadUShort;
    bindDeserializer["ReadUByte"]       = &Deserializer::ReadUByte;
    bindDeserializer["ReadBool"]        = &Deserializer::ReadBool;
    bindDeserializer["ReadFloat"]       = &Deserializer::ReadFloat;
    bindDeserializer["ReadColor"]       = &Deserializer::ReadColor;
    bindDeserializer["ReadVector3"]     = &Deserializer::ReadVector3;
    bindDeserializer["ReadQuaternion"]  = &Deserializer::ReadQuaternion;
    bindDeserializer["ReadVariantMap"]  = &Deserializer::ReadVariantMap;

    auto bindSerializer = lua.new_usertype<Serializer>("Serializer");
    bindSerializer["WriteString"]       = sol::resolve<bool(const char*)>(&Serializer::WriteString);
    bindSerializer["WriteInt64"]        = &Serializer::WriteInt64;
    bindSerializer["WriteInt"]          = &Serializer::WriteInt;
    bindSerializer["WriteShort"]        = &Serializer::WriteShort;
    bindSerializer["WriteByte"]         = &Serializer::WriteByte;
    bindSerializer["WriteUInt64"]       = &Serializer::WriteUInt64;
    bindSerializer["WriteUInt"]         = &Serializer::WriteUInt;
    bindSerializer["WriteUShort"]       = &Serializer::WriteUShort;
    bindSerializer["WriteUByte"]        = &Serializer::WriteUByte;
    bindSerializer["WriteBool"]         = &Serializer::WriteBool;
    bindSerializer["WriteFloat"]        = &Serializer::WriteFloat;
    bindSerializer["WriteColor"]        = &Serializer::WriteColor;
    bindSerializer["WriteVector3"]      = &Serializer::WriteVector3;
    bindSerializer["WriteQuaternion"]   = &Serializer::WriteQuaternion;
    bindSerializer["WriteVariantMap"]   = &Serializer::WriteVariantMap;

    lua.new_usertype<AbstractFile>("AbstractFile",
        sol::base_classes, sol::bases<Deserializer, Serializer>());

    lua.new_usertype<VectorBuffer>("VectorBuffer",
        sol::call_constructor, sol::constructors<VectorBuffer(), VectorBuffer(const void*, unsigned)>(),
        sol::base_classes, sol::bases<AbstractFile, Deserializer, Serializer>());

    lua.new_usertype<MemoryBuffer>("MemoryBuffer",
        sol::call_constructor, sol::constructors<MemoryBuffer(void*, unsigned), MemoryBuffer(const void*, unsigned)>(),
        sol::base_classes, sol::bases<AbstractFile, Deserializer, Serializer>());

    lua.new_usertype<File>("File",
        sol::base_classes, sol::bases<Object, AbstractFile>());

    auto bindFileSystem = lua.new_usertype<FileSystem>("FileSystem");
    bindFileSystem["Copy"]                  = &FileSystem::Copy;
    bindFileSystem["Rename"]                = &FileSystem::Rename;
    bindFileSystem["Delete"]                = &FileSystem::Delete;
    bindFileSystem["Exists"]                = &FileSystem::Exists;
    bindFileSystem["FileExists"]            = &FileSystem::FileExists;
    bindFileSystem["DirExists"]             = &FileSystem::DirExists;
    bindFileSystem["CopyDir"]               = &FileSystem::CopyDir;
    bindFileSystem["CreateDir"]             = &FileSystem::CreateDir;
    bindFileSystem["CreateDirs"]            = &FileSystem::CreateDirs;
    bindFileSystem["CreateDirsRecursive"]   = &FileSystem::CreateDirsRecursive;
    bindFileSystem["RemoveDir"]             = &FileSystem::RemoveDir;
    bindFileSystem["GetProgramDir"]         = &FileSystem::GetProgramDir;
    bindFileSystem["GetProgramFileName"]    = &FileSystem::GetProgramFileName;
    bindFileSystem["GetUserDocumentsDir"]   = &FileSystem::GetUserDocumentsDir;
    bindFileSystem["GetTemporaryDir"]       = &FileSystem::GetTemporaryDir;
    bindFileSystem["GetAppPreferencesDir"]  = [](FileSystem* self, const ea::string& org, const ea::string& app) { return self->GetAppPreferencesDir(org, app); };
    bindFileSystem["ReadTextFromFile"]      = [](FileSystem* self, const ea::string& fileName) {
        auto file = MakeShared<File>(self->GetContext(), fileName, FILE_READ);
        return file->IsOpen() ? file->ReadText() : ea::string{};
    };

    auto bindVirtualFileSystem = lua.new_usertype<VirtualFileSystem>("VirtualFileSystem");
    bindVirtualFileSystem["MountRoot"]              = &VirtualFileSystem::MountRoot;
    bindVirtualFileSystem["MountDir"]               = sol::overload(
        [](VirtualFileSystem* self, const ea::string& path) { self->MountDir(path); },
        [](VirtualFileSystem* self, const ea::string& scheme, const ea::string& path) { self->MountDir(scheme, path); });
    bindVirtualFileSystem["AutomountDir"]           = sol::overload(
        [](VirtualFileSystem* self, const ea::string& path) { self->AutomountDir(path); },
        [](VirtualFileSystem* self, const ea::string& scheme, const ea::string& path) { self->AutomountDir(scheme, path); });
    bindVirtualFileSystem["MountPackage"]           = &VirtualFileSystem::MountPackageFile;
    bindVirtualFileSystem["Unmount"]                = [](VirtualFileSystem* self, const ea::string& path) {
        for (unsigned i = 0; i < self->NumMountPoints(); ++i) {
            auto mountPoint = self->GetMountPoint(i);
            const auto& dir = mountPoint->GetName();
            if ((dir.back() == '/' && RemoveTrailingSlash(dir) == path) || (dir == path)) {
                self->Unmount(mountPoint);
                break;
            }
        }
    };
    bindVirtualFileSystem["UnmountAll"]             = &VirtualFileSystem::UnmountAll;
    bindVirtualFileSystem["GetAbsoluteFileName"]    = [](VirtualFileSystem* self, const ea::string& path) {
        return self->GetAbsoluteNameFromIdentifier({ "", path });
    };

    lua["filesystem"] = context->GetSubsystem<FileSystem>();
    lua["virtual_filesystem"] = context->GetSubsystem<VirtualFileSystem>();
	return 0;
}
