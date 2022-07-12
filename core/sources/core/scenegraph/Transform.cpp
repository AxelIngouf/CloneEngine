#include "Transform.h"

namespace Core
{
	const Transform& Transform::operator=(const Transform& rhs)
	{
		position = rhs.position;
		rotation = rhs.rotation;
		scale = rhs.scale;
		return *this;
	}

	const Transform& Transform::operator+=(const Transform& rhs)
	{
		position = position + rotation * rhs.position;
		rotation *= rhs.rotation;
		scale *= rhs.scale;
		return *this;
	}

	Transform operator+(Transform lhs, const Transform& rhs)
	{
		lhs += rhs;
		return lhs;
	}

	bool operator==(const Transform& lhs, const Transform& rhs)
	{
		return lhs.position == rhs.position && lhs.rotation == rhs.rotation && lhs.scale == rhs.scale;

		// todo : test optimization value of something like :
		/*
		*	void* lhsBitView = (void*)lhs;
		*	void* rhsBitView = (void*)rhs;
		*	for (int i = 0; i < sizeof(transform); i += sizeof(void*))
		*	{
		*		if (lhsBitView[i] != rhsBitView[i])
		*		{
		*			return false;
		*		}
		*	}
		*	return true;
		*/
		// todo : test swapping void* for float*
	}

	bool operator!=(const Transform& lhs, const Transform& rhs)
	{
		return !(lhs == rhs);
	}
}
