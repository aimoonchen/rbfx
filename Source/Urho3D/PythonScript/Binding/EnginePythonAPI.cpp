#include <nanobind/nanobind.h>
//#include <nanobind/stl/string.h>
#include "../../Core/Context.h"
#include "../../Engine/Engine.h"
#include "../../Resource/XMLFile.h"
#include "../../UI/BorderImage.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

// static void RegisterEngineConst(sol::state& lua)
// {
//     auto eventType = lua["EventType"].get_or_create<sol::table>();
//     auto paramType = lua["ParamType"].get_or_create<sol::table>();
// }

NB_MODULE(core, m)
{
    nb::class_<Engine>(m, "Engine")
        .def("Exit", &Engine::Exit);
//    m.attr("engine") = context->GetSubsystem<Engine>();
//    RegisterEngineConst(lua);
}
