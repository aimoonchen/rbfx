#include <nanobind/nanobind.h>
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
#include "../../RenderPipeline/ShaderConsts.h"
#include "../../RenderAPI/RenderDevice.h"
#include "../../Scene/Scene.h"
#include "../../Scene/Component.h"
#include "../../UI/Text3D.h"
//#define PAR_SHAPES_IMPLEMENTATION
#include "ThirdParty/par/par_shapes.h"

using namespace Urho3D;
namespace nb = nanobind;
using namespace nb::literals;

Urho3D::Model* ParMeshToModel(Urho3D::Context* ctx, par_shapes_mesh* mesh, bool swithYZ);

// Urho3D::Model* ParMeshToModel(Urho3D::Context* ctx, par_shapes_mesh* mesh, bool swithYZ)
// {
//     par_shapes_unweld(mesh, true);
//     par_shapes_compute_normals(mesh);
// 
//     if (swithYZ)
//     {
//         // ccw to cw
//         par_shapes_invert(mesh, 0, 0);
//     }
// 
//     Urho3D::BoundingBox bb;
//     float bbox[6];
//     par_shapes_compute_aabb(mesh, bbox);
//     bb.min_ = Urho3D::Vector3{bbox[0], bbox[2], bbox[1]};
//     bb.max_ = Urho3D::Vector3{bbox[3], bbox[5], bbox[4]};
// 
//     // Get index data
//     // 		std::vector<uint32_t> indexData;
//     // 		indexData.resize(mesh->ntriangles * 3);
//     // 		for (int i = 0; i < indexData.size(); ++i) {
//     // 			indexData[i] = mesh->triangles[i];
//     // 		}
//     struct VertexData
//     {
//         Urho3D::Vector3 position;
//         Urho3D::Vector3 normal;
//         // 		Urho3D::Vector2 texcoord;
//     };
// 
//     std::vector<VertexData> vertexData;
//     vertexData.resize(mesh->npoints);
//     if (swithYZ)
//     {
//         for (int i = 0; i < mesh->npoints; ++i)
//         {
//             const auto& pos = ((Urho3D::Vector3*)(mesh->points))[i];
//             vertexData[i].position = {pos.x_, pos.z_, pos.y_};
//             const auto& nor = ((Urho3D::Vector3*)(mesh->normals))[i];
//             vertexData[i].normal = {nor.x_, nor.z_, nor.y_};
//             // 		if (mesh->tcoords) {
//             // 			vertexData[i].texcoord = ((Urho3D::Vector2*)(mesh->tcoords))[i];
//             // 		}
//         }
//     }
//     else
//     {
//         for (int i = 0; i < mesh->npoints; ++i)
//         {
//             vertexData[i].position = ((Urho3D::Vector3*)(mesh->points))[i];
//             vertexData[i].normal = ((Urho3D::Vector3*)(mesh->normals))[i];
//             // 		if (mesh->tcoords) {
//             // 			vertexData[i].texcoord = ((Urho3D::Vector2*)(mesh->tcoords))[i];
//             // 		}
//         }
//     }
// 
//     // 	Urho3D::GenerateTangents(vertexData.data(), sizeof(VertexData), indexData.data(), sizeof(unsigned short), 0,
//     // indexData.size(), 		offsetof(VertexData, normal), offsetof(VertexData, texcoord), offsetof(VertexData, tangent));
// 
//     auto geometry = new Urho3D::Geometry(ctx);
//     geometry->SetNumVertexBuffers(1);
// 
//     auto vbo = new Urho3D::VertexBuffer(ctx);
//     vbo->SetShadowed(true);
//     vbo->SetSize((unsigned int)vertexData.size(),
//         {
//             Urho3D::VertexElement(Urho3D::TYPE_VECTOR3, Urho3D::SEM_POSITION),
//             Urho3D::VertexElement(Urho3D::TYPE_VECTOR3, Urho3D::SEM_NORMAL),
//             // 		Urho3D::VertexElement(Urho3D::TYPE_VECTOR2, Urho3D::SEM_TEXCOORD)
//         });
//     vbo->Update(vertexData.data());
// 
//     auto ibo = new Urho3D::IndexBuffer(ctx);
//     ibo->SetShadowed(true);
//     ibo->SetSize(mesh->ntriangles * 3, false);
//     ibo->Update(mesh->triangles);
// 
//     geometry->SetVertexBuffer(0, vbo);
//     geometry->SetIndexBuffer(ibo);
//     geometry->SetDrawRange(Urho3D::TRIANGLE_LIST, 0, ibo->GetIndexCount());
// 
//     par_shapes_free_mesh(mesh);
// 
//     auto model = new Urho3D::Model(ctx);
//     model->SetNumGeometries(1);
// 
//     // for save to file, remove this for runtime model?
//     model->SetVertexBuffers({SharedPtr<VertexBuffer>{vbo}}, {}, {});
//     model->SetIndexBuffers({SharedPtr<IndexBuffer>{ibo}});
//     //
//     model->SetGeometry(0, 0, geometry);
//     model->SetBoundingBox(bb);
//     return model;
// }

