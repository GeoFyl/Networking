#include "GameObject.h"

void GameObject::InitBox2d(float hx, float hy, float posx, float posy, b2World* world) {
	b2BodyDef body_def;
	body_def.type = b2_dynamicBody;
	body_def.position = b2Vec2(posx, posy);

	b2PolygonShape shape;
	shape.SetAsBox(hx, hy);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.f;
	fixture.friction = 0.5f;

	body_ = world->CreateBody(&body_def);
	body_->CreateFixture(&fixture);

	UpdateBox2d();
}

void GameObject::UpdateBox2d() {
	gef::Matrix44 transform;
	transform.SetIdentity();

	gef::Matrix44 rotation_z;
	rotation_z.RotationZ(body_->GetAngle());

	gef::Matrix44 translation;
	translation.SetIdentity();
	translation.SetTranslation(gef::Vector4(body_->GetPosition().x, body_->GetPosition().y, 0.f));

	transform = rotation_z * translation;
	set_transform(transform);
}