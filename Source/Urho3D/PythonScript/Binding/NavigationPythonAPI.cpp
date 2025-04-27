#include <nanobind/nanobind.h>
#include "Urho3D/Scene/Node.h"
#include "Urho3D/Navigation/NavigationMesh.h"
#include "Urho3D/Navigation/DynamicNavigationMesh.h"
#include "Urho3D/Navigation/Navigable.h"
#include "Urho3D/Navigation/Obstacle.h"
#include "Urho3D/Navigation/OffMeshConnection.h"
#include "Urho3D/Navigation/CrowdAgent.h"
#include "Urho3D/Navigation/CrowdManager.h"
#include "Urho3D/Navigation/NavigationEvents.h"
#include "Urho3D/Navigation/AStar/AStar.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;
void init_cmodule_navigation(nb::module_& pm)
{
    auto m = pm.def_submodule("navigation");
    nb::class_<NavigationMesh, Component>(m, "NavigationMesh")
        .def_ro_static("TypeId", &NavigationMesh::TypeId)
        //"AddTile", [](NavigationMesh* self, const ea::vector<unsigned char>& tileData) { return self->AddTile(tileData); },
        .def("Build", nb::overload_cast<>(&NavigationMesh::Rebuild))
        .def("Build", nb::overload_cast<const BoundingBox&>(&NavigationMesh::BuildTilesInRegion))
        .def("Build", nb::overload_cast<const IntVector2&, const IntVector2&>(&NavigationMesh::BuildTiles))
        .def("FindNearestPoint", [](NavigationMesh* self, const Vector3& point) { return self->FindNearestPoint(point); })
        .def("FindNearestPoint", [](NavigationMesh* self, const Vector3& point, const Vector3& extents) { return self->FindNearestPoint(point, extents); })
        .def("MoveAlongSurface", [](NavigationMesh* self, const Vector3& start, const Vector3& end) { return self->MoveAlongSurface(start, end); })
        .def("MoveAlongSurface", [](NavigationMesh* self, const Vector3& start, const Vector3& end, const Vector3& extents) { return self->MoveAlongSurface(start, end, extents); })
        .def("Raycast", [](NavigationMesh* self, const Vector3& start, const Vector3& end) { return self->Raycast(start, end); })
        .def("Raycast", [](NavigationMesh* self, const Vector3& start, const Vector3& end, const Vector3& extents) { return self->Raycast(start, end, extents); })
        .def("GetAllTileIndices", [](NavigationMesh* self) {
            auto tileIndices = self->GetAllTileIndices();
            return std::vector<IntVector2>(tileIndices.begin(), tileIndices.end());
        });

    nb::class_<DynamicNavigationMesh, Component>(m, "DynamicNavigationMesh")
        .def_ro_static("TypeId", &DynamicNavigationMesh::TypeId)
    //bindDynamicNavigationMesh["bounding_box"]       = sol::readonly_property(&DynamicNavigationMesh::GetBoundingBox);
        .def("Allocate", &DynamicNavigationMesh::Allocate)
        .def("GetTileData", [](DynamicNavigationMesh* self, const IntVector2& tile) {
            const auto& tdata = self->GetTileData(tile);
            std::vector<unsigned char> ret(tdata.size());
            memcpy(ret.data(), tdata.data(), sizeof(unsigned char) * tdata.size());
            return ret;
            })
        .def("GetTileIndex", &DynamicNavigationMesh::GetTileIndex)
        .def("AddTile", [](DynamicNavigationMesh* self, const std::vector<unsigned char>& tileData) {
            ea::vector<unsigned char> tdata(tileData.size());
            memcpy(tdata.data(), tileData.data(), sizeof(unsigned char) * tileData.size());
            self->AddTile(tdata);
            })
        .def("HasTile", &DynamicNavigationMesh::HasTile)
        .def("RemoveTile", &DynamicNavigationMesh::RemoveTile)
        .def("SetTileSize", &DynamicNavigationMesh::SetTileSize)
        .def("SetDrawObstacles", &DynamicNavigationMesh::SetDrawObstacles)
        .def("SetAgentHeight", &DynamicNavigationMesh::SetAgentHeight)
        .def("SetCellHeight", &DynamicNavigationMesh::SetCellHeight)
        .def("SetPadding", &DynamicNavigationMesh::SetPadding)
        .def("DrawDebugGeometry", nb::overload_cast<bool>(&DynamicNavigationMesh::DrawDebugGeometry))
        .def("SetDrawOffMeshConnections", &DynamicNavigationMesh::SetDrawOffMeshConnections);

    nb::class_<CrowdObstacleAvoidanceParams>(m, "CrowdObstacleAvoidanceParams")
        .def_rw("velBias", &CrowdObstacleAvoidanceParams::velBias)
        .def_rw("weightDesVel", &CrowdObstacleAvoidanceParams::weightDesVel)
        .def_rw("weightCurVel", &CrowdObstacleAvoidanceParams::weightCurVel)
        .def_rw("weightSide", &CrowdObstacleAvoidanceParams::weightSide)
        .def_rw("weightToi", &CrowdObstacleAvoidanceParams::weightToi)
        .def_rw("horizTime", &CrowdObstacleAvoidanceParams::horizTime)
        .def_rw("gridSize", &CrowdObstacleAvoidanceParams::gridSize)
        .def_rw("adaptiveDivs", &CrowdObstacleAvoidanceParams::adaptiveDivs)
        .def_rw("adaptiveRings", &CrowdObstacleAvoidanceParams::adaptiveRings)
        .def_rw("adaptiveDepth", &CrowdObstacleAvoidanceParams::adaptiveDepth);
        
    nb::class_<CrowdAgent, Component>(m, "CrowdAgent")
        .def_ro_static("TypeId", &CrowdAgent::TypeId)
        .def_prop_rw("radius", &CrowdAgent::GetRadius, &CrowdAgent::SetRadius)
        .def_prop_rw("height", &CrowdAgent::GetHeight, &CrowdAgent::SetHeight)
        .def_prop_rw("max_speed", &CrowdAgent::GetMaxSpeed, &CrowdAgent::SetMaxSpeed)
        .def_prop_rw("max_accel", &CrowdAgent::GetMaxAccel, &CrowdAgent::SetMaxAccel)
        .def_prop_rw("query_filter_type", &CrowdAgent::GetQueryFilterType, &CrowdAgent::SetQueryFilterType)
        .def("SetUpdateNodePosition", &CrowdAgent::SetUpdateNodePosition)
        .def("SetTargetVelocity", &CrowdAgent::SetTargetVelocity)
        .def("GetActualVelocity", &CrowdAgent::GetActualVelocity)
        .def("GetPosition", &CrowdAgent::GetPosition)
        .def("SetNavigationQuality", &CrowdAgent::SetNavigationQuality);
    
    nb::class_<CrowdManager, Component>(m, "CrowdManager")
        .def_ro_static("TypeId", &CrowdManager::TypeId)
        .def("GetObstacleAvoidanceParams", &CrowdManager::GetObstacleAvoidanceParams)
        .def("SetObstacleAvoidanceParams", &CrowdManager::SetObstacleAvoidanceParams)
        .def("SetCrowdTarget", [](CrowdManager* self, const Vector3& position) { self->SetCrowdTarget(position); })
        .def("SetCrowdTarget", [](CrowdManager* self, const Vector3& position, Node* node) { self->SetCrowdTarget(position, node); })
        .def("GetRandomPointInCircle", [](CrowdManager* self, const Vector3& center, float radius, int queryFilterType) { return self->GetRandomPointInCircle(center, radius, queryFilterType); })
        .def("DrawDebugGeometry", nb::overload_cast<bool>(&CrowdManager::DrawDebugGeometry));

    nb::class_<Navigable, Component>(m, "Navigable")
        .def_ro_static("TypeId", &Navigable::TypeId)
        .def("SetRecursive", &Navigable::SetRecursive)
        .def("IsRecursive", &Navigable::IsRecursive);

    nb::class_<Obstacle, Component>(m, "Obstacle")
        .def_ro_static("TypeId", &Obstacle::TypeId)
        .def_prop_rw("radius", &Obstacle::GetRadius, &Obstacle::SetRadius)
        .def_prop_rw("height", &Obstacle::GetHeight, &Obstacle::SetHeight)
        .def("SetRadius", &Obstacle::SetRadius)
        .def("SetHeight", &Obstacle::SetHeight);

    nb::class_<OffMeshConnection, Component>(m, "OffMeshConnection")
        .def_ro_static("TypeId", &OffMeshConnection::TypeId)
        .def("SetEndPoint", &OffMeshConnection::SetEndPoint);

    //auto eventType = lua["EventType"].get_or_create<sol::table>();
    auto subm = m.def_submodule("EventType");
    subm.attr("CrowdAgentReposition")       = E_CROWD_AGENT_REPOSITION;
    subm.attr("CrowdAgentFailure")          = E_CROWD_AGENT_FAILURE;
    subm.attr("CrowdAgentFormation")        = E_CROWD_AGENT_FORMATION;

    subm = m.def_submodule("CrowdAgentReposition");
    //auto paramType = lua["ParamType"].get_or_create<sol::table>();
    //auto crowdAgentReposition = paramType["CrowdAgentReposition"].get_or_create<sol::table>();
    subm.attr("Node")            = CrowdAgentReposition::P_NODE;
    subm.attr("CrowdAgent")      = CrowdAgentReposition::P_CROWD_AGENT;
    subm.attr("Position")        = CrowdAgentReposition::P_POSITION;
    subm.attr("Velocity")        = CrowdAgentReposition::P_VELOCITY;
    subm = m.def_submodule("CrowdAgentFailure");
    //auto crowdAgentFailure = paramType["CrowdAgentFailure"].get_or_create<sol::table>();
    subm.attr("CrowdAgent")         = CrowdAgentFailure::P_CROWD_AGENT;
    subm.attr("Position")           = CrowdAgentFailure::P_POSITION;
    subm.attr("Velocity")           = CrowdAgentFailure::P_VELOCITY;
    subm.attr("CrowdAgentState")    = CrowdAgentFailure::P_CROWD_AGENT_STATE;
    subm.attr("CrowdTargetState")   = CrowdAgentFailure::P_CROWD_TARGET_STATE;
    subm = m.def_submodule("CrowdAgentFormation");
    //auto crowdAgentFormation = paramType["CrowdAgentFormation"].get_or_create<sol::table>();
    subm.attr("Index")        = CrowdAgentFormation::P_INDEX;
    subm.attr("Size")         = CrowdAgentFormation::P_SIZE;
    subm.attr("Position")     = CrowdAgentFormation::P_POSITION;

    nb::enum_<NavigationQuality>(m, "NavigationQuality")
        .value("LOW", NAVIGATIONQUALITY_LOW)
        .value("MEDIUM", NAVIGATIONQUALITY_MEDIUM)
        .value("HIGH", NAVIGATIONQUALITY_HIGH);

    nb::enum_<CrowdAgentState>(m, "CrowdAgentState")
        .value("INVALID",  CA_STATE_INVALID)
        .value("WALKING",  CA_STATE_WALKING)
        .value("OFFMESH",  CA_STATE_OFFMESH);

    // a star
    nb::class_<AStar::Generator>(m, "AStar")
        .def(nb::new_([]() { return new AStar::Generator(); }))
        .def("SetWorldSize", [](AStar::Generator* self, int xdim, int ydim) { self->setWorldSize({ xdim, ydim }); })
        .def("SetDiagonalMovement", &AStar::Generator::setDiagonalMovement)
        .def("SetHeuristic", [](AStar::Generator* self, int type) {
            if (type == 0) {
                self->setHeuristic(AStar::Heuristic::manhattan);
            } else if (type == 1) {
                self->setHeuristic(AStar::Heuristic::euclidean);
            } else if (type == 2) {
                self->setHeuristic(AStar::Heuristic::octagonal);
            }
        })
        .def("FindPath", [](AStar::Generator* self, int sx, int sy, int tx, int ty) {
            std::vector<int> ret;
            self->findPath({sx, sy}, {tx, ty}, &ret);
            return ret;
        }
        );
    m.attr("Manhattan") = 0;
    m.attr("Euclidean") = 1;
    m.attr("Octagonal") = 2;
}
