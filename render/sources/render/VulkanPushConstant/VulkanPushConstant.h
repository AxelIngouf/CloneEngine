#pragma once
#include "Matrix/Matrix4.h"

namespace Render
{
	struct VulkanPushConstant
	{
		LibMath::Matrix4 model = LibMath::Matrix4::Identity();
	};
}
