#include "GetPush.h"
#include "../../Core/Context.h"
#include "../../Resource/Resource.h"
#include "../../Resource/ResourceCache.h"
#include "../../Graphics/Graphics.h"
#include "../../Graphics/Renderer.h"
#include "../../Graphics/Camera.h"
#include "../../Graphics/Texture2D.h"
#include "../../Graphics/TextureCube.h"
#include "../../Graphics/Drawable.h"
#include "../../Graphics/StaticModel.h"
#include "../../Graphics/Animation.h"
#include "../../Graphics/AnimationState.h"
#include "../../Graphics/AnimationController.h"
#include "../../Graphics/AnimatedModel.h"
#include "../../Graphics/Model.h"
#include "../../Graphics/Skybox.h"
#include "../../Graphics/DecalSet.h"
#include "../../Graphics/BillboardSet.h"
#include "../../Graphics/Material.h"
#include "../../Graphics/Zone.h"
#include "../../Graphics/Octree.h"
#include "../../Graphics/Light.h"
#include "../../Graphics/Terrain.h"
#include "../../Graphics/DebugRenderer.h"
#include "../../Graphics/MeshLine.h"
#include "../../Graphics/Technique.h"
#include "../../Graphics/ProceduralSky.h"
#include "../../Graphics/OutlineGroup.h"
#include "../../Graphics/Geometry.h"
#include "../../Graphics/IndexBuffer.h"
#include "../../RenderPipeline/RenderPipeline.h"
#include "../../RenderAPI/RenderDevice.h"
#include "../../Scene/Scene.h"
#include "../../Scene/Component.h"
#include "../../UI/Text3D.h"
#define PAR_SHAPES_IMPLEMENTATION
#include "ThirdParty/par/par_shapes.h"

Urho3D::Context* GetContext(lua_State* L);
using namespace Urho3D;

Urho3D::Model* ParMeshToModel(Urho3D::Context* ctx, par_shapes_mesh* mesh, bool swithYZ)
{
    par_shapes_unweld(mesh, true);
    par_shapes_compute_normals(mesh);

    if (swithYZ)
    {
        // ccw to cw
        par_shapes_invert(mesh, 0, 0);
    }

    Urho3D::BoundingBox bb;
    float bbox[6];
    par_shapes_compute_aabb(mesh, bbox);
    bb.min_ = Urho3D::Vector3{bbox[0], bbox[2], bbox[1]};
    bb.max_ = Urho3D::Vector3{bbox[3], bbox[5], bbox[4]};

    // Get index data
    // 		std::vector<uint32_t> indexData;
    // 		indexData.resize(mesh->ntriangles * 3);
    // 		for (int i = 0; i < indexData.size(); ++i) {
    // 			indexData[i] = mesh->triangles[i];
    // 		}
    struct VertexData
    {
        Urho3D::Vector3 position;
        Urho3D::Vector3 normal;
        // 		Urho3D::Vector2 texcoord;
    };

    std::vector<VertexData> vertexData;
    vertexData.resize(mesh->npoints);
    if (swithYZ)
    {
        for (int i = 0; i < mesh->npoints; ++i)
        {
            const auto& pos = ((Urho3D::Vector3*)(mesh->points))[i];
            vertexData[i].position = {pos.x_, pos.z_, pos.y_};
            const auto& nor = ((Urho3D::Vector3*)(mesh->normals))[i];
            vertexData[i].normal = {nor.x_, nor.z_, nor.y_};
            // 		if (mesh->tcoords) {
            // 			vertexData[i].texcoord = ((Urho3D::Vector2*)(mesh->tcoords))[i];
            // 		}
        }
    }
    else
    {
        for (int i = 0; i < mesh->npoints; ++i)
        {
            vertexData[i].position = ((Urho3D::Vector3*)(mesh->points))[i];
            vertexData[i].normal = ((Urho3D::Vector3*)(mesh->normals))[i];
            // 		if (mesh->tcoords) {
            // 			vertexData[i].texcoord = ((Urho3D::Vector2*)(mesh->tcoords))[i];
            // 		}
        }
    }

    // 	Urho3D::GenerateTangents(vertexData.data(), sizeof(VertexData), indexData.data(), sizeof(unsigned short), 0,
    // indexData.size(), 		offsetof(VertexData, normal), offsetof(VertexData, texcoord), offsetof(VertexData, tangent));

    auto geometry = new Urho3D::Geometry(ctx);
    geometry->SetNumVertexBuffers(1);

    auto vbo = new Urho3D::VertexBuffer(ctx);
    vbo->SetShadowed(true);
    vbo->SetSize((unsigned int)vertexData.size(),
        {
            Urho3D::VertexElement(Urho3D::TYPE_VECTOR3, Urho3D::SEM_POSITION),
            Urho3D::VertexElement(Urho3D::TYPE_VECTOR3, Urho3D::SEM_NORMAL),
            // 		Urho3D::VertexElement(Urho3D::TYPE_VECTOR2, Urho3D::SEM_TEXCOORD)
        });
    vbo->Update(vertexData.data());

    auto ibo = new Urho3D::IndexBuffer(ctx);
    ibo->SetShadowed(true);
    ibo->SetSize(mesh->ntriangles * 3, false);
    ibo->Update(mesh->triangles);

    geometry->SetVertexBuffer(0, vbo);
    geometry->SetIndexBuffer(ibo);
    geometry->SetDrawRange(Urho3D::TRIANGLE_LIST, 0, ibo->GetIndexCount());

    par_shapes_free_mesh(mesh);

    auto model = new Urho3D::Model(ctx);
    model->SetNumGeometries(1);

    // for save to file, remove this for runtime model?
    model->SetVertexBuffers({SharedPtr<VertexBuffer>{vbo}}, {}, {});
    model->SetIndexBuffers({SharedPtr<IndexBuffer>{ibo}});
    //
    model->SetGeometry(0, 0, geometry);
    model->SetBoundingBox(bb);
    return model;
}

