//
// Copyright (c) 2008-2019 the Urho3D project.
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

#include "../Core/Object.h"
#include "../Core/Timer.h"
#include "../Input/InputConstants.h"
#include "../Graphics/VertexBuffer.h"
#include <memory>

// #include "../Gui/Cursor.h"
// #include "../Gui/UIBatch.h"

#define URHO3D_CEGUI 1
#define URHO3D_FARYGUI 2

#define URHO3D_GUI URHO3D_FARYGUI

namespace CEGUI
{
	class Window;
}
namespace cocos2d
{
    class Scene;
}
namespace fairygui
{
	class GRoot;
	class GComponent;
}
namespace Urho3D
{
class UIElement;
class Sprite;
class FairyGUIImpl;
class CEGui;
	/// Font hinting level (only used for FreeType fonts)
// 	enum FontHintLevel
// 	{
// 		/// Completely disable font hinting. Output will be blurrier but more "correct".
// 		FONT_HINT_LEVEL_NONE = 0,
// 
// 		/// Light hinting. FreeType will pixel-align fonts vertically, but not horizontally.
// 		FONT_HINT_LEVEL_LIGHT,
// 
// 		/// Full hinting, using either the font's own hinting or FreeType's auto-hinter.
// 		FONT_HINT_LEVEL_NORMAL
// 	};

// 	class Cursor;
// 	class Graphics;
// 	class ResourceCache;
	class Timer;
// 	class UIBatch;
// 	class UIElement;
// 	class XMLElement;
// 	class XMLFile;
// 	class RenderSurface;
// 	class UIComponent;

	/// %Gui subsystem. Manages the graphical user interface.
	class URHO3D_API GUI : public Object
	{
		URHO3D_OBJECT(GUI, Object);

	public:
		/// Construct.
		explicit GUI(Context* context);
		/// Destruct.
		~GUI() override;

// 		/// Set cursor Gui element.
// 		void SetCursor(Cursor* cursor);
// 		/// Set focused Gui element.
// 		void SetFocusElement(UIElement* element, bool byKey = false);
// 		/// Set modal element. Until all the modal elements are dismissed, all the inputs and events are only sent to them. Return true when successful.
// 		/// Only the modal element can clear its modal status or when it is being destructed.
// 		bool SetModalElement(UIElement* modalElement, bool enable);
		/// Clear the Gui (excluding the cursor.)
		void Clear();
		/// Update the Gui logic. Called by HandlePostUpdate().
		void Update(float timeStep);
		/// Update the Gui for rendering. Called by HandleRenderUpdate().
		void RenderUpdate();
		/// Render the Gui. If renderUICommand is false (default), is assumed to be the default Gui render to backbuffer called by Engine, and will be performed only once. Additional Gui renders to a different rendertarget may be triggered from the renderpath.
		void Render(bool renderUICommand = false);
        void SetDesignResolutionSize(int w, int h);
		/// Debug draw a Gui element.
//		void DebugDraw(UIElement* element);
// 		/// Load a Gui layout from an XML file. Optionally specify another XML file for element style. Return the root element.
// 		SharedPtr<UIElement> LoadLayout(Deserializer& source, XMLFile* styleFile = nullptr);
// 		/// Load a Gui layout from an XML file. Optionally specify another XML file for element style. Return the root element.
// 		SharedPtr<UIElement> LoadLayout(XMLFile* file, XMLFile* styleFile = nullptr);
// 		/// Save a Gui layout to an XML file. Return true if successful.
// 		bool SaveLayout(Serializer& dest, UIElement* element);
		/// Set clipboard text.
		void SetClipboardText(const ea::string& text);
		/// Set Gui element double click interval in seconds.
		void SetDoubleClickInterval(float interval);
		/// Set max screen distance in pixels between double click clicks.
		void SetMaxDoubleClickDistance(float distPixels);
		/// Set Gui drag event start interval in seconds.
		void SetDragBeginInterval(float interval);
		/// Set Gui drag event start distance threshold in pixels.
		void SetDragBeginDistance(int pixels);
		/// Set tooltip default display delay in seconds.
		void SetDefaultToolTipDelay(float delay);
		/// Set maximum font face texture size. Must be a power of two. Default is 2048.
		void SetMaxFontTextureSize(int size);
		/// Set whether mouse wheel can control also a non-focused element.
		void SetNonFocusedMouseWheel(bool nonFocusedMouseWheel);
		/// Set whether to use system clipboard. Default false.
		void SetUseSystemClipboard(bool enable);
		/// Set whether to show the on-screen keyboard (if supported) when a %LineEdit is focused. Default true on mobile devices.
		void SetUseScreenKeyboard(bool enable);
		/// Set whether to use mutable (eraseable) glyphs to ensure a font face never expands to more than one texture. Default false.
		void SetUseMutableGlyphs(bool enable);
		/// Set whether to force font autohinting instead of using FreeType's TTF bytecode interpreter.
		void SetForceAutoHint(bool enable);
		/// Set the hinting level used by FreeType fonts.
		//void SetFontHintLevel(FontHintLevel level);
		/// Set the font subpixel threshold. Below this size, if the hint level is LIGHT or NONE, fonts will use subpixel positioning plus oversampling for higher-quality rendering. Has no effect at hint level NORMAL.
		void SetFontSubpixelThreshold(float threshold);
		/// Set the oversampling (horizonal stretching) used to improve subpixel font rendering. Only affects fonts smaller than the subpixel limit.
		void SetFontOversampling(int oversampling);
		/// Set %Gui scale. 1.0 is default (pixel perfect). Resize the root element to match.
		void SetScale(float scale);
		/// Scale %Gui to the specified width in pixels.
		void SetWidth(float width);
		/// Scale %Gui to the specified height in pixels.
		void SetHeight(float height);
		/// Set custom size of the root element. This disables automatic resizing of the root element according to window size. Set custom size 0,0 to return to automatic resizing.
		void SetCustomSize(const IntVector2& size);
		/// Set custom size of the root element.
		void SetCustomSize(int width, int height);

