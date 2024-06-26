#include "../../Core/Context.h"
#include "../../Resource/Resource.h"
#include "../../Resource/ResourceCache.h"
#include "../../Resource/Image.h"
#include "../../Urho2D/Sprite2D.h"
#include "../../Urho2D/SpriteSheet2D.h"
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
#include <sol/sol.hpp>
#include "GetPush.h"

using namespace Urho3D;

namespace sol {
    int sol_lua_push(sol::types<Resource*>, lua_State* L, const Resource* obj)
    {
        if (obj) {
            const auto& objType = obj->GetType();
            if (objType == Texture2D::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Texture2D*>(obj)).push(L);
            } else if (objType == TextureCube::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const TextureCube*>(obj)).push(L);
            } else if (objType == Image::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Image*>(obj)).push(L);
            } else if (objType == XMLFile::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const XMLFile*>(obj)).push(L);
            } else if (objType == JSONFile::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const JSONFile*>(obj)).push(L);
            } else if (objType == Model::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Model*>(obj)).push(L);
            } else if (objType == Material::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Material*>(obj)).push(L);
            } else if (objType == Font::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Font*>(obj)).push(L);
            } else if (objType == Animation::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Animation*>(obj)).push(L);
            } else if (objType == PrefabResource::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const PrefabResource*>(obj)).push(L);
            } else if (objType == Sprite2D::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const Sprite2D*>(obj)).push(L);
            } else if (objType == SpriteSheet2D::GetTypeStatic()) {
                return sol::make_object(L, static_cast<const SpriteSheet2D*>(obj)).push(L);
            }
        }
        return sol::make_object(L, obj).push(L);
    }
}

Urho3D::Context* GetContext(lua_State* L);