static void RegisterConst(sol::state& lua)
{
    lua["LIGHT_DIRECTIONAL"]	= LIGHT_DIRECTIONAL;
    lua["LIGHT_SPOT"]			= LIGHT_SPOT;
    lua["LIGHT_POINT"]			= LIGHT_POINT;
    //
    lua["DEFAULT_RANGE"]                = DEFAULT_RANGE;
    lua["DEFAULT_LIGHT_FOV"]            = DEFAULT_LIGHT_FOV;
    lua["DEFAULT_SPECULARINTENSITY"]    = DEFAULT_SPECULARINTENSITY;
    lua["DEFAULT_BRIGHTNESS"]           = DEFAULT_BRIGHTNESS;
    lua["DEFAULT_CONSTANTBIAS"]         = DEFAULT_CONSTANTBIAS;
    lua["DEFAULT_SLOPESCALEDBIAS"]      = DEFAULT_SLOPESCALEDBIAS;
    lua["DEFAULT_NORMALOFFSET"]         = DEFAULT_NORMALOFFSET;
    lua["DEFAULT_BIASAUTOADJUST"]       = DEFAULT_BIASAUTOADJUST;
    lua["DEFAULT_SHADOWFADESTART"]      = DEFAULT_SHADOWFADESTART;
    lua["DEFAULT_SHADOWQUANTIZE"]       = DEFAULT_SHADOWQUANTIZE;
    lua["DEFAULT_SHADOWMINVIEW"]        = DEFAULT_SHADOWMINVIEW;
    lua["DEFAULT_SHADOWNEARFARRATIO"]   = DEFAULT_SHADOWNEARFARRATIO;
    lua["DEFAULT_SHADOWMAXEXTRUSION"]   = DEFAULT_SHADOWMAXEXTRUSION;
    lua["DEFAULT_SHADOWSPLIT"]          = DEFAULT_SHADOWSPLIT;
    lua["DEFAULT_TEMPERATURE"]          = DEFAULT_TEMPERATURE;
//     lua["DEFAULT_RADIUS"]               = DEFAULT_RADIUS;
//     lua["DEFAULT_LENGTH"]               = DEFAULT_LENGTH;
    lua["FC_NONE"]                      = FC_NONE;
    lua["FC_ROTATE_XYZ"]                = FC_ROTATE_XYZ;
    lua["FC_ROTATE_Y"]                  = FC_ROTATE_Y;
    lua["FC_LOOKAT_XYZ"]                = FC_LOOKAT_XYZ;
    lua["FC_LOOKAT_Y"]                  = FC_LOOKAT_Y;
    lua["FC_LOOKAT_MIXED"]              = FC_LOOKAT_MIXED;
    lua["FC_DIRECTION"]                 = FC_DIRECTION;
    lua["FC_AXIS_ANGLE"]                = FC_AXIS_ANGLE;
    //
    auto GIType = lua["GlobalIlluminationType"].get_or_create<sol::table>();
    GIType["None"] = GlobalIlluminationType::None;
    GIType["UseLightMap"] = GlobalIlluminationType::UseLightMap;
    GIType["BlendLightProbes"] = GlobalIlluminationType::BlendLightProbes;
    auto graphic = lua["graphic"].get_or_create<sol::table>();
    //graphic["TU_DIFFUSE"] = TU_DIFFUSE;
    //graphic["TU_ENVIRONMENT"] = TU_ENVIRONMENT;
    graphic["FILTER_BILINEAR"] = FILTER_BILINEAR;
    //graphic["TEXTURE_RENDERTARGET"] = TEXTURE_RENDERTARGET;
    //
    graphic["RAY_AABB"] = RAY_AABB;
    graphic["RAY_OBB"] = RAY_OBB;
    graphic["RAY_TRIANGLE"] = RAY_TRIANGLE;
    graphic["RAY_TRIANGLE_UV"] = RAY_TRIANGLE_UV;
    //
    graphic["DRAWABLE_UNDEFINED"] = DRAWABLE_UNDEFINED;
    graphic["DRAWABLE_GEOMETRY"] = DRAWABLE_GEOMETRY;
    graphic["DRAWABLE_LIGHT"] = DRAWABLE_LIGHT;
    graphic["DRAWABLE_ZONE"] = DRAWABLE_ZONE;
    graphic["DRAWABLE_GEOMETRY2D"] = DRAWABLE_GEOMETRY2D;
    graphic["DRAWABLE_ANY"] = DRAWABLE_ANY;
    //
    graphic["DEFAULT_VIEWMASK"] = DEFAULT_VIEWMASK;
}