		/// Return root Gui element.
// 		UIElement* GetRoot() const { return rootElement_; }
// 
// 		/// Return root modal element.
// 		UIElement* GetRootModalElement() const { return rootModalElement_; }
// 
// 		/// Return cursor.
// 		Cursor* GetCursor() const { return cursor_; }

		/// Return cursor position.
		IntVector2 GetCursorPosition() const;
		/// Return Gui element at global screen coordinates. By default returns only input-enabled elements.
// 		UIElement* GetElementAt(const IntVector2& position, bool enabledOnly = true);
// 		/// Return Gui element at global screen coordinates. By default returns only input-enabled elements.
// 		UIElement* GetElementAt(int x, int y, bool enabledOnly = true);
// 		/// Get a child element at element's screen position relative to specified root element.
// 		UIElement* GetElementAt(UIElement* root, const IntVector2& position, bool enabledOnly = true);
// 
// 		/// Return focused element.
// 		UIElement* GetFocusElement() const { return focusElement_; }
// 
// 		/// Return topmost enabled root-level non-modal element.
// 		UIElement* GetFrontElement() const;
// 		/// Return currently dragged elements.
// 		const Vector<UIElement*> GetDragElements();

		/// Return the number of currently dragged elements.
		unsigned GetNumDragElements() const { return (unsigned)dragConfirmedCount_; }

		/// Return the drag element at index.
		//UIElement* GetDragElement(unsigned index);
		/// Return clipboard text.
		const ea::string& GetClipboardText() const;

		/// Return Gui element double click interval in seconds.
		float GetDoubleClickInterval() const { return doubleClickInterval_; }

		/// Get max screen distance in pixels for double clicks to register. 
		float GetMaxDoubleClickDistance() const { return maxDoubleClickDist_; }

		/// Return Gui drag start event interval in seconds.
		float GetDragBeginInterval() const { return dragBeginInterval_; }

