#include "GameObject.h"

//void GameObject::InitBox2d(float hx, float hy, float posx, float posy, b2World* world) {
//	b2BodyDef body_def;
//	body_def.type = b2_dynamicBody;
//	body_def.position = b2Vec2(posx, posy);
//	
//
//	b2PolygonShape shape;
//	shape.SetAsBox(hx, hy);
//
//	b2FixtureDef fixture;
//	fixture.shape = &shape;
//	fixture.density = 1.f;
//	fixture.friction = 0.5f;
//
//	body_ = world->CreateBody(&body_def);
//	body_->CreateFixture(&fixture);
//	body_->GetUserData().pointer = (uintptr_t)this;
//
//	UpdateBox2d();
//}

void GameObject::InitPhysx(gef::Vector4 halfExtent, gef::Vector4 pos, physx::PxScene* scene, physx::PxPhysics* gPhysics, bool dynamic) {
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
	physx::PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(halfExtent.x(), halfExtent.y(), halfExtent.z()), *gMaterial);

	physx::PxTransform transform(physx::PxVec3(pos.x(), pos.y(), pos.z()));
	if (dynamic) pxbody_ = gPhysics->createRigidDynamic(transform);
	else pxbody_ = gPhysics->createRigidStatic(transform);
	pxbody_->attachShape(*shape);
	if (dynamic) physx::PxRigidBodyExt::updateMassAndInertia(*pxbody_->is<physx::PxRigidDynamic>(), 1.0f);
	scene->addActor(*pxbody_);
	
	shape->release();

	UpdatePhysx();
}

//void GameObject::UpdateBox2d() {
//	gef::Matrix44 transform;
//	transform.SetIdentity();
//
//	gef::Matrix44 rotation_z;
//	rotation_z.RotationZ(body_->GetAngle());
//
//	gef::Matrix44 translation;
//	translation.SetIdentity();
//	translation.SetTranslation(gef::Vector4(body_->GetPosition().x, body_->GetPosition().y, 0.f));
//
//	transform = rotation_z * translation;
//	set_transform(transform);
//}

void GameObject::UpdatePhysx() {
	gef::Matrix44 transform(physx::PxMat44(pxbody_->getGlobalPose()).front());

	/*gef::Matrix44 rotation;
	pxbody_.*/

	/*gef::Matrix44 translation;
	translation.SetIdentity();
	translation.SetTranslation(gef::Vector4(body_->GetPosition().x, body_->GetPosition().y, 0.f));

	transform = rotation_z * translation;*/
	set_transform(transform);
}