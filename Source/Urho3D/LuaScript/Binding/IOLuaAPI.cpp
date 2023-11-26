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
    lua.new_usertype<Deserializer>("Deserializer",
        "ReadString", &Deserializer::ReadString,
        "ReadInt64", &Deserializer::ReadInt64,
        "ReadInt", &Deserializer::ReadInt,
        "ReadShort", &Deserializer::ReadShort,
        "ReadByte", &Deserializer::ReadByte,
        "ReadUInt64", &Deserializer::ReadUInt64,
        "ReadUInt", &Deserializer::ReadUInt,
        "ReadUShort", &Deserializer::ReadUShort,
        "ReadUByte", &Deserializer::ReadUByte,
        "ReadBool", &Deserializer::ReadBool,
        "ReadFloat", &Deserializer::ReadFloat,
        "ReadColor", &Deserializer::ReadColor,
        "ReadVector3", &Deserializer::ReadVector3,
        "ReadQuaternion", &Deserializer::ReadQuaternion,
        "ReadVariantMap", &Deserializer::ReadVariantMap);
    lua.new_usertype<Serializer>("Serializer",
        "WriteString", &Serializer::WriteString,
        "WriteInt64", &Serializer::WriteInt64,
        "WriteInt", &Serializer::WriteInt,
        "WriteShort", &Serializer::WriteShort,
        "WriteByte", &Serializer::WriteByte,
        "WriteUInt64", &Serializer::WriteUInt64,
        "WriteUInt", &Serializer::WriteUInt,
        "WriteUShort", &Serializer::WriteUShort,
        "WriteUByte", &Serializer::WriteUByte,
        "WriteBool", &Serializer::WriteBool,
        "WriteFloat", &Serializer::WriteFloat,
        "WriteColor", &Serializer::WriteColor,
        "WriteVector3", &Serializer::WriteVector3,
        "WriteQuaternion", &Serializer::WriteQuaternion,
        "WriteVariantMap", &Serializer::WriteVariantMap);
    lua.new_usertype<AbstractFile>("AbstractFile",
        sol::base_classes, sol::bases<Deserializer, Serializer>());
    lua.new_usertype<VectorBuffer>("VectorBuffer",
        sol::call_constructor, sol::factories([]() { return VectorBuffer(); }, [](const void* data, unsigned size) { return VectorBuffer(data, size); }),
        sol::base_classes, sol::bases<AbstractFile, Deserializer, Serializer>());
    lua.new_usertype<MemoryBuffer>("MemoryBuffer",
        sol::call_constructor, sol::factories([](void* data, unsigned size) { return MemoryBuffer(data, size); }, [](const void* data, unsigned size) { return MemoryBuffer(data, size); }),
        sol::base_classes, sol::bases<AbstractFile, Deserializer, Serializer>());
    lua.new_usertype<File>("File",
        sol::base_classes, sol::bases<Object, AbstractFile>());
    lua.new_usertype<FileSystem>("FileSystem",
        "Copy", &FileSystem::Copy,
        "Rename", &FileSystem::Rename,
        "Delete", &FileSystem::Delete,
        "Exists", &FileSystem::Exists,
        "FileExists", &FileSystem::FileExists,
        "DirExists", &FileSystem::DirExists,
        "CopyDir", &FileSystem::CopyDir,
        "CreateDir", &FileSystem::CreateDir,
        "CreateDirs", &FileSystem::CreateDirs,
        "CreateDirsRecursive", &FileSystem::CreateDirsRecursive,
        "RemoveDir", &FileSystem::RemoveDir,
        "GetProgramDir", &FileSystem::GetProgramDir,
        "GetProgramFileName", &FileSystem::GetProgramFileName,
        "GetUserDocumentsDir", &FileSystem::GetUserDocumentsDir,
        "GetTemporaryDir", &FileSystem::GetTemporaryDir,
        "GetAppPreferencesDir", [](FileSystem* self, const ea::string& org, const ea::string& app) { return self->GetAppPreferencesDir(org, app); },
        "ReadTextFromFile", [](FileSystem* self, const ea::string& fileName) {
            auto file = MakeShared<File>(self->GetContext(), fileName, FILE_READ);
            return file->IsOpen() ? file->ReadText() : ea::string{};
        });
    lua.new_usertype<VirtualFileSystem>("VirtualFileSystem",
        "MountRoot", &VirtualFileSystem::MountRoot,
        "MountDir", sol::overload(
            [](VirtualFileSystem* self, const ea::string& path) { self->MountDir(path); },
            [](VirtualFileSystem* self, const ea::string& scheme, const ea::string& path) { self->MountDir(scheme, path); }),
        "AutomountDir", sol::overload(
            [](VirtualFileSystem* self, const ea::string& path) { self->AutomountDir(path); },
            [](VirtualFileSystem* self, const ea::string& scheme, const ea::string& path) { self->AutomountDir(scheme, path); }),
        "MountPackage", &VirtualFileSystem::MountPackageFile,
        "Unmount", [](VirtualFileSystem* self, const ea::string& path) {
            for (unsigned i = 0; i < self->NumMountPoints(); ++i) {
                auto mountPoint = self->GetMountPoint(i);
                const auto& dir = mountPoint->GetName();
                if ((dir.back() == '/' && RemoveTrailingSlash(dir) == path) || (dir == path)) {
                    self->Unmount(mountPoint);
                    break;
                }
            }
        },
        "UnmountAll", &VirtualFileSystem::UnmountAll,
        "GetAbsoluteFileName", [](VirtualFileSystem* self, const ea::string& path) {
            return self->GetAbsoluteNameFromIdentifier({"", path });
        });

    lua["filesystem"] = context->GetSubsystem<FileSystem>();
    lua["virtual_filesystem"] = context->GetSubsystem<VirtualFileSystem>();
	return 0;
}