		/// Return Gui drag start event distance threshold in pixels.
		int GetDragBeginDistance() const { return dragBeginDistance_; }

		/// Return tooltip default display delay in seconds.
		float GetDefaultToolTipDelay() const { return defaultToolTipDelay_; }

		/// Return font texture maximum size.
		int GetMaxFontTextureSize() const { return maxFontTextureSize_; }

		/// Return whether mouse wheel can control also a non-focused element.
		bool IsNonFocusedMouseWheel() const { return nonFocusedMouseWheel_; }

		/// Return whether is using the system clipboard.
		bool GetUseSystemClipboard() const { return useSystemClipboard_; }

		/// Return whether focusing a %LineEdit will show the on-screen keyboard.
		bool GetUseScreenKeyboard() const { return useScreenKeyboard_; }

		/// Return whether is using mutable (eraseable) glyphs for fonts.
		bool GetUseMutableGlyphs() const { return useMutableGlyphs_; }

		/// Return whether is using forced autohinting.
		bool GetForceAutoHint() const { return forceAutoHint_; }

		/// Return the current FreeType font hinting level.
		//FontHintLevel GetFontHintLevel() const { return fontHintLevel_; }

		/// Get the font subpixel threshold. Below this size, if the hint level is LIGHT or NONE, fonts will use subpixel positioning plus oversampling for higher-quality rendering. Has no effect at hint level NORMAL.
		float GetFontSubpixelThreshold() const { return fontSubpixelThreshold_; }

		/// Get the oversampling (horizonal stretching) used to improve subpixel font rendering. Only affects fonts smaller than the subpixel limit.
		int GetFontOversampling() const { return fontOversampling_; }

		/// Return true when Gui has modal element(s).
		bool HasModalElement() const;

		/// Return whether a drag is in progress.
		bool IsDragging() const { return dragConfirmedCount_ > 0; };

		/// Return current Gui scale.
		float GetScale() const { return uiScale_; }

		/// Return root element custom size. Returns 0,0 when custom size is not being used and automatic resizing according to window size is in use instead (default.)
		const IntVector2& GetCustomSize() const { return customSize_; }
		bool IsCaptured() const { return captured_; }
		CEGUI::Window* GetCEGUIRoot();
		fairygui::GRoot* GetFairyGUIRoot();
        void SetFairyGUIRoot(fairygui::GRoot* root);
// 		void CreateJoystick(fairygui::GComponent* view);
// 		float GetJoystickRotation() const;
//         int GetJoystickTouchID() const;
// 		bool IsJoystickCapture() const;
        bool IsFocusUI() const;
        bool IsInputing() const;
		/// Set texture to which element will be rendered.
		//void SetElementRenderTexture(UIElement* element, Texture2D* texture);

		/// Data structure used to represent the drag data associated to a UIElement.
		struct DragData
		{
			/// Which button combo initiated the drag.
			MouseButtonFlags dragButtons;
			/// How many buttons initiated the drag.
			int numDragButtons;
			/// Sum of all touch locations
			IntVector2 sumPos;
			/// Flag for a drag start event pending.
			bool dragBeginPending;
			/// Timer used to trigger drag begin event.
			Timer dragBeginTimer;
			/// Drag start position.
			IntVector2 dragBeginSumPos;
		};
        void CreateJoystick(const IntVector2& area, float scale);
        void EnableJoystick(bool enabled);
        int GetJoystickTouchID() const { return activeTouchId_.value_or(-1); }
        float GetJoystickDegree() const { return degree_; }
	private:
// 		void updateFPS(const float elapsed);
// 		void updateLogo(const float elapsed);
// 		void updateLogoGeometry();
// 		void updateFPSGeometry();
// 		void updateLogoGeometryRotation();
// 		CEGUI::InputAggregator* getCurrentInputAggregator();
// 		//! event handler function that draws the logo and FPS overlay elements.
// 		bool sampleBrowserOverlayHandler(const CEGUI::EventArgs& args);
// 		//! event handler function that draws the FPS overlay elements.
// 		bool sampleOverlayHandler(const CEGUI::EventArgs& args);
// 		//! event handler function called when main view is resized
// 		bool resizeHandler(const CEGUI::EventArgs& args);
// 		void initialiseResourceGroupDirectories();
// 		void initialiseDefaultResourceGroups();
// 		bool injectKeyDown(const CEGUI::Key::Scan& ceguiKey);
// 		bool injectKeyUp(const CEGUI::Key::Scan& ceguiKey);
// 		bool injectChar(int character);
// 		bool injectMouseButtonDown(const CEGUI::MouseButton& ceguiMouseButton);
// 		bool injectMouseButtonUp(const CEGUI::MouseButton& ceguiMouseButton);
// 		bool injectMouseWheelChange(float position);
// 		bool injectMousePosition(float x, float y);

