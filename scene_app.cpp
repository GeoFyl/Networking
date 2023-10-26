#include "scene_app.h"
#include <system/platform.h>
#include <platform/d3d11/system/platform_d3d11.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <maths/math_utils.h>
#include <input/input_manager.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"


SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL),
	font_(NULL)
{
}

void SceneApp::Init()
{
	input_ = gef::InputManager::Create(platform_);

	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);

	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(platform_);

	//Initialise PhysX
	initPhysics();

	// setup the mesh for the player
	player_.set_mesh(primitive_builder_->CreateBoxMesh(gef::Vector4(0.5f, 0.75f, 0.5f)));
	player_.InitPhysx(gef::Vector4(0.5f, 0.75f, 0.5f, 0), gef::Vector4(0, 1.25f, 0, 0), gScene, gPhysics, true);
	player_.GetPxBody()->is<physx::PxRigidDynamic>()->setRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z | physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y);

	ground_.set_mesh(primitive_builder_->CreateBoxMesh(gef::Vector4(30.f, 0.5f, 30.f)));
	ground_.InitPhysx(gef::Vector4(30.f, 0.5f, 30.f, 0), gef::Vector4(0, 0, 0, 0), gScene, gPhysics);

	block_.set_mesh(primitive_builder_->CreateBoxMesh(gef::Vector4(0.5, 0.5, 0.5)));
	block_.InitPhysx(gef::Vector4(0.5, 0.5, 0.5, 0), gef::Vector4(2, 1, 0, 0), gScene, gPhysics);


	InitFont();
	SetupLights();

	// initialise imgui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	const gef::PlatformD3D11& platform_d3d = static_cast<const gef::PlatformD3D11&>(platform_);
	ImGui_ImplWin32_Init(platform_d3d.hwnd());
	ImGui_ImplDX11_Init(platform_d3d.device(), platform_d3d.device_context());
}

void SceneApp::initPhysics()
{
	using namespace physx;
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	
}

void SceneApp::CleanUp()
{
	CleanUpFont();

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void SceneApp::textToChat() {
	chatString_ += ChatBuff_;
	chatString_ += "\n";
	memset(ChatBuff_, 0, 100);
	
}

bool SceneApp::Update(float frame_time)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(20, platform_.height() - 200));
	ImGui::SetNextWindowSize(ImVec2(350, 180));
	ImGui::Begin("Chat", NULL, window_flags);
		ImGui::BeginChild("Child1", ImVec2(ImGui::GetWindowContentRegionWidth(), 120));
		ImGui::TextWrapped(chatString_.c_str());
		ImGui::SetScrollHere(1);
		ImGui::EndChild();
		ImGui::Text("Player 1:");
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::InputText("", ChatBuff_, 100, ImGuiInputTextFlags_EnterReturnsTrue)) { textToChat(); };
	ImGui::End();
	//ImGui::ShowDemoWindow(); // Show demo window! :)
	

	input_->Update();
	gef::Keyboard* keyInput = input_->keyboard();
	if (keyInput->IsKeyPressed(gef::Keyboard::KC_ESCAPE)) {
		return false;
	}

	fps_ = 1.0f / frame_time;

	gScene->simulate(frame_time);
	gScene->fetchResults(true);

	physx::PxVec3 playerDir(0, 0, 0);
	physx::PxRigidDynamic* playerBody = player_.GetPxBody()->is<physx::PxRigidDynamic>();

	if (keyInput->IsKeyPressed(gef::Keyboard::KC_ESCAPE)) {
		return false;
	}
	if (keyInput->IsKeyPressed(gef::Keyboard::KC_SPACE)) {
		playerBody->addForce(physx::PxVec3(0, 7, 0), physx::PxForceMode::eIMPULSE);
	}
	if (keyInput->IsKeyDown(gef::Keyboard::KC_W)) {
		playerDir.z -= 1;
	}
	if (keyInput->IsKeyDown(gef::Keyboard::KC_S)) {
		playerDir.z += 1;
	}
	if (keyInput->IsKeyDown(gef::Keyboard::KC_A)) {
		playerDir.x -= 1;
	}
	if (keyInput->IsKeyDown(gef::Keyboard::KC_D)) {
		playerDir.x += 1;
	}
	playerDir.normalize();
	physx::PxTransform newPlayerPos = playerBody->getGlobalPose();
	newPlayerPos.p = newPlayerPos.p + playerDir * frame_time * 3;
	playerBody->setGlobalPose(newPlayerPos);

	player_.UpdatePhysx();


	return true;
}

void SceneApp::Render()
{

	// setup camera

	// projection
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_.width() / (float)platform_.height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_.PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
	renderer_3d_->set_projection_matrix(projection_matrix);

	// view
	gef::Vector4 camera_eye(0.0f, 3.0f, 10.0f);
	gef::Vector4 camera_lookat(0.0f, 0.0f, 0.0f);
	gef::Vector4 camera_up(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_view_matrix(view_matrix);

	// draw 3d geometry
	renderer_3d_->Begin();

	renderer_3d_->set_override_material(NULL);
	renderer_3d_->DrawMesh(ground_);
	renderer_3d_->set_override_material(&primitive_builder_->red_material());
	renderer_3d_->DrawMesh(player_);
	renderer_3d_->set_override_material(&primitive_builder_->blue_material());
	renderer_3d_->DrawMesh(block_);

	renderer_3d_->set_override_material(NULL);

	renderer_3d_->End();

	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
void SceneApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void SceneApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void SceneApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

void SceneApp::SetupLights()
{
	// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();

	// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.25f, 0.25f, 0.25f, 1.0f));

	// add a point light that is almost white, but with a blue tinge
	// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
}
