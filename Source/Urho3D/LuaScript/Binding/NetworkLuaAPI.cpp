#include "GetPush.h"
#include "../../Core/Context.h"
#if URHO3D_NETWORK
#include "../../Network/Network.h"
#include "../../Network/Connection.h"
#include "../../Network/Protocol.h"
#include "../../Network/NetworkEvents.h"
#include "../../Scene/Scene.h"
#endif
#include "../../Utility/HttpProxy.h"

using namespace Urho3D;
Urho3D::Context* GetContext(lua_State* L);

// int sol_lua_push(lua_State* L, RefCounted* refobj)
// {
//     auto obj = dynamic_cast<Object*>(refobj);
//     if (obj) {
//         if (obj->GetTypeName() == "Connection") {
//             return sol::make_object(L, static_cast<const Connection*>(obj)).push(L);
//         } else {
//             assert(false);
//         }
//     } else {
//         assert(false);
//     }
//     return sol::make_object(L, obj).push(L);
// }
#if URHO3D_NETWORK
static void RegisterNetworkConst(sol::state& lua)
{
    auto protocol	= lua["Protocol"].get_or_create<sol::table>();
    //
    protocol["PACKAGE_FRAGMENT_SIZE"]		= PACKAGE_FRAGMENT_SIZE;
	//
    protocol["MSG_IDENTITY"]				= MSG_IDENTITY;
	protocol["MSG_SCENELOADED"]				= MSG_SCENELOADED;
	protocol["MSG_REQUESTPACKAGE"]			= MSG_REQUESTPACKAGE;
	protocol["MSG_PACKAGEDATA"]				= MSG_PACKAGEDATA;
	protocol["MSG_LOADSCENE"]				= MSG_LOADSCENE;
	protocol["MSG_SCENECHECKSUMERROR"]		= MSG_SCENECHECKSUMERROR;
	protocol["MSG_REMOTEEVENT"]				= MSG_REMOTEEVENT;
//	protocol["MSG_REMOTENODEEVENT"]			= MSG_REMOTENODEEVENT;
	protocol["MSG_PACKAGEINFO"]				= MSG_PACKAGEINFO;
//	protocol["MSG_PACKED_MESSAGE"]			= MSG_PACKED_MESSAGE;
	protocol["MSG_USER"]					= MSG_USER;
	
	// event
	auto eventType = lua["EventType"].get_or_create<sol::table>();
	eventType["E_NETWORKMESSAGE"] = E_NETWORKMESSAGE;

    auto paramType = lua["ParamType"].get_or_create<sol::table>();
    paramType["P_CONNECTION"]	= NetworkMessage::P_CONNECTION;
	paramType["P_MESSAGEID"]	= NetworkMessage::P_MESSAGEID;
	paramType["P_DATA"]			= NetworkMessage::P_DATA;
}
#endif
int sol2_NetworkLuaAPI_open(sol::state& lua)
{
#if URHO3D_NETWORK
    auto context = GetContext(lua.lua_state());
	lua.new_usertype<Connection>("Connection",
        "scene", sol::property(&Connection::GetScene, &Connection::SetScene),
        "SendMessage", [](Connection* obj, NetworkMessageId messageId, bool reliable, bool inOrder, const VectorBuffer& msg) { obj->SendMessage(messageId, msg); },
		"SendRemoteEvent", sol::resolve<void(StringHash, bool, const VariantMap&)>(&Connection::SendRemoteEvent),
		"Disconnect", &Connection::Disconnect);
	lua.new_usertype<Network>("Network",
        "serverConnection", sol::property(&Network::GetServerConnection),
        "serverRunning", sol::property(&Network::IsServerRunning),
        "StartServer", [](Network* obj, unsigned short port) { obj->StartServer(port); },
        "StopServer", &Network::StopServer,
		"Connect", [](Network* obj, ea::string_view& url, Scene* scene, const VariantMap& identity) { return obj->Connect(url, scene, identity); },
        "Disconnect", &Network::Disconnect,
		"RegisterRemoteEvent", &Network::RegisterRemoteEvent,
		"GetServerConnection", &Network::GetServerConnection,
        "BroadcastMessage", [](Network* obj, int msgID, bool reliable, bool inOrder, const VectorBuffer& msg) { obj->BroadcastMessage((NetworkMessageId)msgID, msg); });
    lua["network"] = context->GetSubsystem<Network>();
	RegisterNetworkConst(lua);
#endif
    auto http = lua["http"].get_or_create<sol::table>();
    // http request
    http["Fetch"] = [](const ea::string& url) {
        std::vector<char> body;
        auto code = HttpProxy::Instance().Fetch(url.c_str(), body);
        ea::string message;
        message.assign(body.data(), body.size());
        return std::make_tuple(code, message);
    };
    http["Post"] = [](){},
    http["Download"] = [](const ea::string& url, const ea::string& path) { return HttpProxy::Instance().Download(url.c_str(), path.c_str()); };
    http["DownloadAsync"] = [](const ea::string& url, const ea::string& path) { return HttpProxy::Instance().DownloadAsync(url.c_str(), path.c_str()); };
    // for web platform, Emscripten async default.
    http["FetchAsync"] = [](const ea::string& url) { return HttpProxy::Instance().FetchAsync(url.c_str()); };
    http["IsTaskFinish"] = [](uint32_t tid) { return HttpProxy::Instance().IsTaskFinish(tid); };
    http["GetState"] = [](uint32_t tid) {
        int curl_code{ -1 };
        int download_size{ 0 };
        int response_code{ 0 };
        int size{ 0 };
        HttpProxy::Instance().GetState(tid, curl_code, response_code, download_size, size);
        return std::make_tuple(curl_code, response_code, download_size, size);
    };
    http["GetResult"] = [](uint32_t tid) {
        auto body = HttpProxy::Instance().GetResult(tid);
        ea::string message;
        message.assign(body.data(), body.size());
        return message;
    };
	return 0;
}
