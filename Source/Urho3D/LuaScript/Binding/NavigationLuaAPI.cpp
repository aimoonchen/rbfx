#include "GetPush.h"
#include "../../Navigation/NavigationMesh.h"
#include "../../Navigation/DynamicNavigationMesh.h"
#include "../../Navigation/Navigable.h"
#include "../../Navigation/Obstacle.h"
#include "../../Navigation/OffMeshConnection.h"
#include "../../Navigation/CrowdAgent.h"
#include "../../Navigation/CrowdManager.h"
#include "../../Navigation/NavigationEvents.h"

Urho3D::Context* GetContext(lua_State* L);
using namespace Urho3D;

int sol2_NavigationLuaAPI_open(sol::state& lua)
{
    auto context = GetContext(lua.lua_state());

    lua.new_usertype<NavigationMesh>("NavigationMesh",
        "id", sol::var(StringHash("NavigationMesh")),
        //"AddTile", [](NavigationMesh* self, const ea::vector<unsigned char>& tileData) { return self->AddTile(tileData); },
        "Build", sol::overload(
            sol::resolve<bool()>(&NavigationMesh::Build),
            sol::resolve<bool(const BoundingBox&)>(&NavigationMesh::Build),
            sol::resolve<bool(const IntVector2&, const IntVector2&)>(&NavigationMesh::Build)),
        "FindNearestPoint", sol::overload(
            [](NavigationMesh* self, const Vector3& point) { return self->FindNearestPoint(point); },
            [](NavigationMesh* self, const Vector3& point, const Vector3& extents) { return self->FindNearestPoint(point, extents); }),
        "MoveAlongSurface", sol::overload(
            [](NavigationMesh* self, const Vector3& start, const Vector3& end) { return self->MoveAlongSurface(start, end); },
            [](NavigationMesh* self, const Vector3& start, const Vector3& end, const Vector3& extents) { return self->MoveAlongSurface(start, end, extents); }),
        "Raycast", sol::overload(
            [](NavigationMesh* self, const Vector3& start, const Vector3& end) { return self->Raycast(start, end); },
            [](NavigationMesh* self, const Vector3& start, const Vector3& end, const Vector3& extents) { return self->Raycast(start, end, extents); }),
        sol::base_classes, sol::bases<Component>());

    lua.new_usertype<DynamicNavigationMesh>("DynamicNavigationMesh",
        "id", sol::var(StringHash("DynamicNavigationMesh")),
        "bounding_box", sol::property(&DynamicNavigationMesh::GetBoundingBox),
        "Allocate", &DynamicNavigationMesh::Allocate,
        "GetTileData", [](DynamicNavigationMesh* self, const IntVector2& tile) {
            const auto& tdata = self->GetTileData(tile);
            std::vector<unsigned char> ret(tdata.size());
            memcpy(ret.data(), tdata.data(), sizeof(unsigned char) * tdata.size());
            return ret;
        },
        "GetTileIndex", &DynamicNavigationMesh::GetTileIndex,
        "GetNumTiles", &DynamicNavigationMesh::GetNumTiles,
        "AddTile", [](DynamicNavigationMesh* self, const std::vector<unsigned char>& tileData) {
            ea::vector<unsigned char> tdata(tileData.size());
            memcpy(tdata.data(), tileData.data(), sizeof(unsigned char) * tileData.size());
            self->AddTile(tdata);
        },
        "HasTile", &DynamicNavigationMesh::HasTile,
        "RemoveTile", &DynamicNavigationMesh::RemoveTile,
        "SetTileSize",&DynamicNavigationMesh::SetTileSize,
        "SetDrawObstacles", &DynamicNavigationMesh::SetDrawObstacles,
        "SetDrawOffMeshConnections", &DynamicNavigationMesh::SetDrawOffMeshConnections,
        "SetAgentHeight", &DynamicNavigationMesh::SetAgentHeight,
        "SetCellHeight", &DynamicNavigationMesh::SetCellHeight,
        "SetPadding", &DynamicNavigationMesh::SetPadding,
        "DrawDebugGeometry", sol::resolve<void(bool)>(&DynamicNavigationMesh::DrawDebugGeometry),
        sol::base_classes, sol::bases<NavigationMesh, Component>());

    lua.new_usertype<CrowdObstacleAvoidanceParams>("CrowdObstacleAvoidanceParams",
        "velBias", &CrowdObstacleAvoidanceParams::velBias,
        "weightDesVel", &CrowdObstacleAvoidanceParams::weightDesVel,
        "weightCurVel", &CrowdObstacleAvoidanceParams::weightCurVel,
        "weightSide", &CrowdObstacleAvoidanceParams::weightSide,
        "weightToi", &CrowdObstacleAvoidanceParams::weightToi,
        "horizTime", &CrowdObstacleAvoidanceParams::horizTime,
        "gridSize", &CrowdObstacleAvoidanceParams::gridSize,
        "adaptiveDivs", &CrowdObstacleAvoidanceParams::adaptiveDivs,
        "adaptiveRings", &CrowdObstacleAvoidanceParams::adaptiveRings,
        "adaptiveDepth", &CrowdObstacleAvoidanceParams::adaptiveDepth
        );

    lua.new_usertype<CrowdAgent>("CrowdAgent",
        "id", sol::var(StringHash("CrowdAgent")),
        "radius", sol::property(&CrowdAgent::GetRadius, &CrowdAgent::SetRadius),
        "height", sol::property(&CrowdAgent::GetHeight, &CrowdAgent::SetHeight),
        "max_speed", sol::property(&CrowdAgent::GetMaxSpeed, &CrowdAgent::SetMaxSpeed),
        "max_accel", sol::property(&CrowdAgent::GetMaxAccel, &CrowdAgent::SetMaxAccel),
        "query_filter_type", sol::property(&CrowdAgent::GetQueryFilterType, &CrowdAgent::SetQueryFilterType),
        "SetUpdateNodePosition", &CrowdAgent::SetUpdateNodePosition,
        "SetTargetVelocity", &CrowdAgent::SetTargetVelocity,
        "GetActualVelocity", &CrowdAgent::GetActualVelocity,
        "GetPosition", &CrowdAgent::GetPosition,
        "SetNavigationQuality", &CrowdAgent::SetNavigationQuality,
        sol::base_classes, sol::bases<Component>());

    lua.new_usertype<CrowdManager>("CrowdManager",
        "id", sol::var(StringHash("CrowdManager")),
        "GetObstacleAvoidanceParams", &CrowdManager::GetObstacleAvoidanceParams,
        "SetObstacleAvoidanceParams", &CrowdManager::SetObstacleAvoidanceParams,
        "SetCrowdTarget", sol::overload(
            [](CrowdManager* self, const Vector3& position) { self->SetCrowdTarget(position); },
            [](CrowdManager* self, const Vector3& position, Node* node) { self->SetCrowdTarget(position, node); }),
        "GetRandomPointInCircle", [](CrowdManager* self, const Vector3& center, float radius, int queryFilterType) { return self->GetRandomPointInCircle(center, radius, queryFilterType); },
        "DrawDebugGeometry", sol::resolve<void(bool)>(&CrowdManager::DrawDebugGeometry),
        sol::base_classes, sol::bases<Component>());

    lua.new_usertype<Navigable>("Navigable",
        "id", sol::var(StringHash("Navigable")),
        "SetRecursive", &Navigable::SetRecursive,
        "IsRecursive", &Navigable::IsRecursive,
        sol::base_classes, sol::bases<Component>());

    lua.new_usertype<Obstacle>("Obstacle",
        "id", sol::var(StringHash("Obstacle")),
        "radius", sol::property(&Obstacle::GetRadius, &Obstacle::SetRadius),
        "height", sol::property(&Obstacle::GetHeight, &Obstacle::SetHeight),
        "SetRadius", &Obstacle::SetRadius,
        "SetHeight", &Obstacle::SetHeight,
        sol::base_classes, sol::bases<Component>());

    lua.new_usertype<OffMeshConnection>("OffMeshConnection",
        "id", sol::var(StringHash("OffMeshConnection")),
        "SetEndPoint", &OffMeshConnection::SetEndPoint,
        sol::base_classes, sol::bases<Component>());

    auto paramType = lua["ParamType"].get_or_create<sol::table>();
    paramType["P_NODE"]         = CrowdAgentReposition::P_NODE;
    paramType["P_CROWD_AGENT"]  = CrowdAgentReposition::P_CROWD_AGENT;
    paramType["P_VELOCITY"]     = CrowdAgentReposition::P_VELOCITY;

    paramType["P_CROWD_AGENT_STATE"] = CrowdAgentFailure::P_CROWD_AGENT_STATE;
    paramType["P_CROWD_TARGET_STATE"] = CrowdAgentFailure::P_CROWD_AGENT_STATE;

    paramType["P_INDEX"] = CrowdAgentFormation::P_INDEX;
    paramType["P_SIZE"] = CrowdAgentFormation::P_SIZE;
    paramType["P_POSITION"] = CrowdAgentFormation::P_POSITION;
    //
    lua["NAVIGATIONQUALITY_LOW"] = NAVIGATIONQUALITY_LOW;
    lua["NAVIGATIONQUALITY_MEDIUM"] = NAVIGATIONQUALITY_MEDIUM;
    lua["NAVIGATIONQUALITY_HIGH"] = NAVIGATIONQUALITY_HIGH;
    //
    lua["CA_STATE_INVALID"] = CA_STATE_INVALID;
    lua["CA_STATE_WALKING"] = CA_STATE_WALKING;
    lua["CA_STATE_OFFMESH"] = CA_STATE_OFFMESH;
    return 0;
}
