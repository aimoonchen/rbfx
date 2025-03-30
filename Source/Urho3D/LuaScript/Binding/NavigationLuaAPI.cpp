#include "GetPush.h"
#include "../../Navigation/NavigationMesh.h"
#include "../../Navigation/DynamicNavigationMesh.h"
#include "../../Navigation/Navigable.h"
#include "../../Navigation/Obstacle.h"
#include "../../Navigation/OffMeshConnection.h"
#include "../../Navigation/CrowdAgent.h"
#include "../../Navigation/CrowdManager.h"
#include "../../Navigation/NavigationEvents.h"
#include "../../Navigation/AStar/AStar.h"

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
    bindNavigationMesh["GetAllTileIndices"] = [](NavigationMesh* self) {
        auto tileIndices = self->GetAllTileIndices();
        return std::vector<IntVector2>(tileIndices.begin(), tileIndices.end());
    };

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

    auto eventType = lua["EventType"].get_or_create<sol::table>();
    eventType["CrowdAgentReposition"]       = E_CROWD_AGENT_REPOSITION;
    eventType["CrowdAgentFailure"]          = E_CROWD_AGENT_FAILURE;
    eventType["CrowdAgentFormation"]        = E_CROWD_AGENT_FORMATION;

    auto paramType = lua["ParamType"].get_or_create<sol::table>();
    auto crowdAgentReposition = paramType["CrowdAgentReposition"].get_or_create<sol::table>();
    crowdAgentReposition["Node"]            = CrowdAgentReposition::P_NODE;
    crowdAgentReposition["CrowdAgent"]      = CrowdAgentReposition::P_CROWD_AGENT;
    crowdAgentReposition["Position"]        = CrowdAgentReposition::P_POSITION;
    crowdAgentReposition["Velocity"]        = CrowdAgentReposition::P_VELOCITY;

    auto crowdAgentFailure = paramType["CrowdAgentFailure"].get_or_create<sol::table>();
    crowdAgentFailure["CrowdAgent"]         = CrowdAgentFailure::P_CROWD_AGENT;
    crowdAgentFailure["Position"]           = CrowdAgentFailure::P_POSITION;
    crowdAgentFailure["Velocity"]           = CrowdAgentFailure::P_VELOCITY;
    crowdAgentFailure["CrowdAgentState"]    = CrowdAgentFailure::P_CROWD_AGENT_STATE;
    crowdAgentFailure["CrowdTargetState"]   = CrowdAgentFailure::P_CROWD_TARGET_STATE;

    auto crowdAgentFormation = paramType["CrowdAgentFormation"].get_or_create<sol::table>();
    crowdAgentFormation["Index"]        = CrowdAgentFormation::P_INDEX;
    crowdAgentFormation["Size"]         = CrowdAgentFormation::P_SIZE;
    crowdAgentFormation["Position"]     = CrowdAgentFormation::P_POSITION;

    lua.new_enum("NavigationQuality",
        "LOW",      NAVIGATIONQUALITY_LOW,
        "MEDIUM",   NAVIGATIONQUALITY_MEDIUM,
        "HIGH",     NAVIGATIONQUALITY_HIGH);

    lua.new_enum("CrowdAgentState",
        "INVALID",  CA_STATE_INVALID,
        "WALKING",  CA_STATE_WALKING,
        "OFFMESH",  CA_STATE_OFFMESH);

    // a star
    auto astar = lua.new_usertype<AStar::Generator>("AStar",
        sol::call_constructor, sol::factories([context]() { return std::make_unique<AStar::Generator>(); }),
        "SetWorldSize", [](AStar::Generator* self, int xdim, int ydim) { self->setWorldSize({ xdim, ydim }); },
        "SetDiagonalMovement", &AStar::Generator::setDiagonalMovement,
        "SetHeuristic", [](AStar::Generator* self, int type) {
            if (type == 0) {
                self->setHeuristic(AStar::Heuristic::manhattan);
            } else if (type == 1) {
                self->setHeuristic(AStar::Heuristic::euclidean);
            } else if (type == 2) {
                self->setHeuristic(AStar::Heuristic::octagonal);
            }
        },
        "FindPath", [](AStar::Generator* self, int sx, int sy, int tx, int ty) {
            std::vector<int> ret;
            self->findPath({sx, sy}, {tx, ty}, &ret);
            return ret;
        }
        );
    astar["Manhattan"] = sol::var(0);
    astar["Euclidean"] = sol::var(1);
    astar["Octagonal"] = sol::var(2);
    return 0;
}