		// Urho3D proxy
		void OnMouseButtonDown(const IntVector2& windowCursorPos, MouseButton mouseButtons);
		void OnMouseButtonUp(const IntVector2& windowCursorPos, MouseButton mouseButtons);
		void OnMouseMove(float x, float y);
		/// Initialize when screen mode initially set.
		void Initialize();
		/// Update Gui element logic recursively.
// 		void Update(float timeStep, UIElement* element);
// 		/// Upload Gui geometry into a vertex buffer.
// 		void SetVertexData(VertexBuffer* dest, const PODVector<float>& vertexData);
// 		/// Render Gui batches to the current rendertarget. Geometry must have been uploaded first.
// 		void Render(VertexBuffer* buffer, const PODVector<UIBatch>& batches, unsigned batchStart, unsigned batchEnd);
// 		/// Generate batches from an Gui element recursively. Skip the cursor element.
// 		void GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, UIElement* element, IntRect currentScissor);
// 		/// Return Gui element at global screen coordinates. Return position converted to element's screen coordinates.
// 		UIElement* GetElementAt(const IntVector2& position, bool enabledOnly, IntVector2* elementScreenPosition);
// 		/// Return Gui element at screen position recursively.
// 		void GetElementAt(UIElement*& result, UIElement* current, const IntVector2& position, bool enabledOnly);
// 		/// Return the first element in hierarchy that can alter focus.
// 		UIElement* GetFocusableElement(UIElement* element);
		/// Return cursor position and visibility either from the cursor element, or the Input subsystem.
		void GetCursorPositionAndVisible(IntVector2& pos, bool& visible);
		/// Set active cursor's shape.
		//void SetCursorShape(CursorShape shape);
		/// Force release of font faces when global font properties change.
		void ReleaseFontFaces();
		/// Handle button or touch hover.
		void ProcessHover(const IntVector2& windowCursorPos, MouseButtonFlags buttons, QualifierFlags qualifiers/*, Cursor* cursor*/);
		/// Handle button or touch begin.
		void ProcessClickBegin(const IntVector2& windowCursorPos, MouseButton button, MouseButtonFlags buttons, QualifierFlags qualifiers/*, Cursor* cursor, bool cursorVisible*/);
		/// Handle button or touch end.
		void ProcessClickEnd(const IntVector2& windowCursorPos, MouseButton button, MouseButtonFlags buttons, QualifierFlags qualifiers/*, Cursor* cursor, bool cursorVisible*/);
		/// Handle mouse or touch move.
		void ProcessMove(const IntVector2& windowCursorPos, const IntVector2& cursorDeltaPos, MouseButtonFlags buttons, QualifierFlags qualifiers/*, Cursor* cursor, bool cursorVisible*/);
		/// Send a Gui element drag or hover begin event.
// 		void SendDragOrHoverEvent(StringHash eventType, UIElement* element, const IntVector2& screenPos, const IntVector2& deltaPos, Gui::DragData* dragData);
// 		/// Send a Gui click event.
// 		void SendClickEvent(StringHash eventType, UIElement* beginElement, UIElement* endElement, const IntVector2& pos, MouseButton button, MouseButtonFlags buttons, QualifierFlags qualifiers);
// 		/// Send a Gui double click event
// 		void SendDoubleClickEvent(UIElement* beginElement, UIElement* endElement, const IntVector2& firstPos, const IntVector2& secondPos, MouseButton button, MouseButtonFlags buttons, QualifierFlags qualifiers);
		/// Handle screen mode event.
		void HandleScreenMode(StringHash eventType, VariantMap& eventData);
		/// Handle mouse button down event.
		void HandleMouseButtonDown(StringHash eventType, VariantMap& eventData);
		/// Handle mouse button up event.
		void HandleMouseButtonUp(StringHash eventType, VariantMap& eventData);
		/// Handle mouse move event.
		void HandleMouseMove(StringHash eventType, VariantMap& eventData);
		/// Handle mouse wheel event.
		void HandleMouseWheel(StringHash eventType, VariantMap& eventData);
		/// Handle touch begin event.
		void HandleTouchBegin(StringHash eventType, VariantMap& eventData);
		/// Handle touch end event.
		void HandleTouchEnd(StringHash eventType, VariantMap& eventData);
		/// Handle touch move event.
		void HandleTouchMove(StringHash eventType, VariantMap& eventData);
		/// Handle keypress event.
		void HandleKeyDown(StringHash eventType, VariantMap& eventData);
		/// Handle keyup event.
		void HandleKeyUp(StringHash eventType, VariantMap& eventData);
		/// Handle text input event.
		void HandleTextInput(StringHash eventType, VariantMap& eventData);
		/// Handle frame begin event.
		void HandleBeginFrame(StringHash eventType, VariantMap& eventData);
		/// Handle logic post-update event.
		void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
		/// Handle render update event.
		void HandleRenderUpdate(StringHash eventType, VariantMap& eventData);
		/// Handle a file being drag-dropped into the application window.
		//void HandleDropFile(StringHash eventType, VariantMap& eventData);
		/// Remove drag data and return next iterator.
		//HashMap<WeakPtr<UIElement>, DragData*>::Iterator DragElementErase(HashMap<WeakPtr<UIElement>, DragData*>::Iterator i);
		/// Handle clean up on a drag cancel.
		void ProcessDragCancel();
		/// Sum touch positions and return the begin position ready to send.
		IntVector2 SumTouchPositions(GUI::DragData* dragData, const IntVector2& oldSendPos);
		/// Resize root element to effective size.
		void ResizeRootElement();
		/// Return effective size of the root element, according to Gui scale and resolution / custom size.
		IntVector2 GetEffectiveRootElementSize(bool applyScale = true) const;
// 		/// Graphics subsystem.
// 		WeakPtr<Graphics> graphics_;
// 		/// Gui root element.
// 		SharedPtr<UIElement> rootElement_;
// 		/// Gui root modal element.
// 		SharedPtr<UIElement> rootModalElement_;
// 		/// Cursor.
// 		SharedPtr<Cursor> cursor_;
// 		/// Currently focused element.
// 		WeakPtr<UIElement> focusElement_;
// 		/// Gui rendering batches.
// 		PODVector<UIBatch> batches_;
// 		/// Gui rendering vertex data.
// 		PODVector<float> vertexData_;
// 		/// Gui rendering batches for debug draw.
// 		PODVector<UIBatch> debugDrawBatches_;
// 		/// Gui rendering vertex data for debug draw.
// 		PODVector<float> debugVertexData_;
// 		/// Gui vertex buffer.
// 		SharedPtr<VertexBuffer> vertexBuffer_;
// 		/// Gui debug geometry vertex buffer.
// 		SharedPtr<VertexBuffer> debugVertexBuffer_;
// 		/// Gui element query vector.
// 		PODVector<UIElement*> tempElements_;
		/// Clipboard text.
		mutable ea::string clipBoard_;
		/// Seconds between clicks to register a double click.
		float doubleClickInterval_;
		/// Seconds from mouse button down to begin a drag if there has been no movement exceeding pixel threshold.
		float dragBeginInterval_;
		/// Tooltip default display delay in seconds.
		float defaultToolTipDelay_;
		/// Drag begin event distance threshold in pixels.
		int dragBeginDistance_;
		/// Mouse buttons held down.
		MouseButtonFlags mouseButtons_;
		/// Last mouse button pressed.
		MouseButtonFlags lastMouseButtons_;
		/// Qualifier keys held down.
		QualifierFlags qualifiers_;
		/// Font texture maximum size.
		int maxFontTextureSize_;
		/// Initialized flag.
		bool initialized_;
		/// Touch used flag.
		bool usingTouchInput_;
		/// Flag to switch mouse wheel event to be sent to non-focused element at cursor.
		bool nonFocusedMouseWheel_;
		/// Flag for using operating system clipboard instead of internal.
		bool useSystemClipboard_;
		/// Flag for showing the on-screen keyboard on focusing a %LineEdit.
		bool useScreenKeyboard_;
		/// Flag for using mutable (erasable) font glyphs.
		bool useMutableGlyphs_;
		/// Flag for forcing FreeType auto hinting.
		bool forceAutoHint_;
		/// FreeType hinting level (default is FONT_HINT_LEVEL_NORMAL).
		//FontHintLevel fontHintLevel_;
		/// Maxmimum font size for subpixel glyph positioning and oversampling (default is 12).
		float fontSubpixelThreshold_;
		/// Horizontal oversampling for subpixel fonts (default is 2).
		int fontOversampling_;
		/// Flag for Gui already being rendered this frame.
		bool uiRendered_;
		/// Non-modal batch size (used internally for rendering).
		unsigned nonModalBatchSize_;
		/// Timer used to trigger double click.
		Timer clickTimer_;
		/// Gui element last clicked for tracking double clicks.
//		WeakPtr<UIElement> doubleClickElement_;
		/// Screen position of first mouse click for double click distance checking.
		IntVector2 doubleClickFirstPos_;
		/// Max screen distance the first click in a double click can be from the second click in a double click.
		float maxDoubleClickDist_;
		/// Currently hovered elements.
// 		HashMap<WeakPtr<UIElement>, bool> hoveredElements_;
// 		/// Currently dragged elements.
// 		HashMap<WeakPtr<UIElement>, DragData*> dragElements_;
		/// Number of elements in dragElements_.
		int dragElementsCount_;
		/// Number of elements in dragElements_ with dragPending = false.
		int dragConfirmedCount_;
		/// Gui elements that are being touched with touch input.
// 		HashMap<WeakPtr<UIElement>, MouseButtonFlags> touchDragElements_;
// 		/// Confirmed drag elements cache.
// 		Vector<UIElement*> dragElementsConfirmed_;
		/// Current scale of Gui.
		float uiScale_;
		/// Root element custom size. 0,0 for automatic resizing (default.)
		IntVector2 customSize_;
		/// Elements that should be rendered to textures.
		//HashMap<UIElement*, RenderToTextureData> renderToTexture_;
#if URHO3D_GUI == URHO3D_CEGUI
		std::unique_ptr<CEGui> gui_impl_;
#elif URHO3D_GUI == URHO3D_FARYGUI
		std::unique_ptr<FairyGUIImpl> gui_impl_;
#endif
		bool captured_{ false };
        //
        bool enable_joystick_{ false };
        /// Identifier of active touch
        ea::optional<int> activeTouchId_{};
        SharedPtr<UIElement> joystickArea_;
        SharedPtr<Sprite> joystickBackground_;
        SharedPtr<Sprite> joystickCenter_;
        SharedPtr<Sprite> joystickThumb_;
        IntVector2 touchPosition_{};
        IntRect bgRect_{2, 2, 2 + 238, 2 + 238};
        IntRect centerRect_{85, 242, 85 + 95, 242 + 95};
        IntRect thumbRect_{2, 242, 2 + 81, 242 + 189};
        IntVector2 offset_{64, 64};
        float scale_{1.0f};
        float degree_{0.0f};
	};

	/// Register Gui library objects.
	//void URHO3D_API RegisterUILibrary(Context* context);

}