int sol2_GraphicsLuaAPI_open(sol::state& lua)
{
	auto context = GetContext(lua.lua_state());
    auto graphic = lua["graphic"].get_or_create<sol::table>();
    lua.new_usertype<BiasParameters>("BiasParameters",
		sol::call_constructor, sol::factories([](float constantBias, float slopeScaledBias) { return BiasParameters(constantBias, slopeScaledBias); }));
    lua.new_usertype<CascadeParameters>("CascadeParameters",
		sol::call_constructor, sol::factories([](float split1, float split2, float split3, float split4, float fadeStart) { return CascadeParameters(split1, split2, split3, split4, fadeStart); }));

    auto bindMaterial = lua.new_usertype<Material>("Material",
        sol::call_constructor,
        sol::factories([context](const ea::string& vs, const ea::string& fs, const ea::string& passName) {
            auto tech = new Technique(context);
            auto pass = tech->CreatePass(passName);
            pass->SetVertexShader(vs);
            pass->SetPixelShader(fs);
            auto mtl = new Material(context);
            mtl->SetTechnique(0, tech);
            return mtl;
            }),
        sol::base_classes, sol::bases<Resource>());
    bindMaterial["scene"]           = sol::property(&Material::GetScene, &Material::SetScene);
    bindMaterial["render_order"]    = sol::property(&Material::GetRenderOrder, &Material::SetRenderOrder);
    bindMaterial["depth_bias"]      = sol::property(&Material::GetDepthBias, &Material::SetDepthBias);
    bindMaterial["fill_mode"]       = sol::property(&Material::GetFillMode, &Material::SetFillMode);
    bindMaterial["cull_mode"]       = sol::property(&Material::GetCullMode, &Material::SetCullMode);
    bindMaterial["occlusion"]       = sol::property(&Material::GetOcclusion, &Material::SetOcclusion);
    bindMaterial["shadow_cull_mode"] = sol::property(&Material::GetShadowCullMode, &Material::SetShadowCullMode);
    bindMaterial["Clone"]           = sol::overload(
        [](Material* self) { return self->Clone().Detach(); },
        [](Material* self, const ea::string& cloneName) { return self->Clone(cloneName).Detach(); });
    bindMaterial["SetTexture"]                          = [](Material* self, const char* name, Texture* texture) { self->SetTexture(name, texture); };
    bindMaterial["SetUVTransform"]                      = sol::overload(
        [](Material* self, const Vector2& offset, float rotation, const Vector2& repeat) { self->SetUVTransform(offset, rotation, repeat); },
        [](Material* self, const Vector2& offset, float rotation, float repeat) { self->SetUVTransform(offset, rotation, repeat); });
    bindMaterial["SetShaderParameter"]                  = sol::overload(
        [](Material* self, const ea::string& name, const Variant& value) { self->SetShaderParameter(name, value); },
        [](Material* self, const ea::string& name, const Variant& value, bool isCustom) { self->SetShaderParameter(name, value, isCustom); });
    bindMaterial["SetShaderParameterAnimation"]         = sol::overload(
        [](Material* self, const ea::string& name, ValueAnimation* valueAnimation) { self->SetShaderParameterAnimation(name, valueAnimation); },
        [](Material* self, const ea::string& name, ValueAnimation* valueAnimation, WrapMode wrapMode, float speed) { self->SetShaderParameterAnimation(name, valueAnimation, wrapMode, speed); });
    bindMaterial["SetShaderParameterAnimationWrapMode"] = &Material::SetShaderParameterAnimationWrapMode;
    bindMaterial["SetShaderParameterAnimationSpeed"]    = &Material::SetShaderParameterAnimationSpeed;
        
	lua.new_usertype<ResourceWithMetadata>("ResourceWithMetadata",
        sol::constructors<ResourceWithMetadata(Context*)>(),
		sol::base_classes, sol::bases<Resource>());

    auto bindAnimation = lua.new_usertype<Animation>("Animation",
		sol::base_classes, sol::bases<ResourceWithMetadata>());
    bindAnimation["length"]         = sol::property(&Animation::GetLength, &Animation::SetLength);

    auto bindModel = lua.new_usertype<Model>("Model", sol::constructors<Model(Context*)>(),
		sol::base_classes, sol::bases<ResourceWithMetadata>());
    bindModel["GetNumGeometries"]   = &Model::GetNumGeometries;
    bindModel["HideGeometry"]       = [](Model* self, unsigned geomIndex) { self->GetGeometry(geomIndex, 0)->SetIndexBuffer(nullptr); };
    bindModel["CreateRock"]         = [context](int seed, int nsubdivisions) {
        auto par_mesh = par_shapes_create_rock(seed, nsubdivisions);
        return ParMeshToModel(context, par_mesh, false);
    };

	lua.new_usertype<Texture>("Texture", sol::constructors<Texture(Context*)>(),
		sol::base_classes, sol::bases<ResourceWithMetadata>());

    auto bindTexture2D                  = lua.new_usertype<Texture2D>("Texture2D", sol::call_constructor,
        sol::factories([context]() {
            // TODO:for water demo, the texture will be managed by engine
            return new Texture2D(context);
            // lua manage the object
            //return std::make_unique<Texture2D>(context);
            }),
        sol::base_classes, sol::bases<Texture>()
    );
    bindTexture2D["width"]              = sol::property(&Texture2D::GetWidth);
	bindTexture2D["height"]             = sol::property(&Texture2D::GetHeight);
    bindTexture2D["filter_mode"]        = sol::property(&Texture2D::GetFilterMode, &Texture2D::SetFilterMode);
    bindTexture2D["GetRenderSurface"]   = &Texture2D::GetRenderSurface;
    bindTexture2D["SaveFile"]           = [](Texture2D* self, const ea::string& filename) {
        static SharedPtr<Image> image;
        if (filename.empty()) {
            image = self->GetImage();
        } else {
            image->SaveFile({ "file", filename });
            image = nullptr;
        }
    };
    bindTexture2D["SetSize"]            = sol::overload(
        [](Texture2D* self, int width, int height, unsigned format) { self->SetSize(width, height, (TextureFormat)format); },
        [](Texture2D* self, int width, int height, unsigned format, TextureFlag usage) { self->SetSize(width, height, (TextureFormat)format, usage); },
        [](Texture2D* self, int width, int height, unsigned format, TextureFlag usage, int multiSample) { self->SetSize(width, height, (TextureFormat)format, usage, multiSample); }
    );
		
    lua.new_usertype<TextureCube>("TextureCube", sol::base_classes, sol::bases<Texture>());

    auto bindRenderSurface = lua.new_usertype<RenderSurface>("RenderSurface");
    bindRenderSurface["SetViewport"]    = &RenderSurface::SetViewport;
    bindRenderSurface["QueueUpdate"]    = &RenderSurface::QueueUpdate;
    bindRenderSurface["GetWidth"]       = &RenderSurface::GetWidth;
    bindRenderSurface["GetHeight"]      = &RenderSurface::GetHeight;

    auto bindOctree = lua.new_usertype<Octree>("Octree", sol::base_classes, sol::bases<Component>());
    bindOctree["id"]                    = sol::var(StringHash("Octree"));
    bindOctree["AddManualDrawable"]     = &Octree::AddManualDrawable;
    bindOctree["RemoveManualDrawable"]  = &Octree::RemoveManualDrawable;
    bindOctree["RaycastSingle"]         = [](Octree* self, const Ray& ray, RayQueryLevel level, float maxDistance, int drawableFlags) {
        ea::vector<RayQueryResult> results;
        RayOctreeQuery query(results, ray, level, maxDistance, (DrawableFlags)drawableFlags);
        self->RaycastSingle(query);
        Vector3 hitPos;
        Drawable* hitDrawable = nullptr;
        if (!results.empty()) {
            RayQueryResult& result = results[0];
            hitPos = result.position_;
            hitDrawable = result.drawable_;
        }
        return std::make_tuple(hitPos, hitDrawable);
    };
        
    auto bindViewport = lua.new_usertype<Viewport>("Viewport",
        sol::call_constructor, sol::factories(
            [context]() { return new Viewport(context); },
            [context](Scene* scene, Camera* camera) { return new Viewport(context, scene, camera); }));
    bindViewport["SetScene"] = &Viewport::SetScene;
    bindViewport["GetScene"] = &Viewport::GetScene;
    bindViewport["SetCamera"] = &Viewport::SetCamera;
    bindViewport["GetCamera"] = &Viewport::GetCamera;

    auto bindDebugRenderer = lua.new_usertype<DebugRenderer>("DebugRenderer", sol::base_classes, sol::bases<Component>());
    bindDebugRenderer["id"] = sol::var(StringHash("DebugRenderer"));
    bindDebugRenderer["AddLine"] = sol::overload(
            [](DebugRenderer* self, const Vector3& start, const Vector3& end, const Color& color) { self->AddLine(start, end, color); },
            [](DebugRenderer* self, const Vector3& start, const Vector3& end, const Color& color, bool depthTest) {self->AddLine(start, end, color, depthTest); });
    bindDebugRenderer["AddTriangle"] = sol::overload(
        [](DebugRenderer* self, const Vector3& v1, const Vector3& v2, const Vector3& v3, const Color& color) { self->AddTriangle(v1, v2, v3, color); },
        [](DebugRenderer* self, const Vector3& v1, const Vector3& v2, const Vector3& v3, const Color& color, bool depthTest) {self->AddTriangle(v1, v2, v3, color, depthTest); });
        
    auto bineMeshLineDesc = lua.new_usertype<MeshLine::LineDesc>("MeshLineDesc", sol::call_constructor, sol::factories([]() { return MeshLine::LineDesc(); }));
    bineMeshLineDesc["model_mat"]       = &MeshLine::LineDesc::model_mat;
    bineMeshLineDesc["color"]           = &MeshLine::LineDesc::color;
    bineMeshLineDesc["width"]           = &MeshLine::LineDesc::width;
    bineMeshLineDesc["attenuation"]     = &MeshLine::LineDesc::attenuation;
    bineMeshLineDesc["depth"]           = &MeshLine::LineDesc::depth;
    bineMeshLineDesc["depth_bias"]      = &MeshLine::LineDesc::depth_bias;
    bineMeshLineDesc["repeat"]          = &MeshLine::LineDesc::repeat;
    bineMeshLineDesc["visibility"]      = &MeshLine::LineDesc::visibility;
    bineMeshLineDesc["use_dash"]        = &MeshLine::LineDesc::use_dash;
    bineMeshLineDesc["dash_array"]      = &MeshLine::LineDesc::dash_array;
    bineMeshLineDesc["dash_offset"]     = &MeshLine::LineDesc::dash_offset;
    bineMeshLineDesc["dash_ratio"]      = &MeshLine::LineDesc::dash_ratio;
    bineMeshLineDesc["alpha_fade"]      = &MeshLine::LineDesc::alpha_fade;
    bineMeshLineDesc["texture"]         = &MeshLine::LineDesc::tex;
    bineMeshLineDesc["alpha_texture"]   = &MeshLine::LineDesc::alphaTex;
    bineMeshLineDesc["cache"]           = &MeshLine::LineDesc::cache;
    bineMeshLineDesc["visible"]         = &MeshLine::LineDesc::visible;

    auto bindMeshLine = lua.new_usertype<MeshLine>("MeshLine", sol::base_classes, sol::bases<Component>());
    bindMeshLine["id"]              = sol::var(StringHash("MeshLine"));
    bindMeshLine["SetDepthBias"]    = &MeshLine::SetDepthBias;
    bindMeshLine["RemoveLine"]      = &MeshLine::RemoveLine;
    bindMeshLine["BeginLines"]      = &MeshLine::BeginLines;
    bindMeshLine["AppendLine"]      = [](MeshLine* self, std::vector<Vector3> points) { self->AppendLine(std::move(points)); };
    bindMeshLine["EndLines"]        = &MeshLine::EndLines;
    bindMeshLine["AddGrid"]         = &MeshLine::AddGrid;
    bindMeshLine["AddLine"]         = sol::overload(
        [](MeshLine* self, const Vector3& start, const Vector3& end, const MeshLine::LineDesc& lineDesc) { return self->AppendLine(start, end, lineDesc); },
        [](MeshLine* self, std::vector<Vector3> points, const MeshLine::LineDesc& lineDesc) { return self->AppendLine(points, lineDesc); });
        
	auto bindCamera = lua.new_usertype<Camera>("Camera", sol::base_classes, sol::bases<Component>());
    bindCamera["id"]                    = sol::var(StringHash("Camera"));
    bindCamera["far_clip"]              = sol::property(&Camera::GetFarClip, &Camera::SetFarClip);
    bindCamera["near_clip"]             = sol::property(&Camera::GetNearClip, &Camera::SetNearClip);
    bindCamera["fov"]                   = sol::property(&Camera::GetFov, &Camera::SetFov);
    bindCamera["view_mask"]             = sol::property(&Camera::GetViewMask, &Camera::SetViewMask);
    bindCamera["auto_aspect_ratio"]     = sol::property(&Camera::GetAutoAspectRatio, &Camera::SetAutoAspectRatio);
    bindCamera["use_reflection"]        = sol::property(&Camera::GetUseReflection, &Camera::SetUseReflection);
    bindCamera["reflection_plane"]      = sol::property(&Camera::GetReflectionPlane, &Camera::SetReflectionPlane);
    bindCamera["use_clipping"]          = sol::property(&Camera::GetUseClipping, &Camera::SetUseClipping);
    bindCamera["clip_plane"]            = sol::property(&Camera::GetClipPlane, &Camera::SetClipPlane);
    bindCamera["aspect_ratio"]          = sol::property(&Camera::GetAspectRatio, &Camera::SetAspectRatio);
    bindCamera["WorldToScreenPoint"]    = &Camera::WorldToScreenPoint;
    bindCamera["ScreenToWorldPoint"]    = &Camera::ScreenToWorldPoint;
    bindCamera["GetScreenRay"]          = &Camera::GetScreenRay;
		
    auto bindGraphics = lua.new_usertype<Graphics>("Graphics");
    bindGraphics["SetWindowIcon"]       = &Graphics::SetWindowIcon;
    bindGraphics["windowTitle"]         = sol::property(&Graphics::GetWindowTitle, &Graphics::SetWindowTitle);
    bindGraphics["width"]               = sol::property(&Graphics::GetWidth);
    bindGraphics["height"]              = sol::property(&Graphics::GetHeight);
    bindGraphics["GetStats"] = [](Graphics* self) {
        const auto& stats = self->GetSubsystem<RenderDevice>()->GetStats();
        static ea::string ret;
        ret.clear();
        ret.append_sprintf("DP: %d, Triangles(Lines): %d", stats.numDraws_, stats.numPrimitives_);
        return ret; };// ea::string{ {}, "DP: %d, Triangles(Lines): %d", stats.numDraws_, stats.numPrimitives_ };

    auto bindRenderer = lua.new_usertype<Renderer>("Renderer", sol::constructors<Renderer(Context*)>());
    bindRenderer["SetViewport"] = &Renderer::SetViewport;
    bindRenderer["DrawDebugGeometry"] = &Renderer::DrawDebugGeometry;

	auto bindDrawable = lua.new_usertype<Drawable>("Drawable", sol::base_classes, sol::bases<Component>());
    bindDrawable["cast_shadows"]                = sol::property(&Drawable::GetCastShadows, &Drawable::SetCastShadows);
    bindDrawable["view_mask"]                   = sol::property(&Drawable::GetViewMask, &Drawable::SetViewMask);
    bindDrawable["light_mask"]                  = sol::property(&Drawable::GetLightMask, &Drawable::SetLightMask);
    bindDrawable["shadow_mask"]                 = sol::property(&Drawable::GetShadowMask, &Drawable::SetShadowMask);
    bindDrawable["IsInOctree"]                  = &Drawable::IsInOctree;
    bindDrawable["SetOccluder"]                 = &Drawable::SetOccluder;
    bindDrawable["GetBoundingBox"]              = &Drawable::GetBoundingBox;
    bindDrawable["SetCastShadows"]              = &Drawable::SetCastShadows;
    bindDrawable["GetWorldBoundingBox"]         = &Drawable::GetWorldBoundingBox;
    bindDrawable["SetGlobalIlluminationType"]   = &Drawable::SetGlobalIlluminationType;
		
    auto bindZone = lua.new_usertype<Zone>("Zone", sol::base_classes, sol::bases<Drawable, Component>());
    bindZone["id"]                      = sol::var(StringHash("Zone"));
    bindZone["bounding_box"]            = sol::property([](Zone* self) { return self->GetBoundingBox(); }, &Zone::SetBoundingBox);
    bindZone["ambient_color"]           = sol::property(&Zone::GetAmbientColor, &Zone::SetAmbientColor);
    bindZone["ambient_brightness"]      = sol::property(&Zone::GetAmbientBrightness, &Zone::SetAmbientBrightness);
    bindZone["background_brightness"]   = sol::property(&Zone::GetBackgroundBrightness, &Zone::SetBackgroundBrightness);
    bindZone["fog_color"]               = sol::property(&Zone::GetFogColor, &Zone::SetFogColor);
    bindZone["fog_start"]               = sol::property(&Zone::GetFogStart, &Zone::SetFogStart);
    bindZone["fog_end"]                 = sol::property(&Zone::GetFogEnd, &Zone::SetFogEnd);
    //"SetProceduralSky", &Zone::SetProceduralSky,
    bindZone["SetZoneTextureAttr"]      = [](Zone* self, const ea::string& filename) { filename.empty() ? self->SetZoneTexture(nullptr) : self->SetZoneTextureAttr(ResourceRef(StringHash("TextureCube"), filename)); };
		
	auto bindLight = lua.new_usertype<Light>("Light", sol::base_classes, sol::bases<Drawable, Component>());
    bindLight["id"]                     = sol::var(StringHash("Light"));
    bindLight["light_type"]             = sol::property(&Light::GetLightType, &Light::SetLightType);
    bindLight["range"]                  = sol::property(&Light::GetRange, &Light::SetRange);
    bindLight["color"]                  = sol::property(&Light::GetColor, &Light::SetColor);
    bindLight["brightness"]             = sol::property(&Light::GetBrightness, &Light::SetBrightness);
    bindLight["indirect_brightness"]    = sol::property(&Light::GetIndirectBrightness, &Light::SetIndirectBrightness);
    bindLight["fov"]                    = sol::property(&Light::GetFov, &Light::SetFov);
    bindLight["ramp_texture"]           = sol::property(&Light::GetRampTexture, &Light::SetRampTexture);
    bindLight["shadow_bias"]            = sol::property(&Light::GetShadowBias, &Light::SetShadowBias);
    bindLight["shadow_cascade"]         = sol::property(&Light::GetShadowCascade, &Light::SetShadowCascade);
    bindLight["specular_intensity"]     = sol::property(&Light::GetSpecularIntensity, &Light::SetSpecularIntensity);
    bindLight["shadow_distance"]        = sol::property(&Light::GetShadowDistance, &Light::SetShadowDistance);
    bindLight["shadow_fade_distance"]   = sol::property(&Light::GetShadowFadeDistance, &Light::SetShadowFadeDistance);
    bindLight["shadow_resolution"]      = sol::property(&Light::GetShadowResolution, &Light::SetShadowResolution);
    bindLight["shadow_near_far_ratio"]  = sol::property(&Light::GetShadowNearFarRatio, &Light::SetShadowNearFarRatio);
		
    auto bindStaticModel = lua.new_usertype<StaticModel>("StaticModel", sol::base_classes, sol::bases<Drawable, Component>());
    bindStaticModel["id"]           = sol::var(StringHash("StaticModel"));
    bindStaticModel["model"]        = sol::property(&StaticModel::GetModel, &StaticModel::SetModel);
    bindStaticModel["material"]     = sol::property([](StaticModel* self) { return self->GetMaterial(0); }, [](StaticModel* self, Material* mtl) { self->SetMaterial(mtl); });
    bindStaticModel["SetModel"]     = &StaticModel::SetModel;
    bindStaticModel["SetMaterial"]  = sol::overload(
        sol::resolve<void(Material*)>(&StaticModel::SetMaterial),
        sol::resolve<bool(unsigned, Material*)>(&StaticModel::SetMaterial));
    bindStaticModel["GetMaterial"]  = sol::overload(
        sol::resolve<Material * (void) const>(&StaticModel::GetMaterial),
        sol::resolve<Material * (unsigned) const>(&StaticModel::GetMaterial));
    bindStaticModel["RaycastSingle"] = [](StaticModel* self, const Ray& ray) {
        ea::vector<RayQueryResult> results;
        RayOctreeQuery query(results, ray);
        self->ProcessRayQuery(query, results);
        Vector3 hitPos;
        bool hit = false;
        if (!results.empty()) {
            RayQueryResult& result = results[0];
            hitPos = result.position_;
            hit = true;
        }
        return std::make_tuple(hit, hitPos);
    };
        
    auto bindAnimationState = lua.new_usertype<AnimationState>("AnimationState");
    bindAnimationState["weight"] = sol::property(&AnimationState::GetWeight, &AnimationState::SetWeight);
    bindAnimationState["looped"] = sol::property(&AnimationState::IsLooped, &AnimationState::SetLooped);
	bindAnimationState["time"] = sol::property(&AnimationState::GetTime, &AnimationState::SetTime);

	auto bindAnimatedModel = lua.new_usertype<AnimatedModel>("AnimatedModel", sol::base_classes, sol::bases<StaticModel, Drawable, Component>());
    bindAnimatedModel["id"] = sol::var(StringHash("AnimatedModel"));
    bindAnimatedModel["model"] = sol::property(&AnimatedModel::GetModel, [](AnimatedModel* self, Model* model) { self->SetModel(model); });

    auto bindAnimationParameters = lua.new_usertype<AnimationParameters>("AnimationParameters", sol::call_constructor, sol::factories(
            [](Animation* animation) { return AnimationParameters(animation); },
            [context](const ea::string& animationName) { return AnimationParameters(context, animationName); }));
    bindAnimationParameters["weight"]           = &AnimationParameters::weight_;
    bindAnimationParameters["Looped"]           = &AnimationParameters::Looped;
    bindAnimationParameters["StartBone"]        = &AnimationParameters::StartBone;
    bindAnimationParameters["Layer"]            = &AnimationParameters::Layer;
    bindAnimationParameters["Time"]             = &AnimationParameters::Time;
    bindAnimationParameters["Additive"]         = &AnimationParameters::Additive;
    bindAnimationParameters["Weight"]           = &AnimationParameters::Weight;
    bindAnimationParameters["Speed"]            = &AnimationParameters::Speed;
    bindAnimationParameters["AutoFadeOut"]      = &AnimationParameters::AutoFadeOut;
    bindAnimationParameters["KeepOnCompletion"] = &AnimationParameters::KeepOnCompletion;
    bindAnimationParameters["KeepOnZeroWeight"] = &AnimationParameters::KeepOnZeroWeight;

	auto bindAnimationController = lua.new_usertype<AnimationController>("AnimationController", sol::base_classes, sol::bases<AnimationStateSource, Component>());
    bindAnimationController["id"]                   = sol::var(StringHash("AnimationController"));
    bindAnimationController["SetSpeed"]             = &AnimationController::SetSpeed;
    bindAnimationController["SetWeight"]            = &AnimationController::SetWeight;
    bindAnimationController["SetTime"]              = &AnimationController::SetTime;
    bindAnimationController["PlayNewExclusive"]     = sol::overload(
        [](AnimationController* self, const AnimationParameters& params) { return self->PlayNewExclusive(params); },
        [](AnimationController* self, const AnimationParameters& params, float fadeInTime) { return self->PlayNewExclusive(params, fadeInTime); });
    bindAnimationController["PlayNew"]              = sol::overload(
        [](AnimationController* self, const AnimationParameters& params) { return self->PlayNew(params); },
        [](AnimationController* self, const AnimationParameters& params, float fadeInTime) { return self->PlayNew(params, fadeInTime); });
    bindAnimationController["PlayExistingExclusive"] = sol::overload(
        [](AnimationController* self, const AnimationParameters& params) { return self->PlayExistingExclusive(params); },
        [](AnimationController* self, const AnimationParameters& params, float fadeInTime) { return self->PlayExistingExclusive(params, fadeInTime); });
    bindAnimationController["PlayExisting"]         = sol::overload(
        [](AnimationController* self, const AnimationParameters& params) { return self->PlayExisting(params); },
        [](AnimationController* self, const AnimationParameters& params, float fadeInTime) { return self->PlayExisting(params, fadeInTime); });
    bindAnimationController["Stop"]                 = sol::overload(
        [](AnimationController* self, Animation* animation) { return self->Stop(animation); },
        [](AnimationController* self, Animation* animation, float fadeTime) { return self->Stop(animation, fadeTime); });
    bindAnimationController["IsPlaying"]            = sol::overload(
        sol::resolve<bool(const ea::string&)const>(&AnimationController::IsPlaying),
        sol::resolve<bool(Animation*)const>(&AnimationController::IsPlaying));
    bindAnimationController["Fade"]                         = sol::overload(sol::resolve<Animation*, float, float >(&AnimationController::Fade));
    bindAnimationController["UpdateAnimationTime"]          = &AnimationController::UpdateAnimationTime;
    bindAnimationController["UpdateAnimationWeight"]        = sol::overload(
        [](AnimationController* self, Animation* animation, float weight) { return self->UpdateAnimationWeight(animation, weight); },
        [](AnimationController* self, Animation* animation, float weight, float fadeTime) { return self->UpdateAnimationWeight(animation, weight, fadeTime); });
    bindAnimationController["UpdateAnimationSpeed"]         = &AnimationController::UpdateAnimationSpeed;
    bindAnimationController["GetAnimationParameters"]       = sol::resolve<const AnimationParameters & (unsigned) const>(&AnimationController::GetAnimationParameters);
    bindAnimationController["GetLastAnimationParameters"]   = sol::overload(
        [](AnimationController* self, Animation* animation) { return self->GetLastAnimationParameters(animation); },
        [](AnimationController* self, Animation* animation, unsigned layer) { return self->GetLastAnimationParameters(animation, layer); });
        
	lua.new_usertype<Skybox>("Skybox",
        "id", sol::var(StringHash("Skybox")),
        sol::base_classes, sol::bases<StaticModel, Drawable, Component>());

    auto bindDecalSet = lua.new_usertype<DecalSet>("DecalSet", sol::base_classes, sol::bases<Drawable, Component>());
    bindDecalSet["id"] = sol::var(StringHash("DecalSet"));
    bindDecalSet["material"] = sol::property(&DecalSet::GetMaterial, &DecalSet::SetMaterial);
    bindDecalSet["AddDecal"] = sol::overload(
        [](DecalSet* self, Drawable* target, const Vector3& worldPosition, const Quaternion& worldRotation, float size, float aspectRatio, float depth, const Vector2& topLeftUV, const Vector2& bottomRightUV) {
            return self->AddDecal(target, worldPosition, worldRotation, size, aspectRatio, depth, topLeftUV, bottomRightUV); },
        [](DecalSet* self, Drawable* target, const Vector3& worldPosition, const Quaternion& worldRotation, float size, float aspectRatio, float depth, const Vector2& topLeftUV, const Vector2& bottomRightUV, float timeToLive) {
            return self->AddDecal(target, worldPosition, worldRotation, size, aspectRatio, depth, topLeftUV, bottomRightUV, timeToLive); },
        [](DecalSet* self, Drawable* target, const Vector3& worldPosition, const Quaternion& worldRotation, float size, float aspectRatio, float depth, const Vector2& topLeftUV, const Vector2& bottomRightUV, float timeToLive, float normalCutoff) {
            return self->AddDecal(target, worldPosition, worldRotation, size, aspectRatio, depth, topLeftUV, bottomRightUV, timeToLive, normalCutoff); });
    bindDecalSet["RemoveDecals"] = &DecalSet::RemoveDecals;
    bindDecalSet["RemoveAllDecals"] = &DecalSet::RemoveAllDecals;
        
    auto bindBillboard = lua.new_usertype<Billboard>("Billboard");
    bindBillboard["position"]   = &Billboard::position_;
    bindBillboard["size"]       = &Billboard::size_;
    bindBillboard["rotation"]   = &Billboard::rotation_;
    bindBillboard["enabled"]    = &Billboard::enabled_;

    auto bindBillboardSet = lua.new_usertype<BillboardSet>("BillboardSet", sol::base_classes, sol::bases<Drawable, Component>());
    bindBillboardSet["id"]                  = sol::var(StringHash("BillboardSet"));
    bindBillboardSet["num_billboards"]      = sol::property(&BillboardSet::GetNumBillboards, &BillboardSet::SetNumBillboards);
    bindBillboardSet["material"]            = sol::property(&BillboardSet::GetMaterial, &BillboardSet::SetMaterial);
    bindBillboardSet["sorted"]              = sol::property(&BillboardSet::IsSorted, &BillboardSet::SetSorted);
    bindBillboardSet["GetBillboard"]        = &BillboardSet::GetBillboard;
    bindBillboardSet["Commit"]              = &BillboardSet::Commit;
    bindBillboardSet["SetFaceCameraMode"]   = &BillboardSet::SetFaceCameraMode;
        
    auto bindTerrain = lua.new_usertype<Terrain>("Terrain", sol::base_classes, sol::bases<Component>());
    bindTerrain["id"]           = sol::var(StringHash("Terrain"));
    bindTerrain["patch_size"]   = sol::property(&Terrain::GetPatchSize, &Terrain::SetPatchSize);
    bindTerrain["spacing"]      = sol::property(&Terrain::GetSpacing, &Terrain::SetSpacing);
    bindTerrain["smoothing"]    = sol::property(&Terrain::GetSmoothing, &Terrain::SetSmoothing);
    bindTerrain["height_map"]   = sol::property(&Terrain::GetHeightMap, &Terrain::SetHeightMap);
    bindTerrain["height_map"]   = sol::property(&Terrain::GetHeightMap, &Terrain::SetHeightMap);
    bindTerrain["material"]     = sol::property(&Terrain::GetMaterial, &Terrain::SetMaterial);
    bindTerrain["occluder"]     = sol::property(&Terrain::IsOccluder, &Terrain::SetOccluder);
    bindTerrain["GetHeight"]    = &Terrain::GetHeight;
    bindTerrain["GetNormal"]    = &Terrain::GetNormal;

    lua.new_usertype<RenderPipeline>("RenderPipeline",
        "id", sol::var(StringHash("RenderPipeline")),
        sol::base_classes, sol::bases<Component>());

    auto bindProceduralSky = lua.new_usertype<ProceduralSky>("ProceduralSky", sol::base_classes, sol::bases<StaticModel, Drawable, Component>());
    bindProceduralSky["id"]                     = sol::var(StringHash("ProceduralSky"));
    bindProceduralSky["Init"]                   = sol::overload(
        [](ProceduralSky* self, uint32_t verticalCount, uint32_t horizontalCount, ProceduralSky::Month month, float time) { self->Init(verticalCount, horizontalCount, month, time); },
        [](ProceduralSky* self, uint32_t verticalCount, uint32_t horizontalCount, ProceduralSky::Month month, float time, uint32_t cubemapSize, const Vector3& northDir) { self->Init(verticalCount, horizontalCount, month, time, cubemapSize, northDir); });
    bindProceduralSky["January"]                = sol::var(ProceduralSky::January);
    bindProceduralSky["February"]               = sol::var(ProceduralSky::February);
    bindProceduralSky["April"]                  = sol::var(ProceduralSky::April);
    bindProceduralSky["May"]                    = sol::var(ProceduralSky::May);
    bindProceduralSky["June"]                   = sol::var(ProceduralSky::June);
    bindProceduralSky["July"]                   = sol::var(ProceduralSky::July);
    bindProceduralSky["August"]                 = sol::var(ProceduralSky::August);
    bindProceduralSky["September"]              = sol::var(ProceduralSky::September);
    bindProceduralSky["October"]                = sol::var(ProceduralSky::October);
    bindProceduralSky["November"]               = sol::var(ProceduralSky::November);
    bindProceduralSky["December"]               = sol::var(ProceduralSky::December);
    bindProceduralSky["SetMonth"]               = &ProceduralSky::SetMonth;
    bindProceduralSky["SetTime"]                = &ProceduralSky::SetTime;
    bindProceduralSky["SetTimeScale"]           = &ProceduralSky::SetTimeScale;
    bindProceduralSky["GetSunDirection"]        = &ProceduralSky::GetSunDirection;
    bindProceduralSky["GetSunLuminance"]        = &ProceduralSky::GetSunLuminance;
    bindProceduralSky["GetSunLuminanceGamma"]   = &ProceduralSky::GetSunLuminanceGamma;
    bindProceduralSky["GetSkyLuminance"]        = &ProceduralSky::GetSkyLuminance;
    bindProceduralSky["GetSkyLuminanceGamma"]   = &ProceduralSky::GetSkyLuminanceGamma;

    auto bindOutlineGroup = lua.new_usertype<OutlineGroup>("OutlineGroup", sol::base_classes, sol::bases<Component>());
    bindOutlineGroup["id"]                  = sol::var(StringHash("OutlineGroup"));
    bindOutlineGroup["SetColor"]            = &OutlineGroup::SetColor;
    bindOutlineGroup["GetOutlineMaterial"]  = &OutlineGroup::GetOutlineMaterial;
    bindOutlineGroup["HasDrawables"]        = &OutlineGroup::HasDrawables;
    bindOutlineGroup["ContainsDrawable"]    = &OutlineGroup::ContainsDrawable;
    bindOutlineGroup["ClearDrawables"]      = &OutlineGroup::ClearDrawables;
    bindOutlineGroup["AddDrawable"]         = &OutlineGroup::AddDrawable;
    bindOutlineGroup["RemoveDrawable"]      = &OutlineGroup::RemoveDrawable;
        
    auto bindText3D = lua.new_usertype<Text3D>("Text3D", sol::base_classes, sol::bases<Drawable, Component>());
    bindText3D["id"]                = sol::var(StringHash("Text3D"));
    bindText3D["material"]          = sol::property(&Text3D::GetMaterial, &Text3D::SetMaterial);
    bindText3D["SetFont"]           = [](Text3D* self, const ea::string& fontName) { self->SetFont(fontName); };
    bindText3D["SetFontSize"]       = &Text3D::SetFontSize;
    bindText3D["SetText"]           = &Text3D::SetText;
    bindText3D["SetColor"]          = sol::resolve<void(const Color&)>(&Text3D::SetColor);
    bindText3D["SetOpacity"]        = &Text3D::SetOpacity;
    bindText3D["SetFaceCameraMode"] = &Text3D::SetFaceCameraMode;
        
	lua["graphics_system"] = context->GetSubsystem<Graphics>();
	lua["renderer_system"] = context->GetSubsystem<Renderer>();
	RegisterConst(lua);
	
	return 0;
}
