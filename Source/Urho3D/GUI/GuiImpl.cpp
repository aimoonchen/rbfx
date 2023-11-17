// #include "GuiImpl.h"
// 
// #include "CEGUI/System.h"
// #include "CEGUI/DefaultResourceProvider.h"
// #include "CEGUI/ImageManager.h"
// #include "CEGUI/Image.h"
// #include "CEGUI/Font.h"
// #include "CEGUI/Scheme.h"
// #include "CEGUI/WindowManager.h"
// #include "CEGUI/falagard/WidgetLookManager.h"
// #include "CEGUI/ScriptModule.h"
// #include "CEGUI/XMLParser.h"
// #include "CEGUI/GeometryBuffer.h"
// #include "CEGUI/GUIContext.h"
// #include "CEGUI/RenderTarget.h"
// #include "CEGUI/AnimationManager.h"
// #include "CEGUI/FontManager.h"
// #include "CEGUI/InputAggregator.h"
// 
// #include "CEGUIRenderer/Renderer.h"
// #include "CEGUIRenderer/ImageCodec.h"
// //test
// #include "CEGUI/widgets/DefaultWindow.h"
// #include "CEGUI/widgets/FrameWindow.h"
// #include "CEGUI/widgets/PushButton.h"
// #include "CEGUI/widgets/ProgressBar.h"
// 
// #if defined(__WIN32__) || defined(_WIN32)
// #ifndef WIN32_LEAN_AND_MEAN
// #define WIN32_LEAN_AND_MEAN
// #endif
// #ifndef NOMINMAX
// #define NOMINMAX
// #endif
// #include <windows.h>
// #endif
// 
// namespace Urho3D {
// 
// #ifndef CEGUI_SAMPLE_DATAPATH
// //#define CEGUI_SAMPLE_DATAPATH "../datafiles"
// #define CEGUI_SAMPLE_DATAPATH "D:/cegui/datafiles"
// #endif
// 
// const char CEGui::DATAPATH_VAR_NAME[] = "CEGUI_SAMPLE_DATAPATH";
// 
// CEGui::CEGui()
// {
// 	
// }
// 
// void CEGui::Initialize(Graphics* graphics)
// {
// 	CEGUI::String logFile{ "CEGUI.log" };
// 	String dataPathPrefixOverride;
// 
// 	d_renderer = &CEGUI::Urho3DRenderer::create(graphics);
// 
// 	if (!d_renderer)
// 		throw CEGUI::InvalidRequestException("CEGuiBaseApplication::run: Base application subclass did not create Renderer!");
// 
// 	// start up CEGUI system using objects created in subclass constructor.
// 	d_imageCodec = &((CEGUI::Urho3DRenderer*)d_renderer)->createUrho3DImageCodec();
// 	CEGUI::System::create(*d_renderer, d_resourceProvider, nullptr, d_imageCodec, nullptr, "", logFile);
// 
// 	// create viewport-based CEGUI context for the main window
// 	d_context = &CEGUI::System::getSingleton().createGUIContext(d_renderer->getDefaultRenderTarget());
// 
// 	auto& cegui_system = CEGUI::System::getSingleton();
// 	// initialise resource system
// 	initDataPathPrefix(dataPathPrefixOverride);
// 	initialiseResourceGroupDirectories();
// 	initialiseDefaultResourceGroups();
// 
// #ifdef DISABLE_CEGUI
// 	return;
// #endif
// 
// 	// Create a custom font which we use to draw the list items. This custom
// 	// font won't get effected by the scaler and such.
// 	CEGUI::FontManager& fontManager(CEGUI::FontManager::getSingleton());
// 	CEGUI::FontManager::getSingleton().createFromFile("DejaVuSans-12.font");
// 	CEGUI::FontManager::FontList loadedFonts = CEGUI::FontManager::getSingleton().createFromFile("msyh-14.font");
// 	//CEGUI::FontManager::FontList loadedFonts = CEGUI::FontManager::getSingleton().createFromFile("FZZYJ-14.font");
// 	CEGUI::Font* defaultFont = loadedFonts.empty() ? 0 : loadedFonts.front();
// 	// Set it as the default
// 	//d_context->setDefaultFont(defaultFont);
// 
// 	// load all the fonts (if they are not loaded yet)
// 	//fontManager.createAll("*.font", "fonts");
// 
// 	auto FZZYJFonts = CEGUI::FontManager::getSingleton().createFromFile("FZZYJ-14.font");
// 	auto FZZYJFont = loadedFonts.empty() ? 0 : loadedFonts.front();
// 	d_context->setDefaultFont(FZZYJFont);
// 
// 	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
// 	auto root = static_cast<CEGUI::DefaultWindow*>(winMgr.createWindow("DefaultWindow", "Root"));
// 	d_context->setRootWindow(root);
// 	/*
// 	CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
// 	CEGUI::FrameWindow* wnd = static_cast<CEGUI::FrameWindow*>(winMgr.createWindow("TaharezLook/FrameWindow", "Sample Window"));
// 	root->addChild(wnd);
// 	//wnd->setUsingAutoRenderingSurface(true);
// 	wnd->setPosition(CEGUI::UVector2(cegui_reldim(0.25f), cegui_reldim(0.25f)));
// 	wnd->setSize(CEGUI::USize(cegui_reldim(0.5f), cegui_reldim(0.5f)));
// 	wnd->setMaxSize(CEGUI::USize(cegui_reldim(1.0f), cegui_reldim(1.0f)));
// 	wnd->setMinSize(CEGUI::USize(cegui_reldim(0.1f), cegui_reldim(0.1f)));
// 	//wnd->setFont(FZZYJFont);
// 	wnd->setText(CEGUI::String(U"Hello World!【标题栏】"));
// 
// 	auto label0 = winMgr.createWindow("TaharezLook/StaticText", "testLabel0");
// 	root->addChild(label0);
// 	label0->setFont("msyh-14");
// 	label0->setPosition(CEGUI::UVector2(cegui_reldim(0.25f), cegui_reldim(0.75f)));
// 	label0->setSize(CEGUI::USize(cegui_reldim(0.5f), cegui_reldim(0.1f)));
// 	label0->setText(U"惟草木之零落兮，恐美人之迟暮。则为你如花美眷，似水流年。");
// 
// 	auto label1 = winMgr.createWindow("TaharezLook/StaticText", "testLabel1");
// 	root->addChild(label1);
// 	label1->setFont("FZZYJ-14");
// 	label1->setPosition(CEGUI::UVector2(cegui_reldim(0.25f), cegui_reldim(0.85f)));
// 	label1->setSize(CEGUI::USize(cegui_reldim(0.5f), cegui_reldim(0.1f)));
// 	label1->setText(U"惟草木之零落兮，恐美人之迟暮。则为你如花美眷，似水流年。");
// 
// 	//auto button = static_cast<CEGUI::PushButton*>(winMgr.createWindow("TaharezLook/ImageButton", "TestButton"));
// 	auto button = static_cast<CEGUI::PushButton*>(winMgr.createWindow("TaharezLook/Button", "TestButton"));
// 	root->addChild(button);
// 	button->setSize(CEGUI::USize(cegui_absdim(34.f), cegui_absdim(34.f)));
// 	button->setPosition(CEGUI::UVector2(cegui_absdim(0.0f),	cegui_absdim(0.0f)));
// 	button->setHorizontalAlignment(CEGUI::HorizontalAlignment::Right);
// 	button->setVerticalAlignment(CEGUI::VerticalAlignment::Top);
// 	button->setProperty("NormalImage", "TaharezLook/ButtonLeftNormal");
// 	button->setProperty("HoverImage", "TaharezLook/ButtonLeftHighlight");
// 	button->setProperty("PushedImage", "TaharezLook/ButtonLeftPushed");
// // 	button->subscribeEvent(
// // 		CEGUI::PushButton::EventClicked,
// // 		CEGUI::Event::Subscriber(&SampleBrowser::handleSampleExitButtonClicked, this));
// 	
// 	auto bar = static_cast<CEGUI::ProgressBar*>(winMgr.createWindow("TaharezLook/AlternateProgressBar", "TestProgressBar0"));
// 	root->addChild(bar);
// 	bar->setSize(CEGUI::USize(cegui_absdim(100.f), cegui_absdim(32.f)));
// 	bar->setPosition(CEGUI::UVector2(cegui_absdim(50.0f), cegui_absdim(50.0f)));
// 	bar->setProgress(0.5f);
// 
// 	bar = static_cast<CEGUI::ProgressBar*>(winMgr.createWindow("TaharezLook/ProgressBar", "TestProgressBar1"));
// 	root->addChild(bar);
// 	bar->setSize(CEGUI::USize(cegui_absdim(100.f), cegui_absdim(32.f)));
// 	bar->setPosition(CEGUI::UVector2(cegui_absdim(50.0f), cegui_absdim(100.0f)));
// 	bar->setProgress(0.5f);
// 
// 	bar = static_cast<CEGUI::ProgressBar*>(winMgr.createWindow("TaharezLook/VUMeter", "TestProgressBar2"));
// 	root->addChild(bar);
// 	bar->setSize(CEGUI::USize(cegui_absdim(100.f), cegui_absdim(32.f)));
// 	bar->setPosition(CEGUI::UVector2(cegui_absdim(50.0f), cegui_absdim(150.0f)));
// 	bar->setProgress(0.5f);
// 	*/
// 	// create logo imageset and draw the image (we only ever draw this once)
// 	CEGUI::ImageManager::getSingleton().addBitmapImageFromFile("cegui_logo", "logo.png");
// 
// 	CEGUI::Image& ceguiLogo = CEGUI::ImageManager::getSingleton().get("cegui_logo");
// 
// 	CEGUI::ImageRenderSettings imgRenderSettings(CEGUI::Rectf(0, 0, 183, 89));
// 
// 	auto ceguiLogoGeomBuffers = ceguiLogo.createRenderGeometry(imgRenderSettings);
// 
// 	d_logoGeometry.insert(d_logoGeometry.end(), ceguiLogoGeomBuffers.begin(), ceguiLogoGeomBuffers.end());
// 
// 	// initial position update of the logo
// 	updateLogoGeometry();
// 	// setup for spinning logo
// 	updateLogoGeometryRotation();
// 
// 	// clearing this queue actually makes sure it's created(!)
// 	d_context->clearGeometry(CEGUI::RenderQueueID::Overlay);
// 
// 	// subscribe handler to render overlay items
// 	d_context->subscribeEvent(CEGUI::RenderingSurface::EventRenderQueueStarted, CEGUI::Event::Subscriber(&CEGui::sampleBrowserOverlayHandler, this));
// 	
// 	// subscribe handler to reposition logo when window is sized.
// 	cegui_system.subscribeEvent(CEGUI::System::EventDisplaySizeChanged, CEGUI::Event::Subscriber(&CEGui::resizeHandler, this));
// 	
// 	
// 	d_systemInputAggregator = new CEGUI::InputAggregator(d_context);
// 	d_systemInputAggregator->initialise();
// 
// }
// 
// void CEGui::initialiseResourceGroupDirectories()
// {
// 	// initialise the required dirs for the DefaultResourceProvider
// 	auto rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
// 	CEGUI::String dataPathPrefix(getDataPathPrefix().CString());
// 
// 	/* for each resource type, set a resource group directory. We cast strings
// 	   to "const CEGUI::utf8*" in order to support general Unicode strings,
// 	   rather than only ASCII strings (even though currently they're all ASCII).
// 	   */
// 	rp->setResourceGroupDirectory("schemes", dataPathPrefix + "/schemes/");
// 	rp->setResourceGroupDirectory("imagesets", dataPathPrefix + "/imagesets/");
// 	rp->setResourceGroupDirectory("fonts", dataPathPrefix + "/fonts/");
// 	rp->setResourceGroupDirectory("layouts", dataPathPrefix + "/layouts/");
// 	rp->setResourceGroupDirectory("looknfeels", dataPathPrefix + "/looknfeel/");
// 	rp->setResourceGroupDirectory("lua_scripts", dataPathPrefix + "/lua_scripts/");
// 	rp->setResourceGroupDirectory("schemas", dataPathPrefix + "/xml_schemas/");
// 	rp->setResourceGroupDirectory("animations", dataPathPrefix + "/animations/");
// }
// 
// //----------------------------------------------------------------------------//
// void CEGui::initialiseDefaultResourceGroups()
// {
// 	// set the default resource groups to be used
// 	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
// 	CEGUI::Font::setDefaultResourceGroup("fonts");
// 	CEGUI::Scheme::setDefaultResourceGroup("schemes");
// 	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
// 	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
// 	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
// 	CEGUI::AnimationManager::setDefaultResourceGroup("animations");
// 
// 	// setup default group for validation schemas
// 	CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
// 	if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
// 		parser->setProperty("SchemaDefaultResourceGroup", "schemas");
// }
// 
// void CEGui::initDataPathPrefix(const String &override)
// {
// 
// 	if (override.Empty()) {
// 
// #       ifdef __APPLE__
// 
// 		char c_str[PATH_MAX];
// 		CFURLRef datafilesURL = CFBundleCopyResourceURL(CFBundleGetMainBundle(),
// 			CFSTR("datafiles"),
// 			0, 0);
// 		CFURLGetFileSystemRepresentation(datafilesURL, true,
// 			reinterpret_cast<UInt8*>(c_str),
// 			PATH_MAX);
// 		CFRelease(datafilesURL);
// 		d_dataPathPrefix = c_str;
// 
// #       else
// 
// 		// get data path from environment var
// 		char* envDataPath = getenv(DATAPATH_VAR_NAME);
// 
// 		// set data path prefix / base directory.  This will
// 		// be either from an environment variable, or from
// 		// a compiled in default based on original configure
// 		// options
// 		d_dataPathPrefix = envDataPath ? envDataPath : CEGUI_SAMPLE_DATAPATH;
// 
// #       endif
// 
// 	}
// 	else
// 		d_dataPathPrefix = override;
// 
// }
// //----------------------------------------------------------------------------//
// bool CEGui::resizeHandler(const CEGUI::EventArgs& /*args*/)
// {
// 	// clear FPS geometry and see that it gets recreated in the next frame
// 	const size_t bufferCount = d_FPSGeometry.size();
// 	for (size_t i = 0; i < bufferCount; ++i)
// 		d_renderer->destroyGeometryBuffer(*d_FPSGeometry.at(i));
// 	d_FPSGeometry.clear();
// 
// 	d_FPSValue = 0;
// 
// 	if (!d_context)
// 		return true;
// 
// // 	const Sizef& rtSize = d_context->getSurfaceSize();
// // 	const int width = static_cast<int>(rtSize.d_width);
// // 	const int height = static_cast<int>(rtSize.d_height);
// // 	d_sampleApp->handleNewWindowSize(width, height);
// 
// 	updateLogoGeometry();
// 	updateFPSGeometry();
// 
// 	return true;
// }
// 
// CEGUI::Window* CEGui::getRootWindow() const
// {
// 	if (d_context) {
// 		return d_context->getRootWindow();
// 	}
// 	return nullptr;
// }
// 
// //----------------------------------------------------------------------------//
// bool CEGui::sampleBrowserOverlayHandler(const CEGUI::EventArgs& args)
// {
// 	if (static_cast<const CEGUI::RenderQueueEventArgs&>(args).queueID != CEGUI::RenderQueueID::Overlay)
// 		return false;
// 
// 	// draw the logo
// // 	d_renderer->uploadBuffers(d_logoGeometry);
// // 	const size_t logo_buffer_count = d_logoGeometry.size();
// // 	for (size_t i = 0; i < logo_buffer_count; ++i)
// // 	{
// // 		d_logoGeometry[i]->draw();
// // 	}
// 
// 	// draw FPS value
// 	d_renderer->uploadBuffers(d_FPSGeometry);
// 	const size_t fps_buffer_count = d_FPSGeometry.size();
// 	for (size_t i = 0; i < fps_buffer_count; ++i)
// 		d_FPSGeometry.at(i)->draw();
// 
// 	return true;
// }
// 
// //----------------------------------------------------------------------------//
// bool CEGui::sampleOverlayHandler(const CEGUI::EventArgs& args)
// {
// 	if (static_cast<const CEGUI::RenderQueueEventArgs&>(args).queueID != CEGUI::RenderQueueID::Overlay)
// 		return false;
// 
// 	// Draw FPS value
// 	d_renderer->uploadBuffers(d_FPSGeometry);
// 	const size_t bufferCount = d_FPSGeometry.size();
// 	for (size_t i = 0; i < bufferCount; ++i)
// 		d_FPSGeometry.at(i)->draw();
// 
// 	return true;
// }
// 
// //----------------------------------------------------------------------------//
// void CEGui::updateFPS(const float elapsed)
// {
// 	// another frame
// 	++d_FPSFrames;
// 
// 	if ((d_FPSElapsed += elapsed) >= 1.0f)
// 	{
// 		if (d_FPSFrames != d_FPSValue)
// 		{
// 			d_FPSValue = d_FPSFrames;
// 
// 			CEGUI::Font* fnt = &CEGUI::FontManager::getSingleton().get("DejaVuSans-12");
// 			if (!fnt)
// 				return;
// 
// 			// update FPS imagery
// 			std::stringstream sstream;
// 			sstream << "FPS: " << d_FPSValue;
// 
// 			const size_t bufferCount = d_FPSGeometry.size();
// 			for (size_t i = 0; i < bufferCount; ++i)
// 				d_renderer->destroyGeometryBuffer(*d_FPSGeometry.at(i));
// 			d_FPSGeometry.clear();
// 
// 			auto textGeomBuffers = fnt->createTextRenderGeometry(
// 				sstream.str(), glm::vec2(0, 0), nullptr, false,
// 				CEGUI::Colour(0xFFFFFFFF), CEGUI::DefaultParagraphDirection::LeftToRight);
// 
// 			d_FPSGeometry.insert(d_FPSGeometry.end(), textGeomBuffers.begin(),
// 				textGeomBuffers.end());
// 
// 			updateFPSGeometry();
// 		}
// 
// 		// reset counter state
// 		d_FPSFrames = 0;
// 
// 		float modValue = 1.0f;
// 		d_FPSElapsed = std::modf(d_FPSElapsed, &modValue);
// 	}
// }
// 
// //----------------------------------------------------------------------------//
// void CEGui::updateLogo(const float elapsed)
// {
// 	if (!d_spinLogo)
// 		return;
// 
// 	static float rot = 0.0f;
// 
// 	const size_t bufferCount = d_logoGeometry.size();
// 	for (size_t i = 0; i < bufferCount; ++i) {
// 		d_logoGeometry[i]->setRotation(glm::quat(glm::vec3(glm::radians(rot), 0, 0)));
// 	}
// 
// 	rot = fmodf(rot + 180.0f * elapsed, 360.0f);
// }
// 
// void CEGui::updateLogoGeometry()
// {
// 	if (!d_context)
// 		return;
// 
// 	const CEGUI::Rectf scrn(d_context->getRenderTarget().getArea());
// 	const glm::vec3 position(10.0f, scrn.getSize().d_height - 89.0f, 0.0f);
// 
// 	const size_t bufferCount = d_logoGeometry.size();
// 	for (size_t i = 0; i < bufferCount; ++i)
// 	{
// 		d_logoGeometry[i]->setClippingRegion(scrn);
// 		d_logoGeometry[i]->setTranslation(position);
// 	}
// }
// 
// //----------------------------------------------------------------------------//
// void CEGui::updateFPSGeometry()
// {
// 	if (!d_context)
// 		return;
// 
// 	const CEGUI::Rectf scrn(d_context->getRenderTarget().getArea());
// 	const glm::vec3 position(scrn.getSize().d_width - 120.0f, 0.0f, 0.0f);
// 
// 	const size_t bufferCount = d_FPSGeometry.size();
// 	for (size_t i = 0; i < bufferCount; ++i)
// 	{
// 		d_FPSGeometry[i]->setClippingRegion(scrn);
// 		d_FPSGeometry[i]->setTranslation(position);
// 	}
// }
// 
// void CEGui::updateLogoGeometryRotation()
// {
// 	const size_t bufferCount = d_logoGeometry.size();
// 	for (size_t i = 0; i < bufferCount; ++i) {
// 		d_logoGeometry[i]->setPivot(glm::vec3(91.5f, 44.5f, 0));
// 	}
// }
// CEGUI::InputAggregator* CEGui::getCurrentInputAggregator()
// {
// 	// 		if (d_selectedSampleData != nullptr)
// 	// 			return d_selectedSampleData->getInputAggregator();
// 
// 	return d_systemInputAggregator;
// 	//return d_inputAggregator;
// }
// 
// void CEGui::Update(float timeStep)
// {
// #ifdef DISABLE_CEGUI
// 	return;
// #endif
// 	CEGUI::System& gui_system(CEGUI::System::getSingleton());
// 	gui_system.injectTimePulse(timeStep);
// 
// 	d_context->injectTimePulse(timeStep);
// // 	d_guiContext->injectTimePulse(timeStep);
// // 
//  	updateFPS(timeStep);
//  	updateLogo(timeStep);
// }
// 
// void CEGui::Render()
// {
// #ifdef DISABLE_CEGUI
// 	return;
// #endif
// 	CEGUI::System& gui_system(CEGUI::System::getSingleton());
// 	//		gui_system.injectTimePulse(elapsed);
// 	// 		d_sampleApp->update(static_cast<float>(elapsed));
// 	// 
// 	// 		updateFPS(elapsed);
// 	//		updateLogo(elapsed);
// 	// 
// 	//beginRendering(elapsed);
// 
// 	CEGUI::Renderer* gui_renderer(gui_system.getRenderer());
// 	gui_renderer->beginRendering();
// 
// 	//d_sampleApp->renderGUIContexts();
// 	//CEGUI::System& gui_system(CEGUI::System::getSingleton());
// 
// 	d_context->draw();
// 
// 	//d_guiContext->draw();
// 
// 	gui_renderer->endRendering();
// 
// 	CEGUI::WindowManager::getSingleton().cleanDeadPool();
// 
// 	//endRendering();
// }
// 
// void CEGui::Clear()
// {
// 	if (d_systemInputAggregator != nullptr) {
// 		delete d_systemInputAggregator;
// 		d_systemInputAggregator = nullptr;
// 	}
// }
// 
// 
// bool CEGui::injectKeyDown(const CEGUI::Key::Scan& ceguiKey)
// {
// 	if (CEGUI::Key::Scan::Esc != ceguiKey)
// 		return getCurrentInputAggregator()->injectKeyDown(ceguiKey);
// 	// 	else
// 	// 	{
// 	// 		if (d_selectedSampleData)
// 	// 			stopDisplaySample();
// 	// 		else
// 	// 			setQuitting(true);
// 	// 	}
// 
// 	return false;
// }
// 
// //----------------------------------------------------------------------------//
// bool CEGui::injectKeyUp(const CEGUI::Key::Scan& ceguiKey)
// {
// 	if (getCurrentInputAggregator() == nullptr)
// 		return false;
// 
// 	return getCurrentInputAggregator()->injectKeyUp(ceguiKey);
// }
// 
// //----------------------------------------------------------------------------//
// bool CEGui::injectChar(int character)
// {
// 	if (getCurrentInputAggregator() == nullptr)
// 		return false;
// 
// 	return getCurrentInputAggregator()->injectChar(character);
// }
// 
// //----------------------------------------------------------------------------//
// bool CEGui::injectMouseButtonDown(const CEGUI::MouseButton& ceguiMouseButton)
// {
// 	if (getCurrentInputAggregator() == nullptr)
// 		return false;
// 
// 	return getCurrentInputAggregator()->injectMouseButtonDown(ceguiMouseButton);
// }
// 
// //----------------------------------------------------------------------------//
// bool CEGui::injectMouseButtonUp(const CEGUI::MouseButton& ceguiMouseButton)
// {
// 	if (getCurrentInputAggregator() == nullptr)
// 		return false;
// 
// 	return getCurrentInputAggregator()->injectMouseButtonUp(ceguiMouseButton);
// }
// 
// //----------------------------------------------------------------------------//
// bool CEGui::injectMouseWheelChange(float position)
// {
// 	if (getCurrentInputAggregator() == nullptr)
// 		return false;
// 
// 	return getCurrentInputAggregator()->injectMouseWheelChange(position);
// }
// 
// //----------------------------------------------------------------------------//
// bool CEGui::injectMousePosition(float x, float y)
// {
// 	if (getCurrentInputAggregator() == nullptr)
// 		return false;
// 
// 	return getCurrentInputAggregator()->injectMousePosition(x, y);
// }
// 
// }
