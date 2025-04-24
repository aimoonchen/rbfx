#include <nanobind/nanobind.h>
#include "../../Core/Context.h"
#include "../../Resource/Resource.h"
#include "../../Resource/ResourceCache.h"
#include "../../Resource/Image.h"
#include "../../Urho2D/Sprite2D.h"
#include "../../Urho2D/SpriteSheet2D.h"
#include "../../Urho2D/AnimationSet2D.h"
#include "../../Urho2D/TmxFile2D.h"
#include "../../Resource/XMLFile.h"
#include "../../Resource/JSONValue.h"
#include "../../Resource/JSONFile.h"
#include "../../Graphics/Model.h"
#include "../../Graphics/Material.h"
#include "../../Graphics/Texture2D.h"
#include "../../Graphics/TextureCube.h"
#include "../../Graphics/Animation.h"
#include "../../Scene/PrefabResource.h"
#include "../../UI/Font.h"
#include <map>

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;
// namespace {
// ea::unordered_map<Urho3D::StringHash, ea::function<int(lua_State* L, const Resource* obj)>> resource_convert_map = {
//     {Texture2D::GetTypeStatic(),        [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const Texture2D*>(obj)).push(L); }},
//     {TextureCube::GetTypeStatic(),      [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const TextureCube*>(obj)).push(L); }},
//     {Image::GetTypeStatic(),            [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const Image*>(obj)).push(L); }},
//     {XMLFile::GetTypeStatic(),          [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const XMLFile*>(obj)).push(L); }},
//     {JSONFile::GetTypeStatic(),         [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const JSONFile*>(obj)).push(L); }},
//     {Model::GetTypeStatic(),            [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const Model*>(obj)).push(L); }},
//     {Material::GetTypeStatic(),         [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const Material*>(obj)).push(L); }},
//     {Font::GetTypeStatic(),             [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const Font*>(obj)).push(L); }},
//     {Animation::GetTypeStatic(),        [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const Animation*>(obj)).push(L); }},
//     {PrefabResource::GetTypeStatic(),   [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const PrefabResource*>(obj)).push(L); }},
//     {Sprite2D::GetTypeStatic(),         [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const Sprite2D*>(obj)).push(L); }},
//     {SpriteSheet2D::GetTypeStatic(),    [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const SpriteSheet2D*>(obj)).push(L); }},
//     {AnimationSet2D::GetTypeStatic(),   [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const AnimationSet2D*>(obj)).push(L); }},
//     {TmxFile2D::GetTypeStatic(),        [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const TmxFile2D*>(obj)).push(L); }}
// };
// }
// 
// template <typename T> void RegisterResourceConverter()
// {
//     resource_convert_map.insert({T::GetTypeStatic(), [](lua_State* L, const Resource* obj) { return sol::make_object(L, static_cast<const T*>(obj)).push(L); }});
// }
// 
// namespace sol {
//     int sol_lua_push(sol::types<Resource*>, lua_State* L, const Resource* obj)
//     {
//         if (obj) {
//             if (auto it = resource_convert_map.find(obj->GetType()); it != resource_convert_map.end()) {
//                 auto& [typeKey, converter] = *it;
//                 return converter(L, obj);
//             } else {
//                 URHO3D_LOGERRORF("sol_lua_push error, can't find convert function for %s.", obj->GetTypeName());
//             }
//         }
//         return sol::make_object(L, obj).push(L);
//     }
// }
#undef NB_EXPORT
#define NB_EXPORT
NB_MODULE(resource, m)
{
//     nb.enum_<ScanFlags>(m, "ScanFlags")
//         .value("SCAN_FILES", SCAN_FILES)
//         .value("SCAN_DIRS", SCAN_DIRS)
//         .value("SCAN_HIDDEN", SCAN_HIDDEN)
//         .value("SCAN_APPEND", SCAN_APPEND)
//         .value("SCAN_RECURSIVE", SCAN_RECURSIVE);
    nb::class_<ResourceCache>(m, "ResourceCache")
        .def("GetResource", [](ResourceCache* obj, /*StringHash*/const char* typeName, const ea::string& filePath) {
            return obj->GetResource(StringHash(typeName), filePath);
            })
        .def("GetFile", [](ResourceCache* obj, const ea::string& filePath) { return obj->GetFile(filePath).Detach(); })
        .def("SetSearchPackagesFirst", &ResourceCache::SetSearchPackagesFirst)
//         "AddResourceDir", sol::overload(
//             [](ResourceCache* self, const ea::string& pathName) { return self->AddResourceDir(pathName); },
//             [](ResourceCache* self, const ea::string& pathName, unsigned priority) { return self->AddResourceDir(pathName, priority); }),
//         "AddPackageFile", sol::overload(
//             [](ResourceCache* self, const ea::string& pathName) { return self->AddPackageFile(pathName); },
//             [](ResourceCache* self, const ea::string& pathName, unsigned priority) { return self->AddPackageFile(pathName, priority); }),
//         "RemoveResourceDir", &ResourceCache::RemoveResourceDir,
//         "RemovePackageFile", sol::overload(
//             [](ResourceCache* self, const ea::string& fileName) { self->RemovePackageFile(fileName); },
//             [](ResourceCache* self, const ea::string& fileName, bool releaseResources) { self->RemovePackageFile(fileName, releaseResources); },
//             [](ResourceCache* self, const ea::string& fileName, bool releaseResources, bool forceRelease) { self->RemovePackageFile(fileName, releaseResources, forceRelease); }),
//         "RemoveAllResourceDirs", &ResourceCache::RemoveAllResourceDirs,
        .def("ReleaseResource", [](ResourceCache* self, const ea::string& resourceName) { self->ReleaseResource(resourceName); })
        .def("ReleaseResource", [](ResourceCache* self, const ea::string& resourceName, bool force) { self->ReleaseResource(resourceName, force); })
        .def("ReleaseResources", [](ResourceCache* self, const ea::string& partialName) { self->ReleaseResources(partialName); })
        .def("ReleaseResources", [](ResourceCache* self, const ea::string& partialName, bool force) { self->ReleaseResources(partialName, force); })
        .def("ReleaseAllResources", [](ResourceCache* self) { self->ReleaseAllResources(); })
        .def("ReleaseAllResources", [](ResourceCache* self, bool force) { self->ReleaseAllResources(force); })
        .def("Scan", [](ResourceCache* self, const ea::string& pathName, const ea::string& filter, unsigned char flags) {
            ea::vector<ea::string> result;
            self->Scan(result, pathName, filter, ScanFlags(flags));
            std::vector<std::string> stdresult;
            stdresult.reserve(result.size());
            for (auto& it : result)
            {
                stdresult.emplace_back(it.c_str());
            }
            return stdresult;
            //return std::vector<std::string>(result.begin(), result.end());
        });
    
    nb::class_<Image>(m, "Image");// , sol::constructors<Image(Context*)>());

    nb::class_<XMLFile>(m, "XMLFile");// , sol::constructors<XMLFile(Context*)>());

//     lua.new_enum("JSONValueType",
//         "JSON_NULL", JSONValueType::JSON_NULL,
//         "JSON_BOOL", JSONValueType::JSON_BOOL,
//         "JSON_NUMBER", JSONValueType::JSON_NUMBER,
//         "JSON_STRING", JSONValueType::JSON_STRING,
//         "JSON_ARRAY", JSONValueType::JSON_ARRAY,
//         "JSON_OBJECT", JSONValueType::JSON_OBJECT);
//     lua.new_enum("JSONNumberType",
//         "JSONNT_NAN", JSONNumberType::JSONNT_NAN,
//         "JSONNT_INT", JSONNumberType::JSONNT_INT,
//         "JSONNT_UINT", JSONNumberType::JSONNT_UINT,
//         "JSONNT_FLOAT_DOUBLE", JSONNumberType::JSONNT_FLOAT_DOUBLE);

    nb::class_<JSONValue>(m, "JSONValue")
        .def(nb::init<>())
        .def(nb::init<JSONValueType, JSONNumberType>())
        .def(nb::init<bool>())
        .def(nb::init<int>())
        .def(nb::init<unsigned>())
        .def(nb::init<float>())
        .def(nb::init<double>())
        .def(nb::init<const char*>())
//         sol::call_constructor, sol::factories(
//             []() { return JSONValue(); },
//             [](JSONValueType valueType, JSONNumberType numberType) { return JSONValue(valueType, numberType); },
//             [](bool value) { return JSONValue(value); },
//             [](int value) { return JSONValue(value); },
//             [](unsigned value) { return JSONValue(value); },
//             [](float value) { return JSONValue(value); },
//             [](double value) { return JSONValue(value); },
//             [](const char* value) { return JSONValue(value); }
//         ));
    .def("SetType", &JSONValue::SetType)
    .def("Set", &JSONValue::Set)
    .def("Get", nb::overload_cast<const ea::string&>(&JSONValue::Get, nb::const_))
    .def("Get", nb::overload_cast<int>(&JSONValue::Get, nb::const_))
    .def("GetBool", &JSONValue::GetBool)
    .def("GetInt", &JSONValue::GetInt)
    .def("GetFloat", &JSONValue::GetFloat)
    .def("GetDouble", &JSONValue::GetDouble)
    .def("GetString", [](JSONValue* self, const ea::string& defaultValue) { return self->GetString(defaultValue); })
    .def("GetArray", [](JSONValue* self) {
        const auto& arrs = self->GetArray();
        return std::vector<JSONValue>(arrs.begin(), arrs.end());
        })
    .def("GetObject", [](JSONValue* self) {
        const auto& objects = self->GetObject();
        std::map<std::string, JSONValue> ret;
        for (auto& obj : objects) {
            ret.insert({ obj.first.c_str(), obj.second });
        }
        return ret; })
    .def("Push", &JSONValue::Push)
    .def("Pop", &JSONValue::Pop)
    .def("Insert", &JSONValue::Insert)
    .def("Erase", nb::overload_cast<const ea::string&>(&JSONValue::Erase))
    .def("Erase", nb::overload_cast<unsigned, unsigned>(&JSONValue::Erase))
    .def("Contains", &JSONValue::Contains)
    .def("Clear", &JSONValue::Clear)
    .def("Resize", &JSONValue::Resize)
    .def("Size", &JSONValue::Size);
        
    nb::class_<JSONFile>(m, "JSONFile")// , sol::constructors<JSONFile(Context*)>());
        .def("FromString", &JSONFile::FromString)
        .def("ToString", &JSONFile::ToString)
        .def("GetRoot", nb::overload_cast<>(&JSONFile::GetRoot));
        
    m.attr("cache") = Context::GetInstance()->GetSubsystem<ResourceCache>();
}