int sol2_ResourceLuaAPI_open(sol::state& lua)
{
    auto context = GetContext(lua.lua_state());
    lua.new_enum("ScanFlags",
        "SCAN_FILES", SCAN_FILES,
        "SCAN_DIRS", SCAN_DIRS,
        "SCAN_HIDDEN", SCAN_HIDDEN,
        "SCAN_APPEND", SCAN_APPEND,
        "SCAN_RECURSIVE", SCAN_RECURSIVE);
    auto bindResourceCache = lua.new_usertype<ResourceCache>("ResourceCache");
    bindResourceCache["GetResource"]    = [](ResourceCache* obj, /*StringHash*/const char* typeName, const ea::string& filePath) {
        return obj->GetResource(StringHash(typeName), filePath);
        };
    bindResourceCache["GetFile"]                = [](ResourceCache* obj, const ea::string& filePath) { return obj->GetFile(filePath).Detach(); };
    bindResourceCache["SetSearchPackagesFirst"] = &ResourceCache::SetSearchPackagesFirst;
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
    bindResourceCache["ReleaseResource"]        = sol::overload(
        [](ResourceCache* self, const ea::string& resourceName) { self->ReleaseResource(resourceName); },
        [](ResourceCache* self, const ea::string& resourceName, bool force) { self->ReleaseResource(resourceName, force); });
    bindResourceCache["ReleaseResources"]       = sol::overload(
        [](ResourceCache* self, const ea::string& partialName) { self->ReleaseResources(partialName); },
        [](ResourceCache* self, const ea::string& partialName, bool force) { self->ReleaseResources(partialName, force); });
    bindResourceCache["ReleaseAllResources"]    = sol::overload(
        [](ResourceCache* self) { self->ReleaseAllResources(); },
        [](ResourceCache* self, bool force) { self->ReleaseAllResources(force); });
    bindResourceCache["Scan"]                   = [](ResourceCache* self, const ea::string& pathName, const ea::string& filter, unsigned char flags) {
        ea::vector<ea::string> result;
        self->Scan(result, pathName, filter, ScanFlags(flags));
        std::vector<std::string> stdresult;
        stdresult.reserve(result.size());
        for (auto& it : result) {
            stdresult.emplace_back(it.c_str());
        }
        return stdresult;
        };
    
    lua.new_usertype<Image>("Image", sol::constructors<Image(Context*)>());

    lua.new_usertype<XMLFile>("XMLFile", sol::constructors<XMLFile(Context*)>());

    lua.new_enum("JSONValueType",
        "JSON_NULL", JSONValueType::JSON_NULL,
        "JSON_BOOL", JSONValueType::JSON_BOOL,
        "JSON_NUMBER", JSONValueType::JSON_NUMBER,
        "JSON_STRING", JSONValueType::JSON_STRING,
        "JSON_ARRAY", JSONValueType::JSON_ARRAY,
        "JSON_OBJECT", JSONValueType::JSON_OBJECT);
    lua.new_enum("JSONNumberType",
        "JSONNT_NAN", JSONNumberType::JSONNT_NAN,
        "JSONNT_INT", JSONNumberType::JSONNT_INT,
        "JSONNT_UINT", JSONNumberType::JSONNT_UINT,
        "JSONNT_FLOAT_DOUBLE", JSONNumberType::JSONNT_FLOAT_DOUBLE);

    auto bindJSONValue = lua.new_usertype<JSONValue>("JSONValue",
        sol::call_constructor, sol::factories(
            []() { return JSONValue(); },
            [](JSONValueType valueType, JSONNumberType numberType) { return JSONValue(valueType, numberType); },
            [](bool value) { return JSONValue(value); },
            [](int value) { return JSONValue(value); },
            [](unsigned value) { return JSONValue(value); },
            [](float value) { return JSONValue(value); },
            [](double value) { return JSONValue(value); },
            [](const char* value) { return JSONValue(value); }
        ));
    bindJSONValue["SetType"]    = &JSONValue::SetType;
    bindJSONValue["Set"]        = &JSONValue::Set;
    bindJSONValue["Get"]        = sol::overload(
        sol::resolve<const JSONValue & (const ea::string&) const>(&JSONValue::Get),
        sol::resolve<const JSONValue & (int) const>(&JSONValue::Get));
    bindJSONValue["GetBool"]    = &JSONValue::GetBool;
    bindJSONValue["GetInt"]     = &JSONValue::GetInt;
    bindJSONValue["GetFloat"]   = &JSONValue::GetFloat;
    bindJSONValue["GetDouble"]  = &JSONValue::GetDouble;
    bindJSONValue["GetString"]  = [](JSONValue* self, const ea::string& defaultValue) { return self->GetString(defaultValue); };
    bindJSONValue["GetArray"]   = [](JSONValue* self) {
        const auto& arrs = self->GetArray();
        std::vector<JSONValue> ret;
        ret.reserve(arrs.size());
        for (auto& obj : arrs) {
            ret.emplace_back(obj);
        }
        return ret; };
    bindJSONValue["GetObject"]  = [](JSONValue* self) {
        const auto& objects = self->GetObject();
        std::map<std::string, JSONValue> ret;
        for (auto& obj : objects) {
            ret.insert({ obj.first.c_str(), obj.second });
        }
        return ret; };
    bindJSONValue["Push"]       = &JSONValue::Push;
    bindJSONValue["Pop"]        = &JSONValue::Pop;
    bindJSONValue["Insert"]     = &JSONValue::Insert;
    bindJSONValue["Erase"]      = sol::overload(sol::resolve<bool(const ea::string&)>(&JSONValue::Erase), sol::resolve<void(unsigned, unsigned)>(&JSONValue::Erase));
    bindJSONValue["Contains"]   = &JSONValue::Contains;
    bindJSONValue["Clear"]      = &JSONValue::Clear;
    bindJSONValue["Resize"]     = &JSONValue::Resize;
    bindJSONValue["Size"]       = &JSONValue::Size;
        
    auto bindXMLFile = lua.new_usertype<JSONFile>("XMLFile", sol::constructors<JSONFile(Context*)>());
    bindXMLFile["FromString"]   = &JSONFile::FromString;
    bindXMLFile["ToString"]     = &JSONFile::ToString;
    bindXMLFile["GetRoot"]      = sol::resolve<JSONValue & ()>(&JSONFile::GetRoot);
        
    lua["cache"] = context->GetSubsystem<ResourceCache>();
    return 0;
}
