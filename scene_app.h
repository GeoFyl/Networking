#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <Windows.h>
#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include "GameObject.h"
#include <input/keyboard.h>
#include <PxPhysicsAPI.h>
#include <string>
# include <functional>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void SetupLights();
	void initPhysics();
	void textToChat();

	/*std::function<ImGuiTextEditCallbackData()> test = [&](ImGuiTextEditCallbackData* testData) {
		chatString_ = "hello";
	};*/

	gef::InputManager* input_;
    
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Renderer3D* renderer_3d_;

	PrimitiveBuilder* primitive_builder_;

	//gef::Platform* platform_;

	GameObject player_;
	GameObject ground_;
	GameObject block_;

	physx::PxDefaultAllocator		gAllocator;
	physx::PxDefaultErrorCallback	gErrorCallback;
	physx::PxFoundation* gFoundation = NULL;
	physx::PxPhysics* gPhysics = NULL;
	physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
	physx::PxScene* gScene = NULL;

	char ChatBuff_[100] = "";
	std::string chatString_ = "";

	float fps_;
};

#endif // _SCENE_APP_H
