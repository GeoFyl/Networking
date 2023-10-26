#pragma once
#include "graphics/mesh_instance.h"
//#include <box2d/box2d.h>
#include <PxPhysicsAPI.h>
class GameObject : public gef::MeshInstance {
public:
	//void InitBox2d(float hx, float hy, float posx, float posy, b2World* world);
	void InitPhysx(gef::Vector4 halfExtent, gef::Vector4 pos, physx::PxScene* scene, physx::PxPhysics* gPhysics, bool dynamic = false);
	//void UpdateBox2d();
	void UpdatePhysx();
	//b2Body* GetBody() { return body_; }
	physx::PxRigidActor* GetPxBody() { return pxbody_; }

protected:
	//b2Body* body_;
	physx::PxRigidActor* pxbody_;

	physx::PxMaterial* gMaterial = NULL;
};

