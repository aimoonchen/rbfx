#include "../../Core/Context.h"
#if URHO3D_NETWORK
#include <nanobind/nanobind.h>
#include "../../Network/Network.h"
#include "../../Network/Connection.h"
#include "../../Network/Protocol.h"
#include "../../Network/NetworkEvents.h"
#include "../../Scene/Scene.h"
#endif
#include "../../Utility/HttpProxy.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

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
void RegisterNetworkConst(nb::module_ m)
{
    nb::enum_<NetworkMessageId>(m, "Protocol")
    //
    //protocol["PACKAGE_FRAGMENT_SIZE"]		= PACKAGE_FRAGMENT_SIZE;
	//
        .value("MSG_IDENTITY", MSG_IDENTITY)
	    .value("MSG_SCENELOADED", MSG_SCENELOADED)
	    .value("MSG_REQUESTPACKAGE", MSG_REQUESTPACKAGE)
	    .value("MSG_PACKAGEDATA", MSG_PACKAGEDATA)
	    .value("MSG_LOADSCENE", MSG_LOADSCENE)
	    .value("MSG_SCENECHECKSUMERROR", MSG_SCENECHECKSUMERROR)
	    .value("MSG_REMOTEEVENT", MSG_REMOTEEVENT)
//	protocol["MSG_REMOTENODEEVENT"]			= MSG_REMOTENODEEVENT;
        .value("MSG_PACKAGEINFO", MSG_PACKAGEINFO)
//	protocol["MSG_PACKED_MESSAGE"]			= MSG_PACKED_MESSAGE;
        .value("MSG_USER", MSG_USER);
	
	// event
    auto eventType = m.def_submodule("EventType");
    eventType.attr("E_NETWORKMESSAGE") = E_NETWORKMESSAGE;
    auto paramType = m.def_submodule("ParamType");
    paramType.attr("P_CONNECTION")	= NetworkMessage::P_CONNECTION;
	paramType.attr("P_MESSAGEID")	= NetworkMessage::P_MESSAGEID;
	paramType.attr("P_DATA")		= NetworkMessage::P_DATA;
}
#endif

NB_MODULE(network, m)
{
#if URHO3D_NETWORK
    Context* context = nullptr;
    nb::class_<Connection>(m, "Connection")
        .def_prop_rw("scene", &Connection::GetScene, &Connection::SetScene)
        .def("SendMessage", [](Connection* obj, NetworkMessageId messageId, bool reliable, bool inOrder, const VectorBuffer& msg) { obj->SendMessage(messageId, msg); })
        .def("SendRemoteEvent", nb::overload_cast<StringHash, bool, const VariantMap&>(&Connection::SendRemoteEvent))
        .def("Disconnect", &Connection::Disconnect);

    nb::class_<Network>(m, "Network")
        .def_prop_ro("serverConnection", &Network::GetServerConnection)
        .def_prop_ro("serverRunning", &Network::IsServerRunning)
        .def("StartServer", [](Network* obj, unsigned short port) { obj->StartServer(port); })
        .def("StopServer", &Network::StopServer)
        .def("Connect", [](Network* obj, ea::string_view& url, Scene* scene, const VariantMap& identity) { return obj->Connect(url, scene, identity); })
        .def("Disconnect", &Network::Disconnect)
        .def("RegisterRemoteEvent", &Network::RegisterRemoteEvent)
        .def("GetServerConnection", &Network::GetServerConnection)
        .def("BroadcastMessage", [](Network* obj, int msgID, bool reliable, bool inOrder, const VectorBuffer& msg) { obj->BroadcastMessage((NetworkMessageId)msgID, msg); });

    m.attr("network") = context->GetSubsystem<Network>();
	RegisterNetworkConst(m);
#endif
    auto http = m.def_submodule("http");
    // http request
    http.def("Fetch", [](const ea::string& url) {
        std::vector<char> body;
        auto code = HttpProxy::Instance().Fetch(url.c_str(), body);
        ea::string message;
        message.assign(body.data(), body.size());
        return std::make_tuple(code, message);
    });
    http.def("Post", []() {});
    http.def("Download", [](const ea::string& url, const ea::string& path) { return HttpProxy::Instance().Download(url.c_str(), path.c_str()); });
    http.def("DownloadAsync", [](const ea::string& url, const ea::string& path) { return HttpProxy::Instance().DownloadAsync(url.c_str(), path.c_str()); });
    // for web platform, Emscripten async default.
    http.def("FetchAsync", [](const ea::string& url) { return HttpProxy::Instance().FetchAsync(url.c_str()); });
    http.def("IsTaskFinish", [](uint32_t tid) { return HttpProxy::Instance().IsTaskFinish(tid); });
    http.def("GetState", [](uint32_t tid) {
        int curl_code{ -1 };
        int download_size{ 0 };
        int response_code{ 0 };
        int size{ 0 };
        HttpProxy::Instance().GetState(tid, curl_code, response_code, download_size, size);
        return std::make_tuple(curl_code, response_code, download_size, size);
    });
    http.def("GetResult", [](uint32_t tid) {
        auto body = HttpProxy::Instance().GetResult(tid);
        ea::string message;
        message.assign(body.data(), body.size());
        return message;
    });
}
