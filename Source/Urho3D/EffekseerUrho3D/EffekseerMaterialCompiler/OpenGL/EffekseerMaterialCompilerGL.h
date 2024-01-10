
#pragma once

#include "../../Effekseer/Effekseer/Material/Effekseer.MaterialCompiler.h"
#include <vector>
#include "../../../RenderAPI/RenderAPIDefs.h"

namespace Effekseer
{

class MaterialCompilerGL : public MaterialCompiler, public ReferenceObject
{
private:
public:
    MaterialCompilerGL(Urho3D::RenderBackend backend) : backend_{ backend } {}

	virtual ~MaterialCompilerGL() = default;

	CompiledMaterialBinary* Compile(MaterialFile* materialFile, int32_t maximumUniformCount, int32_t maximumTextureCount);

	CompiledMaterialBinary* Compile(MaterialFile* materialFile) override;

	int AddRef() override
	{
		return ReferenceObject::AddRef();
	}

	int Release() override
	{
		return ReferenceObject::Release();
	}

	int GetRef() override
	{
		return ReferenceObject::GetRef();
	}
private:
    Urho3D::RenderBackend backend_;
};

} // namespace Effekseer
