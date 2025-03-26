#include "Physics.h"
#include "Components.h"
#include <iostream>


Vec2 Physics::getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// TODO: return the overlap rectangle size of the bounding boxes of entity a and b
	//		first: check to see if a and b actually have bounding boxes &&
	//			   check to see if a and b have Transform components
	//		second: if the above statement returns true, then compute and return Vec2
	// 
	//
	auto& transformA = a->getComponent<CTransform>();
	auto& transformB = b->getComponent<CTransform>();
	auto& boundingA = a->getComponent<CBoundingBox>();
	auto& boundingB = b->getComponent<CBoundingBox>();

	
	if (boundingA.has && boundingB.has)
	{
		if (transformA.has && transformB.has)
		{
			float overlapXa = boundingA.halfSize.x;
			float overlapXb = boundingB.halfSize.x;
			float overlapYa = boundingA.halfSize.y;
			float overlapYb = boundingB.halfSize.y;
			float dx = abs(transformA.pos.x - transformB.pos.x);
			float dy = abs(transformA.pos.y - transformB.pos.y);
			float overlapInX = (overlapXa + overlapXb)-dx;
			float overlapInY = (overlapYa + overlapYb)-dy;

			

			/*if (overlapInX > 0)
			{
				std::cout << "++++++++++++++++++++++++++++++OVERLAAAP:X +++++++++++++++++++++\n";
				std::cout << "ANIMATION IN A " << animationA.x << "\n";
				std::cout << "ANIMATION IN B " << animationB.x << "\n";
				std::cout << "OVERLAPX IN A " << overlapXa << "\n";
				std::cout << "OVERLAPX IN B " << overlapXb << "\n";
				std::cout << "A POS X " << transformA.pos.x << "\n";
				std::cout << "A POS Y " << transformA.pos.y << "\n";
				std::cout << "B POS X " << transformB.pos.x << "\n";
				std::cout << "DX " << dx << "\n";
				std::cout << "OVERLAPINX " << overlapInX << "\n";

			}*/

			/*if (overlapInY > 0)
			{
				std::cout << "++++++++++++++++++++++++++++++OVERLAAAP:Y +++++++++++++++++++++\n";
				std::cout << "OVERLAPY IN A " << overlapYa << "\n";
				std::cout << "OVERLAPY IN B " << overlapYb << "\n";
				std::cout << "A POS X " << transformA.pos.x << "\n";
				std::cout << "A POS Y " << transformA.pos.y << "\n";
				std::cout << "B POS Y " << transformB.pos.y << "\n";
				std::cout << "DX " << dy << "\n";
				std::cout << "OVERLAPINX " << overlapInY << "\n";

			}*/

			if (overlapInX > 0 && overlapInY > 0)
			{
				return Vec2(overlapInX, overlapInY);
			}



		}
	}
	
	return Vec2(0, 0);
	
	
}

Vec2 Physics::getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// TODO: return the previous overlap rectangle size of the bounding boxes of entity a and b
	//		previous overlap uses the entity's previous position
	//		EXACT same function GetOverlap(), but using the previous positions instead of the current
	//		ones
	//
	auto& transformA = a->getComponent<CTransform>();
	auto& transformB = b->getComponent<CTransform>();
	auto& boundingA = a->getComponent<CBoundingBox>();
	auto& boundingB = b->getComponent<CBoundingBox>();


	if (boundingA.has && boundingB.has)
	{
		if (transformA.has && transformB.has)
		{
			float overlapXa = boundingA.halfSize.x;
			float overlapXb = boundingB.halfSize.x;
			float overlapYa = boundingA.halfSize.y;
			float overlapYb = boundingB.halfSize.y;
			float dx = abs(transformA.prevPos.x - transformB.prevPos.x);
			float dy = abs(transformA.prevPos.y - transformB.prevPos.y);
			float overlapInX = (overlapXa + overlapXb) - dx;
			float overlapInY = (overlapYa + overlapYb) - dy;


			if (overlapInX > 0)
			{
				return Vec2(overlapInX, overlapInY);
			}
			if (overlapInY > 0)
			{
				return Vec2(overlapInX, overlapInY);
			}

		}
	}

	return Vec2(0, 0);
}

