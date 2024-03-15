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

    auto bindNavigationMesh = lua.new_usertype<NavigationMesh>("NavigationMesh", sol::base_classes, sol::bases<Component>());
    bindNavigationMesh["id"]                = sol::var(StringHash("NavigationMesh"));
        //"AddTile", [](NavigationMesh* self, const ea::vector<unsigned char>& tileData) { return self->AddTile(tileData); },
    bindNavigationMesh["Build"]             = sol::overload(
        sol::resolve<bool()>(&NavigationMesh::Rebuild),
        sol::resolve<bool(const BoundingBox&)>(&NavigationMesh::BuildTilesInRegion),
        sol::resolve<bool(const IntVector2&, const IntVector2&)>(&NavigationMesh::BuildTiles));
    bindNavigationMesh["FindNearestPoint"]  = sol::overload(
        [](NavigationMesh* self, const Vector3& point) { return self->FindNearestPoint(point); },
        [](NavigationMesh* self, const Vector3& point, const Vector3& extents) { return self->FindNearestPoint(point, extents); });
    bindNavigationMesh["MoveAlongSurface"]  = sol::overload(
        [](NavigationMesh* self, const Vector3& start, const Vector3& end) { return self->MoveAlongSurface(start, end); },
        [](NavigationMesh* self, const Vector3& start, const Vector3& end, const Vector3& extents) { return self->MoveAlongSurface(start, end, extents); });
    bindNavigationMesh["Raycast"]           = sol::overload(
        [](NavigationMesh* self, const Vector3& start, const Vector3& end) { return self->Raycast(start, end); },
        [](NavigationMesh* self, const Vector3& start, const Vector3& end, const Vector3& extents) { return self->Raycast(start, end, extents); });
        
    auto bindDynamicNavigationMesh = lua.new_usertype<DynamicNavigationMesh>("DynamicNavigationMesh", sol::base_classes, sol::bases<NavigationMesh, Component>());
    bindDynamicNavigationMesh["id"]                 = sol::var(StringHash("DynamicNavigationMesh"));
    //bindDynamicNavigationMesh["bounding_box"]       = sol::readonly_property(&DynamicNavigationMesh::GetBoundingBox);
    bindDynamicNavigationMesh["Allocate"]           = &DynamicNavigationMesh::Allocate;
    bindDynamicNavigationMesh["GetTileData"]        = [](DynamicNavigationMesh* self, const IntVector2& tile) {
        const auto& tdata = self->GetTileData(tile);
        std::vector<unsigned char> ret(tdata.size());
        memcpy(ret.data(), tdata.data(), sizeof(unsigned char) * tdata.size());
        return ret;
        };
    bindDynamicNavigationMesh["GetTileIndex"]       = &DynamicNavigationMesh::GetTileIndex;
    //bindDynamicNavigationMesh["GetNumTiles"]        = &DynamicNavigationMesh::GetNumTiles;
    bindDynamicNavigationMesh["AddTile"]            = [](DynamicNavigationMesh* self, const std::vector<unsigned char>& tileData) {
        ea::vector<unsigned char> tdata(tileData.size());
        memcpy(tdata.data(), tileData.data(), sizeof(unsigned char) * tileData.size());
        self->AddTile(tdata);
        };
    bindDynamicNavigationMesh["HasTile"]                    = &DynamicNavigationMesh::HasTile;
    bindDynamicNavigationMesh["RemoveTile"]                 = &DynamicNavigationMesh::RemoveTile;
    bindDynamicNavigationMesh["SetTileSize"]                = &DynamicNavigationMesh::SetTileSize;
    bindDynamicNavigationMesh["SetDrawObstacles"]           = &DynamicNavigationMesh::SetDrawObstacles;
    bindDynamicNavigationMesh["SetAgentHeight"]             = &DynamicNavigationMesh::SetAgentHeight;
    bindDynamicNavigationMesh["SetCellHeight"]              = &DynamicNavigationMesh::SetCellHeight;
    bindDynamicNavigationMesh["SetPadding"]                 = &DynamicNavigationMesh::SetPadding;
    bindDynamicNavigationMesh["DrawDebugGeometry"]          = sol::resolve<void(bool)>(&DynamicNavigationMesh::DrawDebugGeometry);
    bindDynamicNavigationMesh["SetDrawOffMeshConnections"]  = &DynamicNavigationMesh::SetDrawOffMeshConnections;

    auto bindCrowdObstacleAvoidanceParams = lua.new_usertype<CrowdObstacleAvoidanceParams>("CrowdObstacleAvoidanceParams");
    bindCrowdObstacleAvoidanceParams["velBias"]         = &CrowdObstacleAvoidanceParams::velBias;
    bindCrowdObstacleAvoidanceParams["weightDesVel"]    = &CrowdObstacleAvoidanceParams::weightDesVel;
    bindCrowdObstacleAvoidanceParams["weightCurVel"]    = &CrowdObstacleAvoidanceParams::weightCurVel;
    bindCrowdObstacleAvoidanceParams["weightSide"]      = &CrowdObstacleAvoidanceParams::weightSide;
    bindCrowdObstacleAvoidanceParams["weightToi"]       = &CrowdObstacleAvoidanceParams::weightToi;
    bindCrowdObstacleAvoidanceParams["horizTime"]       = &CrowdObstacleAvoidanceParams::horizTime;
    bindCrowdObstacleAvoidanceParams["gridSize"]        = &CrowdObstacleAvoidanceParams::gridSize;
    bindCrowdObstacleAvoidanceParams["adaptiveDivs"]    = &CrowdObstacleAvoidanceParams::adaptiveDivs;
    bindCrowdObstacleAvoidanceParams["adaptiveRings"]   = &CrowdObstacleAvoidanceParams::adaptiveRings;
    bindCrowdObstacleAvoidanceParams["adaptiveDepth"]   = &CrowdObstacleAvoidanceParams::adaptiveDepth;
        
    auto bindCrowdAgent = lua.new_usertype<CrowdAgent>("CrowdAgent", sol::base_classes, sol::bases<Component>());
    bindCrowdAgent["id"]                    = sol::var(StringHash("CrowdAgent"));
    bindCrowdAgent["radius"]                = sol::property(&CrowdAgent::GetRadius, &CrowdAgent::SetRadius);
    bindCrowdAgent["height"]                = sol::property(&CrowdAgent::GetHeight, &CrowdAgent::SetHeight);
    bindCrowdAgent["max_speed"]             = sol::property(&CrowdAgent::GetMaxSpeed, &CrowdAgent::SetMaxSpeed);
    bindCrowdAgent["max_accel"]             = sol::property(&CrowdAgent::GetMaxAccel, &CrowdAgent::SetMaxAccel);
    bindCrowdAgent["query_filter_type"]     = sol::property(&CrowdAgent::GetQueryFilterType, &CrowdAgent::SetQueryFilterType);
    bindCrowdAgent["SetUpdateNodePosition"] = &CrowdAgent::SetUpdateNodePosition;
    bindCrowdAgent["SetTargetVelocity"]     = &CrowdAgent::SetTargetVelocity;
    bindCrowdAgent["GetActualVelocity"]     = &CrowdAgent::GetActualVelocity;
    bindCrowdAgent["GetPosition"]           = &CrowdAgent::GetPosition;
    bindCrowdAgent["SetNavigationQuality"]  = &CrowdAgent::SetNavigationQuality;

    auto bindCrowdManager = lua.new_usertype<CrowdManager>("CrowdManager", sol::base_classes, sol::bases<Component>());
    bindCrowdManager["id"]                          = sol::var(StringHash("CrowdManager"));
    bindCrowdManager["GetObstacleAvoidanceParams"]  = &CrowdManager::GetObstacleAvoidanceParams;
    bindCrowdManager["SetObstacleAvoidanceParams"]  = &CrowdManager::SetObstacleAvoidanceParams;
    bindCrowdManager["SetCrowdTarget"]              = sol::overload(
        [](CrowdManager* self, const Vector3& position) { self->SetCrowdTarget(position); },
        [](CrowdManager* self, const Vector3& position, Node* node) { self->SetCrowdTarget(position, node); });
    bindCrowdManager["GetRandomPointInCircle"]      = [](CrowdManager* self, const Vector3& center, float radius, int queryFilterType) { return self->GetRandomPointInCircle(center, radius, queryFilterType); };
    bindCrowdManager["DrawDebugGeometry"]           = sol::resolve<void(bool)>(&CrowdManager::DrawDebugGeometry);

    auto bindNavigable = lua.new_usertype<Navigable>("Navigable", sol::base_classes, sol::bases<Component>());
    bindNavigable["id"]                     = sol::var(StringHash("Navigable"));
    bindNavigable["SetRecursive"]           = &Navigable::SetRecursive;
    bindNavigable["IsRecursive"]            = &Navigable::IsRecursive;

    auto bindObstacle = lua.new_usertype<Obstacle>("Obstacle", sol::base_classes, sol::bases<Component>());
    bindObstacle["id"]                      = sol::var(StringHash("Obstacle"));
    bindObstacle["radius"]                  = sol::property(&Obstacle::GetRadius, &Obstacle::SetRadius);
    bindObstacle["height"]                  = sol::property(&Obstacle::GetHeight, &Obstacle::SetHeight);
    bindObstacle["SetRadius"]               = &Obstacle::SetRadius;
    bindObstacle["SetHeight"]               = &Obstacle::SetHeight;

    auto bindOffMeshConnection = lua.new_usertype<OffMeshConnection>("OffMeshConnection", sol::base_classes, sol::bases<Component>());
    bindOffMeshConnection["id"]             = sol::var(StringHash("OffMeshConnection"));
    bindOffMeshConnection["SetEndPoint"]    = &OffMeshConnection::SetEndPoint;

    auto paramType = lua["ParamType"].get_or_create<sol::table>();
    paramType["P_NODE"]                 = CrowdAgentReposition::P_NODE;
    paramType["P_CROWD_AGENT"]          = CrowdAgentReposition::P_CROWD_AGENT;
    paramType["P_VELOCITY"]             = CrowdAgentReposition::P_VELOCITY;

    paramType["P_CROWD_AGENT_STATE"]    = CrowdAgentFailure::P_CROWD_AGENT_STATE;
    paramType["P_CROWD_TARGET_STATE"]   = CrowdAgentFailure::P_CROWD_AGENT_STATE;

    paramType["P_INDEX"]                = CrowdAgentFormation::P_INDEX;
    paramType["P_SIZE"]                 = CrowdAgentFormation::P_SIZE;
    paramType["P_POSITION"]             = CrowdAgentFormation::P_POSITION;
    //
    lua["NAVIGATIONQUALITY_LOW"]        = NAVIGATIONQUALITY_LOW;
    lua["NAVIGATIONQUALITY_MEDIUM"]     = NAVIGATIONQUALITY_MEDIUM;
    lua["NAVIGATIONQUALITY_HIGH"]       = NAVIGATIONQUALITY_HIGH;
    //
    lua["CA_STATE_INVALID"]             = CA_STATE_INVALID;
    lua["CA_STATE_WALKING"]             = CA_STATE_WALKING;
    lua["CA_STATE_OFFMESH"]             = CA_STATE_OFFMESH;
    return 0;
}
