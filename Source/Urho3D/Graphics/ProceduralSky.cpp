// #include "ProceduralSky.h"
// #include <map>
// #include "../Math/Vector3.h"
// #include "../Math/Quaternion.h"
// #include "../Graphics/Geometry.h"
// #include "../Graphics/Graphics.h"
// #include "../Graphics/Model.h"
// #include "../Graphics/VertexBuffer.h"
// #include "../Graphics/IndexBuffer.h"
// #include "../Graphics/StaticModel.h"
// #include "../Graphics/ShaderProgram.h"
// #include "../Graphics/Technique.h"
// #include "../Graphics/Material.h"
// #include "../Graphics/Octree.h"
// #include "../Graphics/ReflectionProbe.h"
// #include "../Graphics/TextureCube.h"
// #include "../Graphics/Texture2D.h"
// #include "../Graphics/Camera.h"
// #include "../Graphics/Viewport.h"
// #include "../Graphics/Zone.h"
// #include "../Graphics/GraphicsEvents.h"
// #include "../Scene/Scene.h"
// #include "../Resource/ImageCube.h"
// #include "../Resource/ResourceCache.h"
// #include "../Graphics/Material.h"
// #include "../Graphics/Skybox.h"
// 
// #include "bx/math.h"
// 
// // from bgfx sample
// namespace Urho3D
// {
// // Represents color. Color-space depends on context.
// // In the code below, used to represent color in XYZ, and RGB color-space
// typedef bx::Vec3 bxColor;
// 
// // HDTV rec. 709 matrix.
// static constexpr float M_XYZ2RGB[] = {
//     3.240479f,
//     -0.969256f,
//     0.055648f,
//     -1.53715f,
//     1.875991f,
//     -0.204043f,
//     -0.49853f,
//     0.041556f,
//     1.057311f,
// };
// 
// // Converts color representation from CIE XYZ to RGB color-space.
// bxColor xyzToRgb(const bxColor& xyz)
// {
//     bxColor rgb(bx::InitNone);
//     rgb.x = M_XYZ2RGB[0] * xyz.x + M_XYZ2RGB[3] * xyz.y + M_XYZ2RGB[6] * xyz.z;
//     rgb.y = M_XYZ2RGB[1] * xyz.x + M_XYZ2RGB[4] * xyz.y + M_XYZ2RGB[7] * xyz.z;
//     rgb.z = M_XYZ2RGB[2] * xyz.x + M_XYZ2RGB[5] * xyz.y + M_XYZ2RGB[8] * xyz.z;
//     return rgb;
// };
// 
// // Precomputed luminance of sunlight in XYZ colorspace.
// // Computed using code from Game Engine Gems, Volume One, chapter 15. Implementation based on Dr. Richard Bird model.
// // This table is used for piecewise linear interpolation. Transitions from and to 0.0 at sunset and sunrise are highly
// // inaccurate
// static std::map<float, bxColor> sunLuminanceXYZTable = {
//     {5.0f, {0.000000f, 0.000000f, 0.000000f}},
//     {7.0f, {12.703322f, 12.989393f, 9.100411f}},
//     {8.0f, {13.202644f, 13.597814f, 11.524929f}},
//     {9.0f, {13.192974f, 13.597458f, 12.264488f}},
//     {10.0f, {13.132943f, 13.535914f, 12.560032f}},
//     {11.0f, {13.088722f, 13.489535f, 12.692996f}},
//     {12.0f, {13.067827f, 13.467483f, 12.745179f}},
//     {13.0f, {13.069653f, 13.469413f, 12.740822f}},
//     {14.0f, {13.094319f, 13.495428f, 12.678066f}},
//     {15.0f, {13.142133f, 13.545483f, 12.526785f}},
//     {16.0f, {13.201734f, 13.606017f, 12.188001f}},
//     {17.0f, {13.182774f, 13.572725f, 11.311157f}},
//     {18.0f, {12.448635f, 12.672520f, 8.267771f}},
//     {20.0f, {0.000000f, 0.000000f, 0.000000f}},
// };
// 
// // Precomputed luminance of sky in the zenith point in XYZ colorspace.
// // Computed using code from Game Engine Gems, Volume One, chapter 15. Implementation based on Dr. Richard Bird model.
// // This table is used for piecewise linear interpolation. Day/night transitions are highly inaccurate.
// // The scale of luminance change in Day/night transitions is not preserved.
// // Luminance at night was increased to eliminate need the of HDR render.
// static std::map<float, bxColor> skyLuminanceXYZTable = {
//     {0.0f, {0.308f, 0.308f, 0.411f}},
//     {1.0f, {0.308f, 0.308f, 0.410f}},
//     {2.0f, {0.301f, 0.301f, 0.402f}},
//     {3.0f, {0.287f, 0.287f, 0.382f}},
//     {4.0f, {0.258f, 0.258f, 0.344f}},
//     {5.0f, {0.258f, 0.258f, 0.344f}},
//     {7.0f, {0.962851f, 1.000000f, 1.747835f}},
//     {8.0f, {0.967787f, 1.000000f, 1.776762f}},
//     {9.0f, {0.970173f, 1.000000f, 1.788413f}},
//     {10.0f, {0.971431f, 1.000000f, 1.794102f}},
//     {11.0f, {0.972099f, 1.000000f, 1.797096f}},
//     {12.0f, {0.972385f, 1.000000f, 1.798389f}},
//     {13.0f, {0.972361f, 1.000000f, 1.798278f}},
//     {14.0f, {0.972020f, 1.000000f, 1.796740f}},
//     {15.0f, {0.971275f, 1.000000f, 1.793407f}},
//     {16.0f, {0.969885f, 1.000000f, 1.787078f}},
//     {17.0f, {0.967216f, 1.000000f, 1.773758f}},
//     {18.0f, {0.961668f, 1.000000f, 1.739891f}},
//     {20.0f, {0.264f, 0.264f, 0.352f}},
//     {21.0f, {0.264f, 0.264f, 0.352f}},
//     {22.0f, {0.290f, 0.290f, 0.386f}},
//     {23.0f, {0.303f, 0.303f, 0.404f}},
// };
// 
// // Turbidity tables. Taken from:
// // A. J. Preetham, P. Shirley, and B. Smits. A Practical Analytic Model for Daylight. SIGGRAPH '99
// // Coefficients correspond to xyY colorspace.
// static constexpr bxColor ABCDE[] = {
//     {-0.2592f, -0.2608f, -1.4630f},
//     {0.0008f, 0.0092f, 0.4275f},
//     {0.2125f, 0.2102f, 5.3251f},
//     {-0.8989f, -1.6537f, -2.5771f},
//     {0.0452f, 0.0529f, 0.3703f},
// };
// 
// static constexpr bxColor ABCDE_t[] = {
//     {-0.0193f, -0.0167f, 0.1787f},
//     {-0.0665f, -0.0950f, -0.3554f},
//     {-0.0004f, -0.0079f, -0.0227f},
//     {-0.0641f, -0.0441f, 0.1206f},
//     {-0.0033f, -0.0109f, -0.0670f},
// };
// 
// // Performs piecewise linear interpolation of a Color parameter.
// class DynamicValueController
// {
//     typedef bxColor ValueType;
//     typedef std::map<float, ValueType> KeyMap;
// 
// public:
//     DynamicValueController() {}
// 
//     ~DynamicValueController() {}
// 
//     void SetMap(const KeyMap& keymap) { m_keyMap = keymap; }
// 
//     ValueType GetValue(float time) const
//     {
//         typename KeyMap::const_iterator itUpper = m_keyMap.upper_bound(time + 1e-6f);
//         typename KeyMap::const_iterator itLower = itUpper;
//         --itLower;
// 
//         if (itLower == m_keyMap.end())
//         {
//             return itUpper->second;
//         }
// 
//         if (itUpper == m_keyMap.end())
//         {
//             return itLower->second;
//         }
// 
//         float lowerTime = itLower->first;
//         const ValueType& lowerVal = itLower->second;
//         float upperTime = itUpper->first;
//         const ValueType& upperVal = itUpper->second;
// 
//         if (lowerTime == upperTime)
//         {
//             return lowerVal;
//         }
// 
//         return interpolate(lowerTime, lowerVal, upperTime, upperVal, time);
//     };
// 
//     void Clear() { m_keyMap.clear(); };
// 
// private:
//     ValueType interpolate(
//         float lowerTime, const ValueType& lowerVal, float upperTime, const ValueType& upperVal, float time) const
//     {
//         const float tt = (time - lowerTime) / (upperTime - lowerTime);
//         const ValueType result = bx::lerp(lowerVal, upperVal, tt);
//         return result;
//     };
// 
//     KeyMap m_keyMap;
// };
// 
// // Controls sun position according to time, month, and observer's latitude.
// // Sun position computation based on Earth's orbital elements:
// // https://nssdc.gsfc.nasa.gov/planetary/factsheet/earthfact.html
// class SunController
// {
// public:
//     enum Month : int
//     {
//         January = 0,
//         February,
//         March,
//         April,
//         May,
//         June,
//         July,
//         August,
//         September,
//         October,
//         November,
//         December
//     };
// 
//     SunController()
//         : m_northDir(1.0f, 0.0f, 0.0f)
//         , m_sunDir(0.0f, -1.0f, 0.0f)
//         , m_upDir(0.0f, 1.0f, 0.0f)
//         , m_latitude(50.0f)
//         , m_month(June)
//         , m_eclipticObliquity(bx::toRad(23.4f))
//         , m_delta(0.0f)
//     {
//     }
// 
//     void Update(float _time)
//     {
//         CalculateSunOrbit();
//         UpdateSunPosition(_time - 12.0f);
//     }
// 
//     bx::Vec3 m_northDir;
//     bx::Vec3 m_sunDir;
//     bx::Vec3 m_upDir;
//     float m_latitude;
//     Month m_month;
// 
// private:
//     void CalculateSunOrbit()
//     {
//         const float day = 30.0f * float(m_month) + 15.0f;
//         float lambda = 280.46f + 0.9856474f * day;
//         lambda = bx::toRad(lambda);
//         m_delta = bx::asin(bx::sin(m_eclipticObliquity) * bx::sin(lambda));
//     }
// 
//     void UpdateSunPosition(float _hour)
//     {
//         const float latitude = bx::toRad(m_latitude);
//         const float hh = _hour * bx::kPi / 12.0f;
//         const float azimuth =
//             bx::atan2(bx::sin(hh), bx::cos(hh) * bx::sin(latitude) - bx::tan(m_delta) * bx::cos(latitude));
// 
//         const float altitude =
//             bx::asin(bx::sin(latitude) * bx::sin(m_delta) + bx::cos(latitude) * bx::cos(m_delta) * bx::cos(hh));
// 
//         const bx::Quaternion rot0 = bx::fromAxisAngle(m_upDir, -azimuth);
//         const bx::Vec3 dir = bx::mul(m_northDir, rot0);
//         const bx::Vec3 uxd = bx::cross(m_upDir, dir);
// 
//         const bx::Quaternion rot1 = bx::fromAxisAngle(uxd, altitude);
//         m_sunDir = bx::mul(dir, rot1);
//     }
// 
//     float m_eclipticObliquity;
//     float m_delta;
// };
// 
// struct ScreenPosVertex
// {
//     float m_x;
//     float m_y;
// };
// 
// ProceduralSky::ProceduralSky(Context* context)
//     : StaticModel(context)
// {
//     SetReflectionMode(ReflectionMode::Zone);
// 
//     SubscribeToEvent(E_BEGINRENDERING, URHO3D_HANDLER(ProceduralSky, HandleBeginRendering));
// }
// 
// ProceduralSky::~ProceduralSky() = default;
// 
// void ProceduralSky::RegisterObject(Context* context)
// {
//     context->RegisterFactory<ProceduralSky>(Category_Geometry);
// 
//     URHO3D_COPY_BASE_ATTRIBUTES(StaticModel);
//     URHO3D_UPDATE_ATTRIBUTE_DEFAULT_VALUE("Reflection Mode", static_cast<int>(ReflectionMode::Zone));
// }
// 
// void ProceduralSky::ProcessRayQuery(const RayOctreeQuery& query, ea::vector<RayQueryResult>& results)
// {
//     // Do not record a raycast result for a skybox, as it would block all other results
// }
// 
// void ProceduralSky::Init(uint32_t verticalCount, uint32_t horizontalCount, Month month, float time, uint32_t cubemapSize, const Vector3& northDir)
// {
//     m_verticalCount = verticalCount;
//     m_horizontalCount = horizontalCount;
//     m_cubemapSize = cubemapSize;
//     m_sunLuminanceXYZ = std::make_unique<DynamicValueController>();
//     m_skyLuminanceXYZ = std::make_unique<DynamicValueController>();
//     m_sun = std::make_unique<SunController>();
//     m_sun->m_northDir = { northDir.x_, northDir.y_, northDir.z_ };
// 
//     ScreenPosVertex* vertices = (ScreenPosVertex*)malloc(verticalCount * horizontalCount * sizeof(ScreenPosVertex));
//     
//     for (uint32_t i = 0; i < verticalCount; i++)
//     {
//         for (uint32_t j = 0; j < horizontalCount; j++)
//         {
//             ScreenPosVertex& v = vertices[i * verticalCount + j];
//             v.m_x = float(j) / (horizontalCount - 1) * 2.0f - 1.0f;
//             v.m_y = float(i) / (verticalCount - 1) * 2.0f - 1.0f;
//         }
//     }
// 
//     uint16_t* indices = (uint16_t*)malloc((verticalCount - 1) * (horizontalCount - 1) * 6 * sizeof(uint16_t));
// 
//     uint32_t k = 0;
//     for (uint32_t i = 0; i < verticalCount - 1; i++)
//     {
//         for (uint32_t j = 0; j < horizontalCount - 1; j++)
//         {
//             indices[k++] = (uint16_t)(j + 0 + horizontalCount * (i + 0));
//             indices[k++] = (uint16_t)(j + 1 + horizontalCount * (i + 0));
//             indices[k++] = (uint16_t)(j + 0 + horizontalCount * (i + 1));
// 
//             indices[k++] = (uint16_t)(j + 1 + horizontalCount * (i + 0));
//             indices[k++] = (uint16_t)(j + 1 + horizontalCount * (i + 1));
//             indices[k++] = (uint16_t)(j + 0 + horizontalCount * (i + 1));
//         }
//     }
// 
//     SharedPtr<VertexBuffer> vertexBuffer(new VertexBuffer(context_));
//     ea::vector<VertexElement> elements;
//     elements.push_back(Urho3D::VertexElement(Urho3D::TYPE_VECTOR2, Urho3D::SEM_POSITION, 0, false));
//     vertexBuffer->SetSize(verticalCount * horizontalCount, elements);
//     vertexBuffer->SetData(vertices);
//     SharedPtr<IndexBuffer> indexBuffer(new IndexBuffer(context_));
//     indexBuffer->SetSize(k, false);
//     indexBuffer->SetData(indices);
// 
//     free(indices);
//     free(vertices);
// 
//     auto geometry = new Geometry(context_);
//     geometry->SetNumVertexBuffers(1);
//     geometry->SetVertexBuffer(0, vertexBuffer);
//     geometry->SetIndexBuffer(indexBuffer);
//     geometry->SetDrawRange(TRIANGLE_LIST, 0, k, false);
// 
//     auto model = new Model(context_);
//     model->SetNumGeometries(1);
//     model->SetGeometry(0, 0, geometry);
//     SetModel(model);
// 
//     auto tech = new Technique(context_);
//     auto pass = tech->CreatePass("base");
//     pass->SetDepthTestMode(CMP_EQUAL);
//     pass->SetCullMode(CULL_NONE);
//     pass->SetDepthWrite(false);
//     pass->SetVertexShader("sky/vs_sky");
//     pass->SetPixelShader("sky/fs_sky");
//     sky_ = new Material(context_);
//     sky_->SetTechnique(0, tech);
// 
//     tech = new Technique(context_);
//     pass = tech->CreatePass("base");
//     //pass->SetDepthTestMode(CMP_EQUAL);
//     pass->SetDepthTestMode(CMP_LESSEQUAL);
//     pass->SetCullMode(CULL_NONE);
//     pass->SetDepthWrite(false);
//     pass->SetVertexShader("sky/vs_sky");
//     pass->SetPixelShader("sky/fs_sky_color_banding_fix");
//     sky_color_banding_fix_ = new Material(context_);
//     sky_color_banding_fix_->SetTechnique(0, tech);
// 
//     SetMaterial(sky_color_banding_fix_);
// 
//     m_sunLuminanceXYZ->SetMap(sunLuminanceXYZTable);
//     m_skyLuminanceXYZ->SetMap(skyLuminanceXYZTable);
//     // m_timeOffset = bx::getHPCounter();
//     m_time = time;
//     m_timeScale = 1.0f;
// 
//     m_preventBanding = true;
//     m_turbidity = 2.15f;
//     m_sun->m_month = (SunController::Month)month;
// 
//     m_sun->Update(time);
// 
//     m_configDirty = true;
// }
// static void computePerezCoeff(float _turbidity, float* _outPerezCoeff)
// {
//     const bx::Vec3 turbidity = {_turbidity, _turbidity, _turbidity};
//     for (uint32_t ii = 0; ii < 5; ++ii)
//     {
//         const bx::Vec3 tmp = bx::mad(ABCDE_t[ii], turbidity, ABCDE[ii]);
//         float* out = _outPerezCoeff + 4 * ii;
//         bx::store(out, tmp);
//         out[3] = 0.0f;
//     }
// }
// 
// void ProceduralSky::update_uniform()
// {
//     auto mtl = GetMaterial();
//     mtl->SetShaderParameter("u_sunLuminance", GetSunLuminance());
//     auto skyLuminance = m_skyLuminanceXYZ->GetValue(m_time);
//     mtl->SetShaderParameter("u_skyLuminanceXYZ", *(Vector3*)&skyLuminance);
//     mtl->SetShaderParameter("u_skyLuminance", GetSkyLuminance());
//     auto sunDir = Vector4{ *(Vector3*)&m_sun->m_sunDir, 1.0f/*m_forzone ? 0.5f : 1.0f*/ };
//     mtl->SetShaderParameter("u_sunDirection", sunDir);
//     mtl->SetShaderParameter("u_parameters", Vector4{ 0.02f, 3.0f, 0.1f, m_time });
// 
//     Vector4 perezCoeff[5];
//     computePerezCoeff(m_turbidity, &perezCoeff[0].x_);
//     ea::vector<Variant> v = {perezCoeff[0], perezCoeff[1], perezCoeff[2], perezCoeff[3], perezCoeff[4]};
//     mtl->SetShaderParameter("u_perezCoeff", v);
// }
// 
// void ProceduralSky::SetTimeScale(float ts)
// {
//     m_timeScale = ts;
// }
// 
// void ProceduralSky::SetTime(float hour)
// {
//     m_time = Mod(m_timeScale * hour, 24.0f);
//     m_sun->Update(m_time);
//     m_configDirty = true;
// }
// 
// void ProceduralSky::SetMonth(Month m)
// {
//     m_sun->m_month = (SunController::Month)m;
//     m_sun->Update(m_time);
//     m_configDirty = true;
// }
// 
// Color ProceduralSky::GetSunLuminanceGamma()
// {
//     auto luminance = GetSunLuminance();
//     if (luminance.x_ > 0.0f || luminance.y_ > 0.0f || luminance.z_ > 0.0f) {
//         float parameterz = 0.1; // u_parameters.z : 0.1
//         return { Pow(luminance.x_ * 0.5f * parameterz, 1.0f / 2.2f), Pow(luminance.y_ * 0.5f * parameterz, 1.0f / 2.2f), Pow(luminance.z_ * 0.5f * parameterz, 1.0f / 2.2f) };
//     } else {
//         return Color::BLACK;
//     }
// }
// 
// Vector3 ProceduralSky::GetSunLuminance()
// {
//     auto sunLuminanceXYZ = m_sunLuminanceXYZ->GetValue(m_time);
//     auto result = xyzToRgb(sunLuminanceXYZ);
//     return *(Vector3*)&result;
// }
// 
// Color ProceduralSky::GetSkyLuminanceGamma()
// {
//     auto luminance = GetSkyLuminance();
//     if (luminance.x_ > 0.0f || luminance.y_ > 0.0f || luminance.z_ > 0.0f) {
//         float parameterz = 0.1; // u_parameters.z : 0.1
//         return { Pow(luminance.x_ * 0.5f * parameterz, 1.0f / 2.2f), Pow(luminance.y_ * 0.5f * parameterz, 1.0f / 2.2f), Pow(luminance.z_ * 0.5f * parameterz, 1.0f / 2.2f) };
//     } else {
//         return Color::BLACK;
//     }
// }
// 
// Vector3 ProceduralSky::GetSkyLuminance()
// {
//     auto skyLuminanceXYZ = m_skyLuminanceXYZ->GetValue(m_time);
//     auto result = xyzToRgb(skyLuminanceXYZ);
//     return *(Vector3*)&result;
// }
// 
// Vector4 ProceduralSky::GetExposition()
// {
//     return { 0.02f, 3.0f, 0.1f, m_time };
// }
// 
// Vector3 ProceduralSky::GetSunDirection()
// {
//     //return *(Vector3*)&m_sun->m_sunDir;
//     return { -m_sun->m_sunDir.x, -m_sun->m_sunDir.y, -m_sun->m_sunDir.z };
// }
// 
// void ProceduralSky::OnWorldBoundingBoxUpdate()
// {
//     // The skybox is supposed to be visible everywhere, so set a humongous bounding box
//     worldBoundingBox_.Define(-M_LARGE_VALUE, M_LARGE_VALUE);
// }
// 
// void ProceduralSky::OnCubemapRendered(TextureCube* texture)
// {
//     auto task = m_lookupMap[texture];
//     ea::vector<SharedPtr<Image>> cubeImages(MAX_CUBEMAP_FACES);
//     for (int i = 0; i < MAX_CUBEMAP_FACES; i++) {
//         cubeImages[i] = texture->GetImage((CubeMapFace)i);
//         assert(cubeImages[i]->readFrameNumber != 0);
//         if (task->finish_frame_number == UINT_MAX) {
//             task->finish_frame_number = cubeImages[i]->readFrameNumber;
//         }
//     }
//     task->cube_image->SetFaceImages(cubeImages, texture->GetWidth());
//     task->probe->SetEnabled(false);
//     m_readReadTask.push_back(task);
//     assert(m_readReadTask.size() <= MaxRTCount);
// }
// 
// void ProceduralSky::HandleBeginRendering(StringHash eventType, VariantMap& eventData)
// {
//     if (!enabled_) {
//         return;
//     }
//     if (!m_readReadTask.empty()) {
//         auto& task = m_readReadTask.front();
//         auto frameNumber = GetSubsystem<Graphics>()->GetCurrentFrameNumber();
//         if (frameNumber >= task->finish_frame_number) {
//             auto zone = GetNode()->GetScene()->GetComponent<Zone>(true);
//             zone->SetZoneTexture(task->probe->GetMixedProbeTexture());
//             zone->SetProcedurelImageCube(task->cube_image.Get());
//             task->finish_frame_number = UINT_MAX;
//             m_readReadTask.pop_front();
//         }
//     }
//     assert(m_readReadTask.size() < MaxRTCount);
//     if (m_configDirty) {
//         m_configDirty = false;
//         update_uniform();
//         update_environment_texture();
//     }
// }
// 
// void ProceduralSky::update_environment_texture()
// {
//     if (!m_scene) {
//         m_scene = new Scene(context_);
//         m_scene->SetName("ProceduralSkyEnvironment");
//         m_scene->CreateComponent<Octree>();
//         auto skyNode = m_scene->CreateChild("Sky");
//         auto sky = skyNode->CreateComponent<ProceduralSky>();
//         sky->UnsubscribeFromEvent(E_BEGINRENDERING);
//         auto& ndir = m_sun->m_northDir;
//         sky->Init(m_verticalCount, m_horizontalCount, (Month)m_sun->m_month, m_time, m_cubemapSize, { ndir.x, ndir.y, ndir.z });
//         m_internalSky = sky;
//         auto probeManager = m_scene->CreateComponent<ReflectionProbeManager>();
//         probeManager->SetAttribute("Filter Cubemaps", false);
//         probeManager->GetCubemapRenderer()->OnCubemapRendered.Subscribe(this, &ProceduralSky::OnCubemapRendered);
//         for (int i = 0; i < MaxRTCount; i++) {
//             auto probeNode = m_scene->CreateChild("Probe");
//             auto probe = probeNode->CreateComponent<ReflectionProbe>();
//             probe->SetEnabled(false);
//             probe->SetProbeType(ReflectionProbeType::Mixed);
//             probe->SetBoundingBox({Vector3{-1.0f, -1.0f, -1.0f}, Vector3{1.0f, 1.0f, 1.0f}});
// 
//             auto cubemap = probe->GetMixedProbeTexture();
//             m_renderTask[i].texture = cubemap;
//             m_renderTask[i].probe = probe;
//             m_renderTask[i].cube_image = new ImageCube(context_);
//             m_lookupMap.insert({ cubemap, &m_renderTask[i] });
//         }
// 
//         auto rttCameraNode = m_scene->CreateChild("Camera");
//         auto camera = rttCameraNode->CreateComponent<Camera>();
//         //camera->SetFarClip(100.0f);
//         camera->SetFov(90.0f);
//         camera->SetAspectRatio(1.0f);
// 
//         auto viewport = new Viewport(context_, m_scene, camera);
//         auto graphics = context_->GetSubsystem<Graphics>();
//         auto renderTexture = new Texture2D(context_);
//         renderTexture->SetSize(m_cubemapSize, m_cubemapSize, Graphics::GetRGBFormat(), TEXTURE_RENDERTARGET);
//         renderTexture->SetFilterMode(FILTER_BILINEAR);
//         auto surface = renderTexture->GetRenderSurface();
//         surface->SetViewport(0, viewport);
//         m_renderTexture = renderTexture;
//     }
//     m_internalSky->SetMonth((Month)m_sun->m_month);
//     m_internalSky->SetTime(m_time);
//     m_internalSky->update_uniform();
// 
//     auto& task = m_renderTask[m_freeRenderIndex];
//     task.probe->SetEnabled(true);
//     task.probe->QueueRender();
// 
//     auto surface = m_renderTexture->GetRenderSurface();
//     if (++m_freeRenderIndex >= MaxRTCount) {
//         m_freeRenderIndex = 0;
//     }
//     surface->QueueUpdate();
// }
// }
