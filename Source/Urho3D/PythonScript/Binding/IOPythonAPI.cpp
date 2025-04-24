#include <nanobind/nanobind.h>
#include "../../Core/Context.h"
#include "../../IO/VectorBuffer.h"
#include "../../IO/MemoryBuffer.h"
#include "../../IO/File.h"
#include "../../IO/FileSystem.h"
#include "../../IO/VirtualFileSystem.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;
#undef NB_EXPORT
#define NB_EXPORT
NB_MODULE(io, m)
{
    nb::class_<Deserializer>(m, "Deserializer")
        .def("ReadString", &Deserializer::ReadString)
        .def("ReadInt64", &Deserializer::ReadInt64)
        .def("ReadInt", &Deserializer::ReadInt)
        .def("ReadShort", &Deserializer::ReadShort)
        .def("ReadByte", &Deserializer::ReadByte)
        .def("ReadUInt64", &Deserializer::ReadUInt64)
        .def("ReadUInt", &Deserializer::ReadUInt)
        .def("ReadUShort", &Deserializer::ReadUShort)
        .def("ReadUByte", &Deserializer::ReadUByte)
        .def("ReadBool", &Deserializer::ReadBool)
        .def("ReadFloat", &Deserializer::ReadFloat)
        .def("ReadColor", &Deserializer::ReadColor)
        .def("ReadVector3", &Deserializer::ReadVector3)
        .def("ReadQuaternion", &Deserializer::ReadQuaternion)
        .def("ReadVariantMap", &Deserializer::ReadVariantMap);

    nb::class_<Serializer>(m, "Serializer")
        .def("WriteString", nb::overload_cast<const char*>(&Serializer::WriteString))
        .def("WriteInt64", &Serializer::WriteInt64)
        .def("WriteInt", &Serializer::WriteInt)
        .def("WriteShort", &Serializer::WriteShort)
        .def("WriteByte", &Serializer::WriteByte)
        .def("WriteUInt64", &Serializer::WriteUInt64)
        .def("WriteUInt", &Serializer::WriteUInt)
        .def("WriteUShort", &Serializer::WriteUShort)
        .def("WriteUByte", &Serializer::WriteUByte)
        .def("WriteBool", &Serializer::WriteBool)
        .def("WriteFloat", &Serializer::WriteFloat)
        .def("WriteColor", &Serializer::WriteColor)
        .def("WriteVector3", &Serializer::WriteVector3)
        .def("WriteQuaternion", &Serializer::WriteQuaternion)
        .def("WriteVariantMap", &Serializer::WriteVariantMap);

    nb::class_<AbstractFile, Serializer>(m, "AbstractFile");

    nb::class_<VectorBuffer, AbstractFile>(m, "VectorBuffer")
        .def(nb::init<>())
        .def(nb::init<const void*, unsigned>());
//         sol::call_constructor, sol::constructors<VectorBuffer(), VectorBuffer(const void*, unsigned)>(),

    nb::class_<MemoryBuffer, AbstractFile>(m, "MemoryBuffer")
        .def(nb::init<void*, unsigned>())
        .def(nb::init<const void*, unsigned>());
        //         sol::call_constructor, sol::constructors<MemoryBuffer(void*, unsigned), MemoryBuffer(const void*, unsigned)>(),

    nb::class_<File, AbstractFile>(m, "File");

    nb::class_<FileSystem>(m, "FileSystem")
        .def("Copy", &FileSystem::Copy)
        .def("Rename", &FileSystem::Rename)
        .def("Delete", &FileSystem::Delete)
        .def("Exists", &FileSystem::Exists)
        .def("FileExists", &FileSystem::FileExists)
        .def("DirExists", &FileSystem::DirExists)
        .def("CopyDir", &FileSystem::CopyDir)
        .def("CreateDir", &FileSystem::CreateDir)
        .def("CreateDirs", &FileSystem::CreateDirs)
        .def("CreateDirsRecursive", &FileSystem::CreateDirsRecursive)
        .def("RemoveDir", &FileSystem::RemoveDir)
        .def("GetProgramDir", &FileSystem::GetProgramDir)
        .def("GetProgramFileName", &FileSystem::GetProgramFileName)
        .def("GetUserDocumentsDir", &FileSystem::GetUserDocumentsDir)
        .def("GetTemporaryDir", &FileSystem::GetTemporaryDir)
        .def("GetAppPreferencesDir", [](FileSystem* self, const ea::string& org, const ea::string& app) { return self->GetAppPreferencesDir(org, app); })
        .def("ReadTextFromFile", [](FileSystem* self, const ea::string& fileName) {
            auto file = MakeShared<File>(self->GetContext(), fileName, FILE_READ);
            return file->IsOpen() ? file->ReadText() : ea::string{};
        });

    nb::class_<VirtualFileSystem>(m, "VirtualFileSystem")
        .def("MountRoot", &VirtualFileSystem::MountRoot)
        .def("MountDir", [](VirtualFileSystem* self, const ea::string& path) { self->MountDir(path); })
        .def("MountDir", [](VirtualFileSystem* self, const ea::string& scheme, const ea::string& path) { self->MountDir(scheme, path); })
        .def("AutomountDir", [](VirtualFileSystem* self, const ea::string& path) { self->AutomountDir(path); })
        .def("AutomountDir", [](VirtualFileSystem* self, const ea::string& scheme, const ea::string& path) { self->AutomountDir(scheme, path); })
        .def("MountPackage", &VirtualFileSystem::MountPackageFile)
        .def("Unmount", [](VirtualFileSystem* self, const ea::string& path) {
            for (unsigned i = 0; i < self->NumMountPoints(); ++i) {
                auto mountPoint = self->GetMountPoint(i);
                const auto& dir = mountPoint->GetName();
                if ((dir.back() == '/' && RemoveTrailingSlash(dir) == path) || (dir == path)) {
                    self->Unmount(mountPoint);
                    break;
                }
            }
        })
        .def("UnmountAll", &VirtualFileSystem::UnmountAll)
        .def("GetAbsoluteFileName", [](VirtualFileSystem* self, const ea::string& path) {
        return self->GetAbsoluteNameFromIdentifier({"", path});
        });

    m.attr("filesystem") = Context::GetInstance()->GetSubsystem<FileSystem>();
    m.attr("virtual_filesystem") = Context::GetInstance()->GetSubsystem<VirtualFileSystem>();
}
