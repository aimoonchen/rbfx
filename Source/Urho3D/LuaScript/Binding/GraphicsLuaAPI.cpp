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
    lua.new_usertype<Material>("Material",
        sol::call_constructor, sol::factories([context](const ea::string& vs, const ea::string& fs, const ea::string& passName) {
            auto tech = new Technique(context);
            auto pass = tech->CreatePass(passName);
            pass->SetVertexShader(vs);
            pass->SetPixelShader(fs);
            auto mtl = new Material(context);
            mtl->SetTechnique(0, tech);
            return mtl;
            }),
        "scene", sol::property(&Material::GetScene, &Material::SetScene),
        "render_order", sol::property(&Material::GetRenderOrder, &Material::SetRenderOrder),
        "depth_bias", sol::property(&Material::GetDepthBias, &Material::SetDepthBias),
        "fill_mode", sol::property(&Material::GetFillMode, &Material::SetFillMode),
        "cull_mode", sol::property(&Material::GetCullMode, &Material::SetCullMode),
        "occlusion", sol::property(&Material::GetOcclusion, &Material::SetOcclusion),
        "shadow_cull_mode", sol::property(&Material::GetShadowCullMode, &Material::SetShadowCullMode),
        "Clone", sol::overload(
            [](Material* self) { return self->Clone().Detach(); },
            [](Material* self, const ea::string& cloneName) { return self->Clone(cloneName).Detach(); }),
        "SetTexture", [](Material* self, const char* name, Texture* texture) { self->SetTexture(name, texture); },
        "SetUVTransform", sol::overload(
            [](Material* self, const Vector2& offset, float rotation, const Vector2& repeat) { self->SetUVTransform(offset, rotation, repeat); },
            [](Material* self, const Vector2& offset, float rotation, float repeat) { self->SetUVTransform(offset, rotation, repeat); }),
        "SetShaderParameter", sol::overload(
            [](Material* self, const ea::string& name, const Variant& value) { self->SetShaderParameter(name, value); },
            [](Material* self, const ea::string& name, const Variant& value, bool isCustom) { self->SetShaderParameter(name, value, isCustom); }),
        "SetShaderParameterAnimation", sol::overload(
            [](Material* self, const ea::string& name, ValueAnimation* valueAnimation) { self->SetShaderParameterAnimation(name, valueAnimation); },
            [](Material* self, const ea::string& name, ValueAnimation* valueAnimation, WrapMode wrapMode, float speed) { self->SetShaderParameterAnimation(name, valueAnimation, wrapMode, speed); }),
        "SetShaderParameterAnimationWrapMode", &Material::SetShaderParameterAnimationWrapMode,
        "SetShaderParameterAnimationSpeed", &Material::SetShaderParameterAnimationSpeed,
        sol::base_classes, sol::bases<Resource>());
	lua.new_usertype<ResourceWithMetadata>("ResourceWithMetadata", sol::constructors<ResourceWithMetadata(Context*)>(),
		sol::base_classes, sol::bases<Resource>());
    lua.new_usertype<Animation>("Animation",
		"length", sol::property(&Animation::GetLength, &Animation::SetLength),
		sol::base_classes, sol::bases<ResourceWithMetadata>());
    lua.new_usertype<Model>("Model", sol::constructors<Model(Context*)>(),
        "GetNumGeometries", &Model::GetNumGeometries,
        "HideGeometry", [](Model* self, unsigned geomIndex) { self->GetGeometry(geomIndex, 0)->SetIndexBuffer(nullptr); },
        "CreateRock", [context](int seed, int nsubdivisions) {
            auto par_mesh = par_shapes_create_rock(seed, nsubdivisions);
            return ParMeshToModel(context, par_mesh, false);
        },
		sol::base_classes, sol::bases<ResourceWithMetadata>());
	lua.new_usertype<Texture>("Texture", sol::constructors<Texture(Context*)>(),
		sol::base_classes, sol::bases<ResourceWithMetadata>());
    lua.new_usertype<Texture2D>("Texture2D",
        sol::call_constructor, sol::factories([context]() {
            // TODO:for water demo, the texture will be managed by engine
            return new Texture2D(context);
            // lua manage the object
            //return std::make_unique<Texture2D>(context);
            }),
		"width",sol::property(&Texture2D::GetWidth),
		"height", sol::property(&Texture2D::GetHeight),
        "filter_mode", sol::property(&Texture2D::GetFilterMode, &Texture2D::SetFilterMode),
        "render_surface", sol::property(&Texture2D::GetRenderSurface),
        "SaveFile", [](Texture2D* self, const ea::string& filename) {
            static SharedPtr<Image> image;
            if (filename.empty()) {
                image = self->GetImage();
            } else {
                image->SaveFile({ "file", filename });
                image = nullptr;
            }
        },
        "SetSize", sol::overload(
            [](Texture2D* self, int width, int height, unsigned format) { self->SetSize(width, height, (TextureFormat)format); },
            [](Texture2D* self, int width, int height, unsigned format, TextureFlag usage) { self->SetSize(width, height, (TextureFormat)format, usage); },
            [](Texture2D* self, int width, int height, unsigned format, TextureFlag usage, int multiSample) { self->SetSize(width, height, (TextureFormat)format, usage, multiSample); }),
		sol::base_classes, sol::bases<Texture>());
    lua.new_usertype<TextureCube>("TextureCube",
        sol::base_classes, sol::bases<Texture>());
    lua.new_usertype<RenderSurface>("RenderSurface",
        "SetViewport", &RenderSurface::SetViewport,
        "QueueUpdate", &RenderSurface::QueueUpdate,
        "GetWidth", &RenderSurface::GetWidth,
        "GetHeight", &RenderSurface::GetHeight
        );
    lua.new_usertype<Octree>("Octree",
        "id", sol::var(StringHash("Octree")),
        "AddManualDrawable", &Octree::AddManualDrawable,
        "RemoveManualDrawable", &Octree::RemoveManualDrawable,
        "RaycastSingle", [](Octree* self, const Ray& ray, RayQueryLevel level, float maxDistance, int drawableFlags) {
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
        },
        sol::base_classes, sol::bases<Component>()
        );
    lua.new_usertype<Viewport>("Viewport",
		sol::call_constructor, sol::factories([context]() { return new Viewport(context); },
			[context](Scene* scene, Camera* camera) { return new Viewport(context, scene, camera); }),
        "SetScene", &Viewport::SetScene,
        "GetScene", &Viewport::GetScene,
        "SetCamera", &Viewport::SetCamera,
        "GetCamera", &Viewport::GetCamera
        );
    lua.new_usertype<DebugRenderer>("DebugRenderer",
        "id", sol::var(StringHash("DebugRenderer")),
        "AddLine", sol::overload(
            [](DebugRenderer* self, const Vector3& start, const Vector3& end, const Color& color) { self->AddLine(start, end, color); },
            [](DebugRenderer* self, const Vector3& start, const Vector3& end, const Color& color, bool depthTest) {self->AddLine(start, end, color, depthTest); }),
        "AddTriangle", sol::overload(
            [](DebugRenderer* self, const Vector3& v1, const Vector3& v2, const Vector3& v3, const Color& color) { self->AddTriangle(v1, v2, v3, color); },
            [](DebugRenderer* self, const Vector3& v1, const Vector3& v2, const Vector3& v3, const Color& color, bool depthTest) {self->AddTriangle(v1, v2, v3, color, depthTest); }),
        sol::base_classes, sol::bases<Component>());
    lua.new_usertype<MeshLine::LineDesc>("MeshLineDesc",
        sol::call_constructor, sol::factories([]() { return MeshLine::LineDesc(); }),
        "model_mat", &MeshLine::LineDesc::model_mat,
        "color", &MeshLine::LineDesc::color,
        "width", &MeshLine::LineDesc::width,
        "attenuation", &MeshLine::LineDesc::attenuation,
        "depth", &MeshLine::LineDesc::depth,
        "depth_bias", &MeshLine::LineDesc::depth_bias,
        "repeat", &MeshLine::LineDesc::repeat,
        "visibility", &MeshLine::LineDesc::visibility,
        "use_dash", &MeshLine::LineDesc::use_dash,
        "dash_array", &MeshLine::LineDesc::dash_array,
        "dash_offset", &MeshLine::LineDesc::dash_offset,
        "dash_ratio", &MeshLine::LineDesc::dash_ratio,
        "alpha_fade", &MeshLine::LineDesc::alpha_fade,
        "texture", &MeshLine::LineDesc::tex,
        "alpha_texture", &MeshLine::LineDesc::alphaTex,
        "cache", &MeshLine::LineDesc::cache,
        "visible", &MeshLine::LineDesc::visible);
    lua.new_usertype<MeshLine>("MeshLine",
        "id", sol::var(StringHash("MeshLine")),
        "SetDepthBias", &MeshLine::SetDepthBias,
        "RemoveLine", &MeshLine::RemoveLine,
        "BeginLines", &MeshLine::BeginLines,
        "AppendLine", [](MeshLine* self, std::vector<Vector3> points) { self->AppendLine(std::move(points)); },
        "EndLines", &MeshLine::EndLines,
        "AddGrid", &MeshLine::AddGrid,
        "AddLine", sol::overload(
            [](MeshLine* self, const Vector3& start, const Vector3& end, const MeshLine::LineDesc& lineDesc) { return self->AppendLine(start, end, lineDesc); },
            [](MeshLine* self, std::vector<Vector3> points, const MeshLine::LineDesc& lineDesc) { return self->AppendLine(points, lineDesc); }),
        sol::base_classes, sol::bases<Component>());
	lua.new_usertype<Camera>("Camera",
        "id", sol::var(StringHash("Camera")),
		"far_clip", sol::property(&Camera::GetFarClip, &Camera::SetFarClip),
        "near_clip", sol::property(&Camera::GetNearClip, &Camera::SetNearClip),
		"fov", sol::property(&Camera::GetFov, &Camera::SetFov),
        "view_mask", sol::property(&Camera::GetViewMask, &Camera::SetViewMask),
        "auto_aspect_ratio", sol::property(&Camera::GetAutoAspectRatio, &Camera::SetAutoAspectRatio),
        "use_reflection", sol::property(&Camera::GetUseReflection, &Camera::SetUseReflection),
        "reflection_plane", sol::property(&Camera::GetReflectionPlane, &Camera::SetReflectionPlane),
        "use_clipping", sol::property(&Camera::GetUseClipping, &Camera::SetUseClipping),
        "clip_plane", sol::property(&Camera::GetClipPlane, &Camera::SetClipPlane),
        "aspect_ratio", sol::property(&Camera::GetAspectRatio, &Camera::SetAspectRatio),
        "WorldToScreenPoint", &Camera::WorldToScreenPoint,
        "ScreenToWorldPoint", &Camera::ScreenToWorldPoint,
        "GetScreenRay", &Camera::GetScreenRay,
		sol::base_classes, sol::bases<Component>());
	lua.new_usertype<Graphics>("Graphics",
		"SetWindowIcon", &Graphics::SetWindowIcon,
		"windowTitle", sol::property(&Graphics::GetWindowTitle, &Graphics::SetWindowTitle),
//         "GetRGBFormat", &Graphics::GetRGBFormat,
//         "SetSRGB", &Graphics::SetSRGB,
//         "GetSRGB", &Graphics::GetSRGB,
//         "GetSRGBSupport", &Graphics::GetSRGBSupport,
//         "GetSRGBWriteSupport", &Graphics::GetSRGBWriteSupport,
		"width", sol::property(&Graphics::GetWidth),
        "height", sol::property(&Graphics::GetHeight),
//         "GetNumPrimitives", &Graphics::GetNumPrimitives,
//         "GetNumBatches", &Graphics::GetNumBatches,
//         "SetBGFXCamera", &Graphics::SetBGFXCamera,
//         "TakeScreenShot", sol::resolve<bool(const ea::string&)>(&Graphics::TakeScreenShot),
        "GetStats", [](Graphics* self) {
            const auto& stats = self->GetSubsystem<RenderDevice>()->GetStats();
            static ea::string ret;
            ret.clear();
            ret.append_sprintf("DP: %d, Triangles(Lines): %d", stats.numDraws_, stats.numPrimitives_);
            return ret;// ea::string{ {}, "DP: %d, Triangles(Lines): %d", stats.numDraws_, stats.numPrimitives_ };
        }
        );
	lua.new_usertype<Renderer>("Renderer", sol::constructors<Renderer(Context*)>(),
		"SetViewport", &Renderer::SetViewport,
        "DrawDebugGeometry", &Renderer::DrawDebugGeometry);
	lua.new_usertype<Drawable>("Drawable",
		"cast_shadows", sol::property(&Drawable::GetCastShadows, &Drawable::SetCastShadows),
        "view_mask", sol::property(&Drawable::GetViewMask, &Drawable::SetViewMask),
        "light_mask", sol::property(&Drawable::GetLightMask, &Drawable::SetLightMask),
        "shadow_mask", sol::property(&Drawable::GetShadowMask, &Drawable::SetShadowMask),
        "SetCastShadows", &Drawable::SetCastShadows,
        "SetGlobalIlluminationType", &Drawable::SetGlobalIlluminationType,
        "IsInOctree", &Drawable::IsInOctree,
        "SetOccluder", &Drawable::SetOccluder,
        "GetBoundingBox", &Drawable::GetBoundingBox,
        "GetWorldBoundingBox", &Drawable::GetWorldBoundingBox,
		sol::base_classes, sol::bases<Component>());
    lua.new_usertype<Zone>("Zone",
        "id",           sol::var(StringHash("Zone")),
		"bounding_box",	sol::property([](Zone* self) { return self->GetBoundingBox(); }, &Zone::SetBoundingBox),
		"ambient_color", sol::property(&Zone::GetAmbientColor, &Zone::SetAmbientColor),
        "ambient_brightness", sol::property(&Zone::GetAmbientBrightness, &Zone::SetAmbientBrightness),
        "background_brightness", sol::property(&Zone::GetBackgroundBrightness, &Zone::SetBackgroundBrightness),
		"fog_color",		sol::property(&Zone::GetFogColor, &Zone::SetFogColor),
		"fog_start",		sol::property(&Zone::GetFogStart, &Zone::SetFogStart),
		"fog_end",		sol::property(&Zone::GetFogEnd, &Zone::SetFogEnd),
        //"SetProceduralSky", &Zone::SetProceduralSky,
        "SetZoneTextureAttr", [](Zone* self, const ea::string& filename) { filename.empty() ? self->SetZoneTexture(nullptr) : self->SetZoneTextureAttr(ResourceRef(StringHash("TextureCube"), filename)); },
		sol::base_classes, sol::bases<Drawable, Component>());
	lua.new_usertype<Light>("Light",
        "id", sol::var(StringHash("Light")),
		"light_type", sol::property(&Light::GetLightType, &Light::SetLightType),
        "range", sol::property(&Light::GetRange, &Light::SetRange),
		"color", sol::property(&Light::GetColor, &Light::SetColor),
        "brightness", sol::property(&Light::GetBrightness, &Light::SetBrightness),
        "indirect_brightness", sol::property(&Light::GetIndirectBrightness, &Light::SetIndirectBrightness),
        "fov", sol::property(&Light::GetFov, &Light::SetFov),
        "ramp_texture", sol::property(&Light::GetRampTexture, &Light::SetRampTexture),
		"shadow_bias", sol::property(&Light::GetShadowBias, &Light::SetShadowBias),
		"shadow_cascade", sol::property(&Light::GetShadowCascade, &Light::SetShadowCascade),
        "specular_intensity", sol::property(&Light::GetSpecularIntensity, &Light::SetSpecularIntensity),
        "shadow_distance", sol::property(&Light::GetShadowDistance, &Light::SetShadowDistance),
        "shadow_fade_distance", sol::property(&Light::GetShadowFadeDistance, &Light::SetShadowFadeDistance),
        "shadow_resolution", sol::property(&Light::GetShadowResolution, &Light::SetShadowResolution),
        "shadow_near_far_ratio", sol::property(&Light::GetShadowNearFarRatio, &Light::SetShadowNearFarRatio),
		sol::base_classes, sol::bases<Drawable, Component>());
    lua.new_usertype<StaticModel>("StaticModel",
        "id", sol::var(StringHash("StaticModel")),
		"model", sol::property(&StaticModel::GetModel, &StaticModel::SetModel),
		"material", sol::property([](StaticModel* self) { return self->GetMaterial(0); }, [](StaticModel* self, Material* mtl ) { self->SetMaterial(mtl); }),
        "SetModel", &StaticModel::SetModel,
        "SetMaterial", sol::overload(
			sol::resolve<void(Material*)>(&StaticModel::SetMaterial),
            sol::resolve<bool(unsigned, Material*)>(&StaticModel::SetMaterial)),
        "GetMaterial", sol::overload(
            sol::resolve<Material*(void) const>(&StaticModel::GetMaterial),
            sol::resolve<Material*(unsigned) const>(&StaticModel::GetMaterial)),
        "RaycastSingle", [](StaticModel* self, const Ray& ray) {
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
        },
        sol::base_classes, sol::bases<Drawable, Component>());
	lua.new_usertype<AnimationState>("AnimationState",
		"weight", sol::property(&AnimationState::GetWeight, &AnimationState::SetWeight),
		"looped", sol::property(&AnimationState::IsLooped, &AnimationState::SetLooped),
		"time", sol::property(&AnimationState::GetTime, &AnimationState::SetTime));
	lua.new_usertype<AnimatedModel>("AnimatedModel",
        "id", sol::var(StringHash("AnimatedModel")),
		"model", sol::property(&AnimatedModel::GetModel, [](AnimatedModel* self, Model* model) { self->SetModel(model); }),
		sol::base_classes, sol::bases<StaticModel, Drawable, Component>());
    lua.new_usertype<AnimationParameters>("AnimationParameters",
        sol::call_constructor, sol::factories(
            [](Animation* animation) { return AnimationParameters(animation); },
            [context](const ea::string& animationName) { return AnimationParameters(context, animationName); }),
        "weight", &AnimationParameters::weight_,
        "Looped", &AnimationParameters::Looped,
        "StartBone", &AnimationParameters::StartBone,
        "Layer", &AnimationParameters::Layer,
        "Time", &AnimationParameters::Time,
        "Additive", &AnimationParameters::Additive,
        "Weight", &AnimationParameters::Weight,
        "Speed", &AnimationParameters::Speed,
        "AutoFadeOut", &AnimationParameters::AutoFadeOut,
        "KeepOnCompletion", &AnimationParameters::KeepOnCompletion,
        "KeepOnZeroWeight", &AnimationParameters::KeepOnZeroWeight
        );
	lua.new_usertype<AnimationController>("AnimationController",
        "id", sol::var(StringHash("AnimationController")),
		"SetSpeed", &AnimationController::SetSpeed,
		"SetWeight", &AnimationController::SetWeight,
		"SetTime", &AnimationController::SetTime,
        "PlayNewExclusive", sol::overload(
            [](AnimationController* self, const AnimationParameters& params) { return self->PlayNewExclusive(params); },
            [](AnimationController* self, const AnimationParameters& params, float fadeInTime) { return self->PlayNewExclusive(params, fadeInTime); }),
		"PlayNew", sol::overload(
			[](AnimationController* self, const AnimationParameters& params) { return self->PlayNew(params); },
			[](AnimationController* self, const AnimationParameters& params, float fadeInTime) { return self->PlayNew(params, fadeInTime); }),
        "PlayExistingExclusive", sol::overload(
            [](AnimationController* self, const AnimationParameters& params) { return self->PlayExistingExclusive(params); },
            [](AnimationController* self, const AnimationParameters& params, float fadeInTime) { return self->PlayExistingExclusive(params, fadeInTime); }),
        "PlayExisting", sol::overload(
            [](AnimationController* self, const AnimationParameters& params) { return self->PlayExisting(params); },
            [](AnimationController* self, const AnimationParameters& params, float fadeInTime) { return self->PlayExisting(params, fadeInTime); }),
		"Stop", sol::overload(
			[](AnimationController* self, Animation* animation) { return self->Stop(animation); },
			[](AnimationController* self, Animation* animation, float fadeTime) { return self->Stop(animation, fadeTime); }),
		"IsPlaying", sol::overload(
			sol::resolve<bool(const ea::string&)const>(&AnimationController::IsPlaying),
			sol::resolve<bool(Animation*)const>(&AnimationController::IsPlaying)),
        "Fade", sol::overload(sol::resolve<Animation*, float, float >(&AnimationController::Fade)),
        "UpdateAnimationTime", &AnimationController::UpdateAnimationTime,
        "UpdateAnimationWeight", sol::overload(
            [](AnimationController* self, Animation* animation, float weight) { return self->UpdateAnimationWeight(animation, weight); },
            [](AnimationController* self, Animation* animation, float weight, float fadeTime) { return self->UpdateAnimationWeight(animation, weight, fadeTime); }),
        "UpdateAnimationSpeed", &AnimationController::UpdateAnimationSpeed,
        "GetAnimationParameters", sol::resolve<const AnimationParameters&(unsigned) const>(&AnimationController::GetAnimationParameters),
        "GetLastAnimationParameters", sol::overload(
            [](AnimationController* self, Animation* animation) { return self->GetLastAnimationParameters(animation); },
            [](AnimationController* self, Animation* animation, unsigned layer) { return self->GetLastAnimationParameters(animation, layer); }),
        sol::base_classes, sol::bases<AnimationStateSource, Component>());
	lua.new_usertype<Skybox>("Skybox",
        "id", sol::var(StringHash("Skybox")),
        sol::base_classes, sol::bases<StaticModel, Drawable, Component>());
    lua.new_usertype<DecalSet>("DecalSet",
        "id", sol::var(StringHash("DecalSet")),
        "material", sol::property(&DecalSet::GetMaterial, &DecalSet::SetMaterial),
        "AddDecal", sol::overload(
            [](DecalSet* self, Drawable* target, const Vector3& worldPosition, const Quaternion& worldRotation, float size, float aspectRatio, float depth, const Vector2& topLeftUV, const Vector2& bottomRightUV) {
                return self->AddDecal(target, worldPosition, worldRotation, size, aspectRatio, depth, topLeftUV, bottomRightUV);
            },
            [](DecalSet* self, Drawable* target, const Vector3& worldPosition, const Quaternion& worldRotation, float size, float aspectRatio, float depth, const Vector2& topLeftUV, const Vector2& bottomRightUV, float timeToLive) {
                return self->AddDecal(target, worldPosition, worldRotation, size, aspectRatio, depth, topLeftUV, bottomRightUV, timeToLive);
            },
            [](DecalSet* self, Drawable* target, const Vector3& worldPosition, const Quaternion& worldRotation, float size, float aspectRatio, float depth, const Vector2& topLeftUV, const Vector2& bottomRightUV, float timeToLive, float normalCutoff) {
                return self->AddDecal(target, worldPosition, worldRotation, size, aspectRatio, depth, topLeftUV, bottomRightUV, timeToLive, normalCutoff);
            }),
        "RemoveDecals", &DecalSet::RemoveDecals,
        "RemoveAllDecals", &DecalSet::RemoveAllDecals,
        sol::base_classes, sol::bases<Drawable, Component>());
    lua.new_usertype<Billboard>("Billboard",
        "position", &Billboard::position_,
        "size",     &Billboard::size_,
        "rotation", &Billboard::rotation_,
        "enabled",  &Billboard::enabled_);
    lua.new_usertype<BillboardSet>("BillboardSet",
        "id", sol::var(StringHash("BillboardSet")),
        "num_billboards", sol::property(&BillboardSet::GetNumBillboards, &BillboardSet::SetNumBillboards),
        "material", sol::property(&BillboardSet::GetMaterial, &BillboardSet::SetMaterial),
        "sorted", sol::property(&BillboardSet::IsSorted, &BillboardSet::SetSorted),
        "GetBillboard", &BillboardSet::GetBillboard,
        "Commit", &BillboardSet::Commit,
        "SetFaceCameraMode", &BillboardSet::SetFaceCameraMode,
        sol::base_classes, sol::bases<Drawable, Component>());
    lua.new_usertype<Terrain>("Terrain",
        "id", sol::var(StringHash("Terrain")),
        "patch_size", sol::property(&Terrain::GetPatchSize, &Terrain::SetPatchSize),
        "spacing", sol::property(&Terrain::GetSpacing, &Terrain::SetSpacing),
        "smoothing", sol::property(&Terrain::GetSmoothing, &Terrain::SetSmoothing),
        "height_map", sol::property(&Terrain::GetHeightMap, &Terrain::SetHeightMap),
        "height_map", sol::property(&Terrain::GetHeightMap, &Terrain::SetHeightMap),
        "material", sol::property(&Terrain::GetMaterial, &Terrain::SetMaterial),
        "occluder", sol::property(&Terrain::IsOccluder, &Terrain::SetOccluder),
        "GetHeight", &Terrain::GetHeight,
        "GetNormal", &Terrain::GetNormal,
        sol::base_classes, sol::bases<Component>()
        );
    lua.new_usertype<RenderPipeline>("RenderPipeline",
        "id", sol::var(StringHash("RenderPipeline")),
        sol::base_classes, sol::bases<Component>()
        );
