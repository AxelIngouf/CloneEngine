#pragma once
#include "core/ECS/Component.h"
#include "core/File.h"
#include "../PhysicsRigidActor.h"

namespace Model
{
	struct Vertex;
}

namespace Physics
{
	STRUCT(DynamicConvexMeshParams,
		FIELD(void*, vertices),
	    FIELD(File, file),
		FIELD(float, staticFriction),
		FIELD(float, dynamicFriction),
		FIELD(float, restitution),
		FIELD(float, angularDamping),
		FIELD(LibMath::Vector3, velocity),
		FIELD(bool, isTrigger),
		SUPPLEMENT(
			EMPTY(),
			DynamicConvexMeshParams(); ,
			EMPTY()
		)
	);

	COMPONENT(PhysicsDynamicConvexMeshComponent,
		OTHER_BASE(PhysicsRigidDynamic),
		INIT_PARAM(DynamicConvexMeshParams),
		FUNCTION(void, Initialize, const void*, params),
		FUNCTION(void, Constructor),
		FUNCTION(void, Finalize),
		FIELD(void*, vertices),
		FIELD(std::string, path, Core::FieldFlag::READONLY),
		FIELD(float, staticFriction),
		FIELD(float, dynamicFriction),
		FIELD(float, restitution),
		FIELD(float, angularDamping),
		FIELD(LibMath::Vector3, velocity),
		FIELD(bool, isTrigger),
		SUPPLEMENT(
			EMPTY(),
			PhysicsDynamicConvexMeshComponent() = default;
	        PhysicsDynamicConvexMeshComponent(PhysicsDynamicConvexMeshComponent&& other) noexcept; ,
		    EMPTY()
		)
	);



	STRUCT(StaticConvexMeshParams,
		FIELD(void*, vertices),
		FIELD(File, file),
		FIELD(float, staticFriction),
		FIELD(float, dynamicFriction),
		FIELD(float, restitution),
		FIELD(bool, isTrigger),
		SUPPLEMENT(
			EMPTY(),
			StaticConvexMeshParams();,
			EMPTY()
		)
	);

	COMPONENT(PhysicsStaticConvexMeshComponent,
		OTHER_BASE(PhysicsRigidStatic),
		INIT_PARAM(StaticConvexMeshParams),
		FUNCTION(void, Initialize, const void*, params),
		FUNCTION(void, Constructor),
		FUNCTION(void, Finalize),
		FIELD(void*, vertices),
		FIELD(std::string, path, Core::FieldFlag::READONLY),
		FIELD(float, staticFriction),
		FIELD(float, dynamicFriction),
		FIELD(float, restitution),
		FIELD(bool, isTrigger),
		SUPPLEMENT(
			EMPTY(),
			PhysicsStaticConvexMeshComponent() = default;
	        PhysicsStaticConvexMeshComponent(PhysicsStaticConvexMeshComponent&& other) noexcept;,
		EMPTY()
		)
	);



	STRUCT(TriangleMeshInitParams,
		FIELD(void*, vertices),
		FIELD(void*, indices),
		FIELD(File, file),
		FIELD(float, staticFriction),
		FIELD(float, dynamicFriction),
		FIELD(float, restitution),
		FIELD(bool, isTrigger),
		SUPPLEMENT(
			EMPTY(),
			TriangleMeshInitParams();,
			EMPTY()
		)
	);

	COMPONENT(PhysicsStaticTriangleMeshComponent,
		OTHER_BASE(PhysicsRigidStatic),
		INIT_PARAM(TriangleMeshInitParams),
		FUNCTION(void, Initialize, const void*, params),
		FUNCTION(void, Constructor),
		FUNCTION(void, Finalize),
		FIELD(void*, vertices),
		FIELD(void*, indices),
		FIELD(std::string, path, Core::FieldFlag::READONLY),
		FIELD(float, staticFriction),
		FIELD(float, dynamicFriction),
		FIELD(float, restitution),
		FIELD(bool, isTrigger),
		SUPPLEMENT(
			EMPTY(),
			PhysicsStaticTriangleMeshComponent() = default;
	        PhysicsStaticTriangleMeshComponent(PhysicsStaticTriangleMeshComponent&& other) noexcept; ,
		    EMPTY()
		)
	);
}
