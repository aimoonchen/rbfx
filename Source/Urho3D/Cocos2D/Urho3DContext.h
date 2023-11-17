#pragma once
#ifdef URHO3D_IS_BUILDING
#include "Urho3D.h"
#else
#include <Urho3D/Urho3D.h>
#endif
namespace Urho3D {
	class Context;
}
URHO3D_API void SetUrho3DContext(Urho3D::Context* context);
URHO3D_API Urho3D::Context* GetUrho3DContext();