static void RegisterConst(nb::module_ m)
{
    m["LIGHT_DIRECTIONAL"]	= LIGHT_DIRECTIONAL;
    m["LIGHT_SPOT"]			= LIGHT_SPOT;
    m["LIGHT_POINT"]			= LIGHT_POINT;
    //
    m["DEFAULT_RANGE"]                = DEFAULT_RANGE;
    m["DEFAULT_LIGHT_FOV"]            = DEFAULT_LIGHT_FOV;
    m["DEFAULT_SPECULARINTENSITY"]    = DEFAULT_SPECULARINTENSITY;
    m["DEFAULT_BRIGHTNESS"]           = DEFAULT_BRIGHTNESS;
    m["DEFAULT_CONSTANTBIAS"]         = DEFAULT_CONSTANTBIAS;
    m["DEFAULT_SLOPESCALEDBIAS"]      = DEFAULT_SLOPESCALEDBIAS;
    m["DEFAULT_NORMALOFFSET"]         = DEFAULT_NORMALOFFSET;
    m["DEFAULT_BIASAUTOADJUST"]       = DEFAULT_BIASAUTOADJUST;
    m["DEFAULT_SHADOWFADESTART"]      = DEFAULT_SHADOWFADESTART;
    m["DEFAULT_SHADOWQUANTIZE"]       = DEFAULT_SHADOWQUANTIZE;
    m["DEFAULT_SHADOWMINVIEW"]        = DEFAULT_SHADOWMINVIEW;
    m["DEFAULT_SHADOWNEARFARRATIO"]   = DEFAULT_SHADOWNEARFARRATIO;
    m["DEFAULT_SHADOWMAXEXTRUSION"]   = DEFAULT_SHADOWMAXEXTRUSION;
    m["DEFAULT_SHADOWSPLIT"]          = DEFAULT_SHADOWSPLIT;
    m["DEFAULT_TEMPERATURE"]          = DEFAULT_TEMPERATURE;
    m["FC_NONE"]                      = FC_NONE;
    m["FC_ROTATE_XYZ"]                = FC_ROTATE_XYZ;
    m["FC_ROTATE_Y"]                  = FC_ROTATE_Y;
    m["FC_LOOKAT_XYZ"]                = FC_LOOKAT_XYZ;
    m["FC_LOOKAT_Y"]                  = FC_LOOKAT_Y;
    m["FC_LOOKAT_MIXED"]              = FC_LOOKAT_MIXED;
    m["FC_DIRECTION"]                 = FC_DIRECTION;
    m["FC_AXIS_ANGLE"]                = FC_AXIS_ANGLE;
    //
    auto GIType = m.def_submodule("GlobalIlluminationType");
    GIType["None"]              = GlobalIlluminationType::None;
    GIType["UseLightMap"]       = GlobalIlluminationType::UseLightMap;
    GIType["BlendLightProbes"]  = GlobalIlluminationType::BlendLightProbes;
    auto graphic = m.def_submodule("graphic");
    nb::enum_<TextureFormat>(m, "TextureFormat")
        .value("TEX_FORMAT_RGBA8_UNORM",       TextureFormat::TEX_FORMAT_RGBA8_UNORM)
        .value("TEX_FORMAT_RGBA8_UNORM_SRGB",  TextureFormat::TEX_FORMAT_RGBA8_UNORM_SRGB);
    nb::enum_<TextureFlag>(m, "TextureFlag")
        .value("None",                         TextureFlag::None)
        .value("BindRenderTarget",             TextureFlag::BindRenderTarget)
        .value("BindDepthStencil",             TextureFlag::BindDepthStencil)
        .value("BindUnorderedAccess",          TextureFlag::BindUnorderedAccess)
        .value("NoMultiSampledAutoResolve",    TextureFlag::NoMultiSampledAutoResolve);
    nb::enum_<BlendMode>(m, "BlendMode")
        .value("REPLACE",                      BLEND_REPLACE)
        .value("ADD",                          BLEND_ADD)
        .value("MULTIPLY",                     BLEND_MULTIPLY)
        .value("ALPHA",                        BLEND_ALPHA)
        .value("ADDALPHA",                     BLEND_ADDALPHA)
        .value("PREMULALPHA",                  BLEND_PREMULALPHA)
        .value("INVDESTALPHA",                 BLEND_INVDESTALPHA)
        .value("SUBTRACT",                     BLEND_SUBTRACT)
        .value("SUBTRACTALPHA",                BLEND_SUBTRACTALPHA)
        .value("DEFERRED_DECAL",               BLEND_DEFERRED_DECAL);
    auto shaderResources = graphic.def_submodule("ShaderResources");
    shaderResources["Albedo"]       = ShaderResources::Albedo.GetHash();
    shaderResources["Normal"]       = ShaderResources::Normal.GetHash();
    shaderResources["Properties"]   = ShaderResources::Properties.GetHash();
    shaderResources["Emission"]     = ShaderResources::Emission.GetHash();
    shaderResources["Reflection0"]  = ShaderResources::Reflection0.GetHash();
    shaderResources["Reflection1"]  = ShaderResources::Reflection1.GetHash();
    shaderResources["LightRamp"]    = ShaderResources::LightRamp.GetHash();
    shaderResources["LightShape"]   = ShaderResources::LightShape.GetHash();
    shaderResources["ShadowMap"]    = ShaderResources::ShadowMap.GetHash();
    shaderResources["DepthBuffer"]  = ShaderResources::DepthBuffer.GetHash();

    graphic["FILTER_BILINEAR"]      = FILTER_BILINEAR;
    //
    graphic["RAY_AABB"]             = RAY_AABB;
    graphic["RAY_OBB"]              = RAY_OBB;
    graphic["RAY_TRIANGLE"]         = RAY_TRIANGLE;
    graphic["RAY_TRIANGLE_UV"]      = RAY_TRIANGLE_UV;
    //
    graphic["DRAWABLE_UNDEFINED"]   = DRAWABLE_UNDEFINED;
    graphic["DRAWABLE_GEOMETRY"]    = DRAWABLE_GEOMETRY;
    graphic["DRAWABLE_LIGHT"]       = DRAWABLE_LIGHT;
    graphic["DRAWABLE_ZONE"]        = DRAWABLE_ZONE;
    graphic["DRAWABLE_GEOMETRY2D"]  = DRAWABLE_GEOMETRY2D;
    graphic["DRAWABLE_ANY"]         = DRAWABLE_ANY;
    //
    graphic["DEFAULT_VIEWMASK"]     = DEFAULT_VIEWMASK;
}

