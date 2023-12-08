#pragma once
#include <string>
#include <vector>

namespace LLGI
{
enum class DeviceType
{
    Default,
    DirectX12,
    Metal,
    Vulkan,
};
enum class ShaderStageType
{
    Vertex,
    Pixel,
    Compute,
    Max,
};
//Compiler* CreateCompiler(DeviceType device);

struct CompilerResult
{
    std::string Message;
    std::vector<std::vector<uint8_t>> Binary;
};

class Compiler// : public ReferenceObject
{
private:
public:
    Compiler() = default;
    virtual ~Compiler() = default;

    virtual void Initialize() {};
    virtual void Compile(CompilerResult& result, const char* code, ShaderStageType shaderStage) {};

    virtual DeviceType GetDeviceType() const { return DeviceType::Default; }
};

} // namespace LLGI
