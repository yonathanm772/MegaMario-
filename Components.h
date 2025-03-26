#pragma once

//#include "Vec2.h"
//#include <SFML/Graphics.hpp>
//#include <string>

#include "Animation.h"
#include "Assets.h"


class Component
{
public:
		bool has = false;
};

// tells where an entity is and how fast its going, and at what angle
class CTransform : public Component
{
public:
	Vec2 pos				= { 0, 0 };
	Vec2 prevPos			= { 0, 0 };
	Vec2 scale				= { 1.0, 1.0 };
	Vec2 velocity			= { 0, 0 };
	int lastDirection       = 1; //1 = facing right, -1 = facing left
	float angle				= 0; 
	
	

	CTransform() {}
	CTransform(const Vec2& p)
		:pos(p) {}
	CTransform(const Vec2& p, const Vec2& sp)
		:pos(p), velocity(sp) {}
	CTransform(const Vec2& p, const Vec2 &sp, const Vec2& sc,float a)
		:pos(p), prevPos(p),velocity(sp), scale(sc), angle(a){}
};

class CLifespan : public Component
{
public:
	//int remaining = 0;		// amount of lifespan remaining on the entity
	int lifespan = 0;			// amount of lifespan remaining on the entity
	//int total = 0;			// the total initial amount of lifespan
	int frameCreated = 0;		// the total initial amount of lifespan
	CLifespan() {}
	//CLifespan(int total)
	//	: remaining(total), total(total) {}
	//same logic as above
	CLifespan(int duration, int frame)
		: lifespan(duration), frameCreated(frame) {}
};

class CInput : public Component
{
public:
	bool up = false;
	bool left = false;
	bool right = false;
	bool down = false;
	bool shoot = false;
	bool canShoot = true;
	bool canJump = true;

	CInput() {}
};

class CBoundingBox : public Component
{
public:
	Vec2 size;
	Vec2 halfSize;
	CBoundingBox() {}
	CBoundingBox(const Vec2& s)
		:size(s), halfSize(s.x / 2, s.y / 2) {}
};

class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;
	CAnimation() {}
	CAnimation(const Animation& animation, bool r)
		:animation(animation), repeat(r) {}
};

class CGravity : public Component
{
public:
	float gravity = 0;
	CGravity() {}
	CGravity(float g) : gravity(g) {}
};

class CState : public Component
{
public:
	std::string state = "jumping";
	bool isGrounded = false;
	bool isRunning = false;
	CState() {}
	CState(std::string &s) : state(s) {}
};