//     lua.new_usertype<ProceduralSky>("ProceduralSky",
//         "id", sol::var(StringHash("ProceduralSky")),
//         "January", sol::var(ProceduralSky::January),
//         "February", sol::var(ProceduralSky::February),
//         "April", sol::var(ProceduralSky::April),
//         "May", sol::var(ProceduralSky::May),
//         "June", sol::var(ProceduralSky::June),
//         "July", sol::var(ProceduralSky::July),
//         "August", sol::var(ProceduralSky::August),
//         "September", sol::var(ProceduralSky::September),
//         "October", sol::var(ProceduralSky::October),
//         "November", sol::var(ProceduralSky::November),
//         "December", sol::var(ProceduralSky::December),
//         "Init", sol::overload(
//             [](ProceduralSky* self, uint32_t verticalCount, uint32_t horizontalCount, ProceduralSky::Month month, float time) { self->Init(verticalCount, horizontalCount, month, time); },
//             [](ProceduralSky* self, uint32_t verticalCount, uint32_t horizontalCount, ProceduralSky::Month month, float time, uint32_t cubemapSize, const Vector3& northDir) { self->Init(verticalCount, horizontalCount, month, time, cubemapSize, northDir); }),
//         "SetMonth", &ProceduralSky::SetMonth,
//         "SetTime", &ProceduralSky::SetTime,
//         "SetTimeScale", &ProceduralSky::SetTimeScale,
//         "GetSunDirection", &ProceduralSky::GetSunDirection,
//         "GetSunLuminance", &ProceduralSky::GetSunLuminance,
//         "GetSunLuminanceGamma", &ProceduralSky::GetSunLuminanceGamma,
//         "GetSkyLuminance", &ProceduralSky::GetSkyLuminance,
//         "GetSkyLuminanceGamma", &ProceduralSky::GetSkyLuminanceGamma,
//         sol::base_classes, sol::bases<StaticModel, Drawable, Component>()
//         );
    lua.new_usertype<OutlineGroup>("OutlineGroup",
        "id", sol::var(StringHash("OutlineGroup")),
        "SetColor", &OutlineGroup::SetColor,
        "GetOutlineMaterial", &OutlineGroup::GetOutlineMaterial,
        "HasDrawables", &OutlineGroup::HasDrawables,
        "ContainsDrawable", &OutlineGroup::ContainsDrawable,
        "ClearDrawables", &OutlineGroup::ClearDrawables,
        "AddDrawable", & OutlineGroup::AddDrawable,
        "RemoveDrawable", &OutlineGroup::RemoveDrawable,
        sol::base_classes, sol::bases<Component>());
    lua.new_usertype<Text3D>("Text3D",
        "id", sol::var(StringHash("Text3D")),
        "material", sol::property(&Text3D::GetMaterial, &Text3D::SetMaterial),
        "SetFont", [](Text3D* self, const ea::string& fontName) { self->SetFont(fontName); },
        "SetFontSize", &Text3D::SetFontSize,
        "SetText", &Text3D::SetText,
        "SetColor", sol::resolve<void(const Color&)>(&Text3D::SetColor),
        "SetOpacity", &Text3D::SetOpacity,
        "SetFaceCameraMode", &Text3D::SetFaceCameraMode,
        sol::base_classes, sol::bases<Drawable, Component>()
        );
	lua["graphics_system"] = context->GetSubsystem<Graphics>();
	lua["renderer_system"] = context->GetSubsystem<Renderer>();
	RegisterConst(lua);
	
	return 0;
}