#include "Urho3DContext.h"

Urho3D::Context* g_urho3d_context = nullptr;

void SetUrho3DContext(Urho3D::Context* context)
{
	g_urho3d_context = context;
}

Urho3D::Context* GetUrho3DContext()
{
	return g_urho3d_context;
}