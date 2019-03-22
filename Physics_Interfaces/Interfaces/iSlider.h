#pragma once
#include <glm/mat4x4.hpp>
#include "Interfaces/sRigidBodyDef.h"
#include "iCompondBody.h"

namespace nPhysics
{
	struct sSliderDef
	{
		glm::vec3 PositionA;
		glm::vec3 PositionB;
		sRigidBodyDef sliderRbDef;
		iShape* SliderShape;
	};
	class iSliderComponent {

	public:
		virtual ~iSliderComponent() {}
		virtual void GetSliderTransform(glm::mat4& transformOut) = 0;


	protected:
		iSliderComponent() : iCompoundBody() {}
		iSliderComponent(const iSliderComponent& other) : iCompoundBody(other)


	};

}