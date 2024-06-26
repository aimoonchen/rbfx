//
// Copyright (c) 2008-2022 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "Sample.h"

#include <Urho3D/UI/Text.h>
#include <Urho3D/XR/VirtualReality.h>

namespace Urho3D
{

class Node;
class Scene;

} // namespace Urho3D

/// Simple VR
/// This sample demonstrates:
///     - Initializing XR
///     - Displaying XR with a StereoRenderPipeline
///     - Simple teleportation locomotion
class HelloVR : public Sample
{
    URHO3D_OBJECT(HelloVR, Sample);

public:
    /// Construct.
    explicit HelloVR(Context* context);

    /// Setup after engine initialization and before running the main loop.
    void Start() override;
    /// Shutdown OpenXR
    void Stop() override;

private:
    /// Periodically update the scene.
    void Update();
    /// Return VR status string.
    ea::string GetStatus() const;

    /// Construct the scene content.
    void CreateScene();
    /// Construct an instruction text to the UI.
    void CreateInstructions();
    /// Set up a viewport for displaying the scene.
    void SetupViewport();
    /// Update hand components.
    void SetupHandComponents(Node* handPoseNode, Node* handAimNode);
    /// Grab dynamic object with hand.
    void GrabDynamicObject(Node* handNode, VRHand hand);
    /// Release dynamic object from hand.
    void ReleaseDynamicObject(Node* handNode);

    /// Container of all interactive dynamic objects
    SharedPtr<Node> dynamicObjects_;
    /// Text element displayed on the flat screen.
    SharedPtr<Text> statusText_;
};