NB_MODULE(graphics, m)
{
    nb::class_<BiasParameters>(m, "BiasParameters")
        .def(nb::init<float, float>());

    nb::class_<CascadeParameters>(m, "CascadeParameters")
        .def(nb::init<float, float, float, float, float>());

    nb::class_<Material, Resource>(m, "Material")
        .def(nb::init<Context*>())
        //.def(nb::init<Context*, const ea::string&, const ea::string&, const ea::string&>())
//         sol::call_constructor,
//         sol::factories([context](const ea::string& vs, const ea::string& fs, const ea::string& passName) {
//             auto tech = new Technique(context);
//             auto pass = tech->CreatePass(passName);
//             pass->SetVertexShader(vs);
//             pass->SetPixelShader(fs);
//             auto mtl = new Material(context);
//             mtl->SetTechnique(0, tech);
//             return mtl;
//             }),
        .def_prop_rw("scene", &Material::GetScene, &Material::SetScene)
        .def_prop_rw("render_order", &Material::GetRenderOrder, &Material::SetRenderOrder)
        .def_prop_rw("depth_bias", &Material::GetDepthBias, &Material::SetDepthBias)
        .def_prop_rw("fill_mode", &Material::GetFillMode, &Material::SetFillMode)
        .def_prop_rw("cull_mode", &Material::GetCullMode, &Material::SetCullMode)
        .def_prop_rw("occlusion", &Material::GetOcclusion, &Material::SetOcclusion)
        .def_prop_rw("shadow_cull_mode", &Material::GetShadowCullMode, &Material::SetShadowCullMode)
        .def("Clone", [](Material* self) { return self->Clone().Detach(); })
        .def("Clone", [](Material* self, const ea::string& cloneName) { return self->Clone(cloneName).Detach(); })
        .def("SetTexture", [](Material* self, const char* name, Texture* texture) { self->SetTexture(name, texture); })
        .def("SetUVTransform", [](Material* self, const Vector2& offset, float rotation, const Vector2& repeat) { self->SetUVTransform(offset, rotation, repeat); })
        .def("SetUVTransform", [](Material* self, const Vector2& offset, float rotation, float repeat) { self->SetUVTransform(offset, rotation, repeat); })
        .def("SetShaderParameter", [](Material* self, const ea::string& name, const Variant& value) { self->SetShaderParameter(name, value); })
        .def("SetShaderParameter", [](Material* self, const ea::string& name, const Variant& value, bool isCustom) { self->SetShaderParameter(name, value, isCustom); })
        //.def("SetShaderParameterAnimation", [](Material* self, const ea::string& name, ValueAnimation* valueAnimation) { self->SetShaderParameterAnimation(name, valueAnimation); })
        //.def("SetShaderParameterAnimation", [](Material* self, const ea::string& name, ValueAnimation* valueAnimation, WrapMode wrapMode, float speed) { self->SetShaderParameterAnimation(name, valueAnimation, wrapMode, speed); })
        .def("SetShaderParameterAnimationWrapMode", &Material::SetShaderParameterAnimationWrapMode)
        .def("SetShaderParameterAnimationSpeed", &Material::SetShaderParameterAnimationSpeed);
        //.def("SetShaderParameterAnimationWrapMode", [](Material& self, const char* name, WrapMode wrapMode) { self.SetShaderParameterAnimationWrapMode(name, wrapMode); })
        //.def("SetShaderParameterAnimationSpeed", [](Material& self, const char* name, float speed) { self.SetShaderParameterAnimationSpeed(name, speed); });
        
    nb::class_<ResourceWithMetadata, Resource>(m, "ResourceWithMetadata").def(nb::init<Context*>());
//         sol::constructors<ResourceWithMetadata(Context*)>(),

    nb::class_<Animation, ResourceWithMetadata>(m, "Animation")
		//sol::base_classes, sol::bases<ResourceWithMetadata>());
        .def_prop_rw("length", &Animation::GetLength, &Animation::SetLength);

    nb::class_<Model, ResourceWithMetadata>(m, "Model")
        .def(nb::init<Context*>())
//         , sol::constructors<Model(Context*)>(),
        .def("GetNumGeometries", &Model::GetNumGeometries)
        .def("HideGeometry", [](Model* self, unsigned geomIndex) { self->GetGeometry(geomIndex, 0)->SetIndexBuffer(nullptr); })
        .def("CreateRock", [](int seed, int nsubdivisions) {
            auto par_mesh = par_shapes_create_rock(seed, nsubdivisions);
            return ParMeshToModel(Context::GetInstance(), par_mesh, false);
        });

    nb::class_<Texture, ResourceWithMetadata>(m, "Texture").def(nb::init<Context*>());
//     , sol::constructors<Texture(Context*)>(),

    nb::class_<Texture2D>(m, "Texture2D")
        .def(nb::init<Context*>())
//         , sol::call_constructor,
//         sol::factories([context]() {
//             // TODO:for water demo, the texture will be managed by engine
//             return new Texture2D(context);
//             // lua manage the object
//             //return std::make_unique<Texture2D>(context);
//             }),
//     );
        .def_prop_ro("width", &Texture2D::GetWidth)
	    .def_prop_ro("height", &Texture2D::GetHeight)
        .def_prop_rw("filter_mode", &Texture2D::GetFilterMode, &Texture2D::SetFilterMode)
        .def("GetRenderSurface", &Texture2D::GetRenderSurface)
        .def("SaveFile", [](Texture2D* self, const ea::string& filename) {
            static SharedPtr<Image> image;
            if (filename.empty()) {
                image = self->GetImage();
            } else {
                image->SaveFile({ "file", filename });
                image = nullptr;
            }
        })
        .def("SetSize", [](Texture2D* self, int width, int height, unsigned format) { self->SetSize(width, height, (TextureFormat)format); })
        .def("SetSize", [](Texture2D* self, int width, int height, unsigned format, TextureFlag usage) { self->SetSize(width, height, (TextureFormat)format, usage); })
        .def("SetSize", [](Texture2D* self, int width, int height, unsigned format, TextureFlag usage, int multiSample) { self->SetSize(width, height, (TextureFormat)format, usage, multiSample); });
		
    nb::class_<TextureCube, Texture>(m, "TextureCube");

    nb::class_<RenderSurface>(m, "RenderSurface")
        .def("SetViewport", &RenderSurface::SetViewport)
        .def("QueueUpdate", &RenderSurface::QueueUpdate)
        .def("GetWidth", &RenderSurface::GetWidth)
        .def("GetHeight", &RenderSurface::GetHeight);

    nb::class_<Octree, Component>(m, "Octree")
        //bindOctree["id"]                    = sol::var(StringHash("Octree"));
        .def("AddManualDrawable", &Octree::AddManualDrawable)
        .def("RemoveManualDrawable", &Octree::RemoveManualDrawable)
        .def("RaycastSingle", [](Octree* self, const Ray& ray, RayQueryLevel level, float maxDistance, int drawableFlags) {
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
        });
        
    nb::class_<Viewport>(m, "Viewport")
        .def(nb::init<Context*>())
        .def(nb::init<Context*, Scene*, Camera*>())
    //         sol::call_constructor, sol::factories(
    //             [context]() { return new Viewport(context); },
    //             [context](Scene* scene, Camera* camera) { return new Viewport(context, scene, camera); }));
        .def("SetScene", &Viewport::SetScene)
        .def("GetScene", &Viewport::GetScene)
        .def("SetCamera", &Viewport::SetCamera)
        .def("GetCamera", &Viewport::GetCamera);

    nb::class_<DebugRenderer, Component>(m, "DebugRenderer")
        //bindDebugRenderer["id"] = sol::var(StringHash("DebugRenderer"));
        .def("AddLine", [](DebugRenderer* self, const Vector3& start, const Vector3& end, const Color& color) { self->AddLine(start, end, color); })
        .def("AddLine", [](DebugRenderer* self, const Vector3& start, const Vector3& end, const Color& color, bool depthTest) {self->AddLine(start, end, color, depthTest); })
        .def("AddTriangle", [](DebugRenderer* self, const Vector3& v1, const Vector3& v2, const Vector3& v3, const Color& color) { self->AddTriangle(v1, v2, v3, color); })
        .def("AddTriangle", [](DebugRenderer* self, const Vector3& v1, const Vector3& v2, const Vector3& v3, const Color& color, bool depthTest) {self->AddTriangle(v1, v2, v3, color, depthTest); });
        
    nb::class_<MeshLine::LineDesc>(m, "MeshLineDesc")
        .def(nb::init<>())
        //, sol::call_constructor, sol::factories([]() { return MeshLine::LineDesc(); }));
        .def_rw("model_mat", &MeshLine::LineDesc::model_mat)
        .def_rw("color", &MeshLine::LineDesc::color)
        .def_rw("width", &MeshLine::LineDesc::width)
        .def_rw("world_space", &MeshLine::LineDesc::world_space)
        .def_rw("depth", &MeshLine::LineDesc::depth)
        .def_rw("depth_bias", &MeshLine::LineDesc::depth_bias)
        .def_rw("repeat", &MeshLine::LineDesc::repeat)
        .def_rw("visibility", &MeshLine::LineDesc::visibility)
        .def_rw("use_dash", &MeshLine::LineDesc::use_dash)
        .def_rw("dash_array", &MeshLine::LineDesc::dash_array)
        .def_rw("dash_offset", &MeshLine::LineDesc::dash_offset)
        .def_rw("dash_ratio", &MeshLine::LineDesc::dash_ratio)
        .def_rw("alpha_fade", &MeshLine::LineDesc::alpha_fade)
        .def_rw("texture", &MeshLine::LineDesc::tex)
        .def_rw("alpha_texture", &MeshLine::LineDesc::alphaTex)
        .def_rw("cache", &MeshLine::LineDesc::cache)
        .def_rw("visible", &MeshLine::LineDesc::visible);

    nb::class_<MeshLine, Component>(m, "MeshLine")
        //bindMeshLine["id"]                  = sol::var(StringHash("MeshLine"));
        .def("SetDepthBias", &MeshLine::SetDepthBias)
        .def("SetSlopeScaledDepthBias", &MeshLine::SetSlopeScaledDepthBias)
        .def("RemoveLine", &MeshLine::RemoveLine)
        .def("BeginLines", &MeshLine::BeginLines)
        .def("AppendLine", [](MeshLine* self, std::vector<Vector3> points) { self->AppendLine(std::move(points)); })
        .def("EndLines", &MeshLine::EndLines)
        .def("AddGrid", &MeshLine::AddGrid)
        .def("AddLine", [](MeshLine* self, const Vector3& start, const Vector3& end, const MeshLine::LineDesc& lineDesc) { return self->AppendLine(start, end, lineDesc); })
        .def("AddLine", [](MeshLine* self, std::vector<Vector3> points, const MeshLine::LineDesc& lineDesc) { return self->AppendLine(points, lineDesc); });
        
    nb::class_<Camera, Component>(m, "Camera")
        .def_prop_rw("zoom", &Camera::GetZoom, &Camera::SetZoom)
        //bindCamera["id"]                    = sol::var(StringHash("Camera"));
        .def_prop_rw("far_clip", &Camera::GetFarClip, &Camera::SetFarClip)
        .def_prop_rw("near_clip", &Camera::GetNearClip, &Camera::SetNearClip)
        .def_prop_rw("fov", &Camera::GetFov, &Camera::SetFov)
        .def_prop_rw("view_mask", &Camera::GetViewMask, &Camera::SetViewMask)
        .def_prop_rw("auto_aspect_ratio", &Camera::GetAutoAspectRatio, &Camera::SetAutoAspectRatio)
        .def_prop_rw("use_reflection", &Camera::GetUseReflection, &Camera::SetUseReflection)
        .def_prop_rw("reflection_plane", &Camera::GetReflectionPlane, &Camera::SetReflectionPlane)
        .def_prop_rw("use_clipping", &Camera::GetUseClipping, &Camera::SetUseClipping)
        .def_prop_rw("clip_plane", &Camera::GetClipPlane, &Camera::SetClipPlane)
        .def_prop_rw("aspect_ratio", &Camera::GetAspectRatio, &Camera::SetAspectRatio)
        //bindCamera["zoom"]                  = sol::property(&Camera::GetZoom, &Camera::SetZoom);
        .def("WorldToScreenPoint", &Camera::WorldToScreenPoint)
        .def("ScreenToWorldPoint", &Camera::ScreenToWorldPoint)
        .def("GetScreenRay", &Camera::GetScreenRay)
        .def("SetOrthographic", &Camera::SetOrthographic)
        .def("SetOrthoSize", [](Camera* self, float orthoSize) { self->SetOrthoSize(orthoSize); })
        .def("SetOrthoSize", [](Camera* self, const Vector2& orthoSize) { self->SetOrthoSize(orthoSize); });
		
    nb::class_<Graphics>(m, "Graphics")
        .def("SetWindowIcon", &Graphics::SetWindowIcon)
        .def_prop_rw("windowTitle", &Graphics::GetWindowTitle, &Graphics::SetWindowTitle)
        .def_prop_ro("width", &Graphics::GetWidth)
        .def_prop_ro("height", &Graphics::GetHeight)
        .def("GetStats", [](Graphics* self) {
            const auto& stats = self->GetSubsystem<RenderDevice>()->GetStats();
            static ea::string ret;
            ret.clear();
            ret.append_sprintf("DP: %d, Triangles(Lines): %d", stats.numDraws_, stats.numPrimitives_);
            return ret; });// ea::string{ {}, "DP: %d, Triangles(Lines): %d", stats.numDraws_, stats.numPrimitives_ };

    nb::class_<Renderer>(m, "Renderer")
        .def(nb::init<Context*>())
        // , sol::constructors<Renderer(Context*)>());
        .def("SetViewport", &Renderer::SetViewport)
        .def("DrawDebugGeometry", &Renderer::DrawDebugGeometry);

    nb::class_<Drawable, Component>(m, "Drawable")
        .def_prop_rw("cast_shadows", &Drawable::GetCastShadows, &Drawable::SetCastShadows)
        .def_prop_rw("view_mask", &Drawable::GetViewMask, &Drawable::SetViewMask)
        .def_prop_rw("light_mask", &Drawable::GetLightMask, &Drawable::SetLightMask)
        .def_prop_rw("shadow_mask", &Drawable::GetShadowMask, &Drawable::SetShadowMask)
        //bindDrawable["OnSetEnabled"]                = &Drawable::OnSetEnabled;
        .def("IsInOctree", &Drawable::IsInOctree)
        .def("SetOccluder", &Drawable::SetOccluder)
        .def("GetBoundingBox", &Drawable::GetBoundingBox)
        .def("SetCastShadows", &Drawable::SetCastShadows)
        .def("GetWorldBoundingBox", &Drawable::GetWorldBoundingBox)
        .def("SetGlobalIlluminationType", &Drawable::SetGlobalIlluminationType);
		
    nb::class_<Zone, Drawable>(m, "Zone")
        //bindZone["id"]                      = sol::var(StringHash("Zone"));
        .def_prop_rw("bounding_box", [](Zone* self) { return self->GetBoundingBox(); }, &Zone::SetBoundingBox)
        .def_prop_rw("ambient_color", &Zone::GetAmbientColor, &Zone::SetAmbientColor)
        .def_prop_rw("ambient_brightness", &Zone::GetAmbientBrightness, &Zone::SetAmbientBrightness)
        .def_prop_rw("background_brightness", &Zone::GetBackgroundBrightness, &Zone::SetBackgroundBrightness)
        .def_prop_rw("fog_color", &Zone::GetFogColor, &Zone::SetFogColor)
        .def_prop_rw("fog_start", &Zone::GetFogStart, &Zone::SetFogStart)
        .def_prop_rw("fog_end", &Zone::GetFogEnd, &Zone::SetFogEnd)
        .def("SetProceduralSky", &Zone::SetProceduralSky)
        .def("SetZoneTextureAttr", [](Zone* self, const ea::string& filename) { filename.empty() ? self->SetZoneTexture(nullptr) : self->SetZoneTextureAttr(ResourceRef(StringHash("TextureCube"), filename)); });
		
    nb::class_<Light, Drawable>(m, "Light")
        //bindLight["id"]                     = sol::var(StringHash("Light"));
        .def_prop_rw("light_type", &Light::GetLightType, &Light::SetLightType)
        .def_prop_rw("range", &Light::GetRange, &Light::SetRange)
        .def_prop_rw("color", &Light::GetColor, &Light::SetColor)
        .def_prop_rw("brightness", &Light::GetBrightness, &Light::SetBrightness)
        .def_prop_rw("indirect_brightness", &Light::GetIndirectBrightness, &Light::SetIndirectBrightness)
        .def_prop_rw("fov", &Light::GetFov, &Light::SetFov)
        .def_prop_rw("ramp_texture", &Light::GetRampTexture, &Light::SetRampTexture)
        .def_prop_rw("shadow_bias", &Light::GetShadowBias, &Light::SetShadowBias)
        .def_prop_rw("shadow_cascade", &Light::GetShadowCascade, &Light::SetShadowCascade)
        .def_prop_rw("specular_intensity", &Light::GetSpecularIntensity, &Light::SetSpecularIntensity)
        .def_prop_rw("shadow_distance", &Light::GetShadowDistance, &Light::SetShadowDistance)
        .def_prop_rw("shadow_fade_distance", &Light::GetShadowFadeDistance, &Light::SetShadowFadeDistance)
        .def_prop_rw("shadow_resolution", &Light::GetShadowResolution, &Light::SetShadowResolution)
        .def_prop_rw("shadow_near_far_ratio", &Light::GetShadowNearFarRatio, &Light::SetShadowNearFarRatio);
		
    nb::class_<StaticModel, Drawable>(m, "StaticModel")
    //bindStaticModel["id"]           = sol::var(StringHash("StaticModel"));
        .def_prop_rw("model", &StaticModel::GetModel, &StaticModel::SetModel)
        .def_prop_rw("material", [](StaticModel* self) { return self->GetMaterial(0); }, [](StaticModel* self, Material* mtl) { self->SetMaterial(mtl); })
        .def("SetModel", &StaticModel::SetModel)
        .def("SetMaterial", nb::overload_cast<Material*>(&StaticModel::SetMaterial))
        .def("SetMaterial", nb::overload_cast<unsigned, Material*>(&StaticModel::SetMaterial))
        .def("GetMaterial", nb::overload_cast<>(&StaticModel::GetMaterial, nb::const_))
        .def("GetMaterial", nb::overload_cast<unsigned>(&StaticModel::GetMaterial, nb::const_))
        .def("RaycastSingle", [](StaticModel* self, const Ray& ray) {
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
        });
        
    nb::class_<AnimationState>(m, "AnimationState")
        .def_prop_rw("weight", &AnimationState::GetWeight, &AnimationState::SetWeight)
        .def_prop_rw("looped", &AnimationState::IsLooped, &AnimationState::SetLooped)
        .def_prop_rw("time", &AnimationState::GetTime, &AnimationState::SetTime);

    nb::class_<AnimatedModel, StaticModel>(m, "AnimatedModel")
        //bindAnimatedModel["id"] = sol::var(StringHash("AnimatedModel"));
        .def_prop_rw("model", &AnimatedModel::GetModel, [](AnimatedModel* self, Model* model) { self->SetModel(model); });

    nb::class_<AnimationParameters>(m, "AnimationParameters")
        .def(nb::init<Animation*>())
        .def(nb::init<Context*, const ea::string&>())
//         , sol::call_constructor, sol::factories(
//             [](Animation* animation) { return AnimationParameters(animation); },
//             [context](const ea::string& animationName) { return AnimationParameters(context, animationName); }));
        .def_rw("weight", &AnimationParameters::weight_)
        .def("Looped", &AnimationParameters::Looped)
        .def("StartBone", [](AnimationParameters* self, std::string_view startBone) { return self->StartBone(startBone.data()); })
        .def("Layer", &AnimationParameters::Layer)
        .def("Time", &AnimationParameters::Time)
        .def("Additive", &AnimationParameters::Additive)
        .def("Weight", &AnimationParameters::Weight)
        .def("Speed", &AnimationParameters::Speed)
        .def("AutoFadeOut", &AnimationParameters::AutoFadeOut)
        .def("KeepOnCompletion", &AnimationParameters::KeepOnCompletion)
        .def("KeepOnZeroWeight", &AnimationParameters::KeepOnZeroWeight);

    nb::class_<AnimationController, AnimationStateSource>(m, "AnimationController")
        //bindAnimationController["id"]                   = sol::var(StringHash("AnimationController"));
        .def("SetSpeed", &AnimationController::SetSpeed)
        .def("SetWeight", &AnimationController::SetWeight)
        .def("SetTime", &AnimationController::SetTime)
        .def("PlayNewExclusive", [](AnimationController* self, const AnimationParameters& params) { return self->PlayNewExclusive(params); })
        .def("PlayNewExclusive", [](AnimationController* self, const AnimationParameters& params, float fadeInTime) { return self->PlayNewExclusive(params, fadeInTime); })
        .def("PlayNew", [](AnimationController* self, const AnimationParameters& params) { return self->PlayNew(params); })
        .def("PlayNew", [](AnimationController* self, const AnimationParameters& params, float fadeInTime) { return self->PlayNew(params, fadeInTime); })
        .def("PlayExistingExclusive", [](AnimationController* self, const AnimationParameters& params) { return self->PlayExistingExclusive(params); })
        .def("PlayExistingExclusive", [](AnimationController* self, const AnimationParameters& params, float fadeInTime) { return self->PlayExistingExclusive(params, fadeInTime); })
        .def("PlayExisting", [](AnimationController* self, const AnimationParameters& params) { return self->PlayExisting(params); })
        .def("PlayExisting", [](AnimationController* self, const AnimationParameters& params, float fadeInTime) { return self->PlayExisting(params, fadeInTime); })
        .def("Stop", [](AnimationController* self, Animation* animation) { return self->Stop(animation); })
        .def("Stop", [](AnimationController* self, Animation* animation, float fadeTime) { return self->Stop(animation, fadeTime); })
        .def("IsPlaying", nb::overload_cast<const ea::string&>(&AnimationController::IsPlaying, nb::const_))
        .def("IsPlaying", nb::overload_cast<Animation*>(&AnimationController::IsPlaying, nb::const_))
        .def("Fade", nb::overload_cast<Animation*, float, float >(&AnimationController::Fade))
        .def("UpdateAnimationTime", &AnimationController::UpdateAnimationTime)
        .def("UpdateAnimationWeight", [](AnimationController* self, Animation* animation, float weight) { return self->UpdateAnimationWeight(animation, weight); })
        .def("UpdateAnimationWeight", [](AnimationController* self, Animation* animation, float weight, float fadeTime) { return self->UpdateAnimationWeight(animation, weight, fadeTime); })
        .def("UpdateAnimationSpeed", &AnimationController::UpdateAnimationSpeed)
        .def("GetAnimationParameters", nb::overload_cast<unsigned>(&AnimationController::GetAnimationParameters, nb::const_))
        .def("GetLastAnimationParameters", [](AnimationController* self, Animation* animation) { return self->GetLastAnimationParameters(animation); })
        .def("GetLastAnimationParameters", [](AnimationController* self, Animation* animation, unsigned layer) { return self->GetLastAnimationParameters(animation, layer); });
        
    nb::class_<Skybox, StaticModel>(m, "Skybox");
        //"id", sol::var(StringHash("Skybox")),

    nb::class_<DecalSet, Drawable>(m, "DecalSet")
        //bindDecalSet["id"] = sol::var(StringHash("DecalSet"));
        .def_prop_rw("material", &DecalSet::GetMaterial, &DecalSet::SetMaterial)
        .def("AddDecal", [](DecalSet* self, Drawable* target, const Vector3& worldPosition, const Quaternion& worldRotation, float size, float aspectRatio, float depth, const Vector2& topLeftUV, const Vector2& bottomRightUV) {
                return self->AddDecal(target, worldPosition, worldRotation, size, aspectRatio, depth, topLeftUV, bottomRightUV); })
        .def("AddDecal", [](DecalSet* self, Drawable* target, const Vector3& worldPosition, const Quaternion& worldRotation, float size, float aspectRatio, float depth, const Vector2& topLeftUV, const Vector2& bottomRightUV, float timeToLive) {
                return self->AddDecal(target, worldPosition, worldRotation, size, aspectRatio, depth, topLeftUV, bottomRightUV, timeToLive); })
        .def("AddDecal", [](DecalSet* self, Drawable* target, const Vector3& worldPosition, const Quaternion& worldRotation, float size, float aspectRatio, float depth, const Vector2& topLeftUV, const Vector2& bottomRightUV, float timeToLive, float normalCutoff) {
                return self->AddDecal(target, worldPosition, worldRotation, size, aspectRatio, depth, topLeftUV, bottomRightUV, timeToLive, normalCutoff); })
        .def("RemoveDecals", &DecalSet::RemoveDecals)
        .def("RemoveAllDecals", &DecalSet::RemoveAllDecals);
        
    nb::class_<Billboard>(m, "Billboard")
        .def_rw("position", &Billboard::position_)
        .def_rw("size", &Billboard::size_)
        .def_rw("rotation", &Billboard::rotation_)
        .def_rw("enabled", &Billboard::enabled_);

    nb::class_<BillboardSet, Drawable>(m, "BillboardSet")
        //bindBillboardSet["id"]                  = sol::var(StringHash("BillboardSet"));
        .def_prop_rw("num_billboards", &BillboardSet::GetNumBillboards, &BillboardSet::SetNumBillboards)
        .def_prop_rw("material", &BillboardSet::GetMaterial, &BillboardSet::SetMaterial)
        .def_prop_rw("sorted", &BillboardSet::IsSorted, &BillboardSet::SetSorted)
        .def("GetBillboard", &BillboardSet::GetBillboard)
        .def("Commit", &BillboardSet::Commit)
        .def("SetFaceCameraMode", &BillboardSet::SetFaceCameraMode);
        
    nb::class_<Terrain, Component>(m, "Terrain")
        //bindTerrain["id"]           = sol::var(StringHash("Terrain"));
        .def_prop_rw("patch_size", &Terrain::GetPatchSize, &Terrain::SetPatchSize)
        .def_prop_rw("spacing", &Terrain::GetSpacing, &Terrain::SetSpacing)
        .def_prop_rw("smoothing", &Terrain::GetSmoothing, &Terrain::SetSmoothing)
        .def_prop_rw("height_map", &Terrain::GetHeightMap, &Terrain::SetHeightMap)
        .def_prop_rw("height_map", &Terrain::GetHeightMap, &Terrain::SetHeightMap)
        .def_prop_rw("material", &Terrain::GetMaterial, &Terrain::SetMaterial)
        .def_prop_rw("occluder", &Terrain::IsOccluder, &Terrain::SetOccluder)
        .def("GetHeight", &Terrain::GetHeight)
        .def("GetNormal", &Terrain::GetNormal);

    nb::class_<RenderPipeline, Component>(m, "RenderPipeline")
        //"id", sol::var(StringHash("RenderPipeline")),
        .def("SetRenderPassEnabled", &RenderPipeline::SetRenderPassEnabled);

    nb::class_<ProceduralSky, StaticModel>(m, "ProceduralSky")
    //bindProceduralSky["id"]                     = sol::var(StringHash("ProceduralSky"));
    .def("Init", [](ProceduralSky* self, uint32_t verticalCount, uint32_t horizontalCount, ProceduralSky::Month month, float time) { self->Init(verticalCount, horizontalCount, month, time); })
    .def("Init", [](ProceduralSky* self, uint32_t verticalCount, uint32_t horizontalCount, ProceduralSky::Month month, float time, uint32_t cubemapSize, const Vector3& northDir) { self->Init(verticalCount, horizontalCount, month, time, cubemapSize, northDir); })
//     .attr("January", ProceduralSky::January)
//     .attr("February", ProceduralSky::February)
//     .attr("April", ProceduralSky::April)
//     .attr("May", ProceduralSky::May)
//     .attr("June", ProceduralSky::June)
//     .attr("July", ProceduralSky::July)
//     .attr("August", ProceduralSky::August)
//     .attr("September", ProceduralSky::September)
//     .attr("October", ProceduralSky::October)
//     .attr("November", ProceduralSky::November)
//     .attr("December", ProceduralSky::December)
    .def("SetMonth", &ProceduralSky::SetMonth)
    .def("SetTime", &ProceduralSky::SetTime)
    .def("SetTimeScale", &ProceduralSky::SetTimeScale)
    .def("GetSunDirection", &ProceduralSky::GetSunDirection)
    .def("GetSunLuminance", &ProceduralSky::GetSunLuminance)
    .def("GetSunLuminanceGamma", &ProceduralSky::GetSunLuminanceGamma)
    .def("GetSkyLuminance", &ProceduralSky::GetSkyLuminance)
    .def("GetSkyLuminanceGamma", &ProceduralSky::GetSkyLuminanceGamma);

    nb::class_<OutlineGroup, Component>(m, "OutlineGroup")
    //bindOutlineGroup["id"]                  = sol::var(StringHash("OutlineGroup"));
    .def("SetColor", &OutlineGroup::SetColor)
    .def("GetOutlineMaterial", &OutlineGroup::GetOutlineMaterial)
    .def("HasDrawables", &OutlineGroup::HasDrawables)
    .def("ContainsDrawable", &OutlineGroup::ContainsDrawable)
    .def("ClearDrawables", &OutlineGroup::ClearDrawables)
    .def("AddDrawable", &OutlineGroup::AddDrawable)
    .def("RemoveDrawable", &OutlineGroup::RemoveDrawable);
        
    nb::class_<Text3D, Drawable>(m, "Text3D")
    //bindText3D["id"]                = sol::var(StringHash("Text3D"));
    .def_prop_rw("material", &Text3D::GetMaterial, &Text3D::SetMaterial)
    .def("SetFont", [](Text3D* self, const ea::string& fontName) { self->SetFont(fontName); })
    .def("SetFontSize", &Text3D::SetFontSize)
    .def("SetText", &Text3D::SetText)
    .def("SetColor", nb::overload_cast<const Color&>(&Text3D::SetColor))
    .def("SetOpacity", &Text3D::SetOpacity)
    .def("SetFaceCameraMode", &Text3D::SetFaceCameraMode);
        
    m.attr("graphics_system") = Context::GetInstance()->GetSubsystem<Graphics>();
    m.attr("renderer_system") = Context::GetInstance()->GetSubsystem<Renderer>();
    RegisterConst(m);
}
