#pragma once
#include "graphics/mesh_instance.h"
#include <box2d/box2d.h>
class GameObject : public gef::MeshInstance {
public:
	void InitBox2d(float hx, float hy, float posx, float posy, b2World* world);
	void UpdateBox2d();
	b2Body* GetBody() { return body_; }

protected:
	b2Body* body_;
};

