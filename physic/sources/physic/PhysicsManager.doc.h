#pragma once
#include "physic_export.h"
#include <vector>

#include "Vector/Vector3.h"
#include "Quaternion/Quaternion.h"

namespace Core
{
	struct Vertex;
}

namespace Physics
{
	class PhysicsRigidDynamic;
	class PhysicsRigidStatic;

	/**
	 * Debug line containing two points and a single color.
	 */
	struct DebugLine
	{
		DebugLine() = delete;

		DebugLine(const LibMath::Vector3& first, const LibMath::Vector3& second, const uint32_t col)
			: firstPoint(first), secondPoint(second), color(col)
		{
		}

		/**
		 * Line first point.
		 */
		LibMath::Vector3 firstPoint;

		/**
		 * Line second point.
		 */
		LibMath::Vector3 secondPoint;

		/**
		 * Line color.
		 */
		uint32_t color;
	};

	/**
	 * Initializes the engine physics.
	 */
	PHYSIC_EXPORT void InitPhysics();

	/**
	 * Advance the simulation of given amount of time.
	 *
	 * @param delta Amount of time to simulate.
	 */
	PHYSIC_EXPORT void Advance(float delta);

	/**
	 * Creates a dynamic Convex mesh collider corresponding to the given parameters.
	 * 
	 * @param location Location.
	 * @param rotation Rotation.
	 * @param vertices Mesh vertices vector.
	 * @param staticFriction Surface static friction.
	 * @param dynamicFriction Surface dynamic friction.
	 * @param restitution Surface restitution.
	 * @param velocity Initial velocity.
	 * @param density Density.
	 * @param angularDamping Angular damping.
	 * @return Pointer to the created collider as a Physics rigid dynamic.
	 */
	PHYSIC_EXPORT PhysicsRigidDynamic* CreateConvexMeshRigidDynamic(const LibMath::Vector3& location,
	                                                                const LibMath::Quaternion& rotation,
	                                                                const std::vector<Model::Vertex>& vertices,
	                                                                float staticFriction = .5f,
	                                                                float dynamicFriction = .5f,
	                                                                float restitution = .1f,
	                                                                const LibMath::Vector3& velocity = LibMath::Vector3(
		                                                                0), float density = 10.f,
	                                                                float angularDamping = 0.5f);

	/**
	 * Creates a dynamic Sphere collider corresponding to the given parameters.
	 *
	 * @param location Location.
	 * @param rotation Rotation.
	 * @param radius Sphere radius.
	 * @param staticFriction Surface static friction.
	 * @param dynamicFriction Surface dynamic friction.
	 * @param restitution Surface restitution.
	 * @param velocity Initial velocity.
	 * @param density Density.
	 * @param angularDamping Angular damping.
	 * @return Pointer to the created collider as a Physics rigid dynamic.
	 */
	PHYSIC_EXPORT PhysicsRigidDynamic* CreateSphereRigidDynamic(const LibMath::Vector3& location,
	                                                            const LibMath::Quaternion& rotation, float radius,
	                                                            float staticFriction = .5f, float dynamicFriction = .5f,
	                                                            float restitution = .1f,
	                                                            const LibMath::Vector3& velocity = LibMath::Vector3(0),
	                                                            float density = 10.f,
	                                                            float angularDamping = 0.5f);

	/**
	 * Creates a dynamic Capsule collider corresponding to the given parameters.
	 *
	 * @param location Location.
	 * @param rotation Rotation.
	 * @param radius Capsule radius.
	 * @param halfHeight Capsule half height.
	 * @param staticFriction Surface static friction.
	 * @param dynamicFriction Surface dynamic friction.
	 * @param restitution Surface restitution.
	 * @param velocity Initial velocity.
	 * @param density Density.
	 * @param angularDamping Angular damping.
	 * @return Pointer to the created collider as a Physics rigid dynamic.
	 */
	PHYSIC_EXPORT PhysicsRigidDynamic* CreateCapsuleRigidDynamic(const LibMath::Vector3& location,
	                                                             const LibMath::Quaternion& rotation, float radius,
	                                                             float halfHeight,
	                                                             float staticFriction = .5f,
	                                                             float dynamicFriction = .5f, float restitution = .1f,
	                                                             const LibMath::Vector3& velocity = LibMath::Vector3(0),
	                                                             float density = 10.f,
	                                                             float angularDamping = 0.5f);

	/**
	 * Creates a dynamic Box collider corresponding to the given parameters.
	 *
	 * @param location Location.
	 * @param rotation Rotation.
	 * @param extents Box extents.
	 * @param staticFriction Surface static friction.
	 * @param dynamicFriction Surface dynamic friction.
	 * @param restitution Surface restitution.
	 * @param velocity Initial velocity.
	 * @param density Density.
	 * @param angularDamping Angular damping.
	 * @return Pointer to the created collider as a Physics rigid dynamic.
	 */
	PHYSIC_EXPORT PhysicsRigidDynamic* CreateBoxRigidDynamic(const LibMath::Vector3& location,
	                                                         const LibMath::Quaternion& rotation,
	                                                         const LibMath::Vector3& extents,
	                                                         float staticFriction = .5f, float dynamicFriction = .5f,
	                                                         float restitution = .1f,
	                                                         const LibMath::Vector3& velocity = LibMath::Vector3(0),
	                                                         float density = 10.f,
	                                                         float angularDamping = 0.5f);

	/**
	 * Creates a static Convex mesh collider corresponding the given parameters.
	 * 
	 * @param location Location.
	 * @param rotation Rotation.
	 * @param vertices Mesh vertices vector.
	 * @param staticFriction Surface static friction.
	 * @param dynamicFriction Surface dynamic friction.
	 * @param restitution Surface restitution.
	 * @return Pointer to the created collider as a Physics rigid static.
	 */
	PHYSIC_EXPORT PhysicsRigidStatic* CreateMeshRigidStatic(const LibMath::Vector3& location,
	                                                        const LibMath::Quaternion& rotation,
	                                                        const std::vector<Model::Vertex>& vertices,
	                                                        float staticFriction = .5f, float dynamicFriction = .5f,
	                                                        float restitution = .1f);

	/**
	 * Creates a static Sphere collider corresponding the given parameters.
	 *
	 * @param location Location.
	 * @param rotation Rotation.
	 * @param radius Sphere radius.
	 * @param staticFriction Surface static friction.
	 * @param dynamicFriction Surface dynamic friction.
	 * @param restitution Surface restitution.
	 * @return Pointer to the created collider as a Physics rigid static.
	 */
	PHYSIC_EXPORT PhysicsRigidStatic* CreateSphereRigidStatic(const LibMath::Vector3& location,
	                                                          const LibMath::Quaternion& rotation, float radius,
	                                                          float staticFriction = .5f, float dynamicFriction = .5f,
	                                                          float restitution = .1f);

	/**
	 * Creates a static Capsule collider corresponding the given parameters.
	 *
	 * @param location Location.
	 * @param rotation Rotation.
	 * @param radius Capsule radius.
	 * @param halfHeight Capsule half height.
	 * @param staticFriction Surface static friction.
	 * @param dynamicFriction Surface dynamic friction.
	 * @param restitution Surface restitution.
	 * @return Pointer to the created collider as a Physics rigid static.
	 */
	PHYSIC_EXPORT PhysicsRigidStatic* CreateCapsuleRigidStatic(const LibMath::Vector3& location,
	                                                           const LibMath::Quaternion& rotation, float radius,
	                                                           float halfHeight, float staticFriction = .5f,
	                                                           float dynamicFriction = .5f, float restitution = .1f);

	/**
	 * Creates a static Box collider corresponding the given parameters.
	 *
	 * @param location Location.
	 * @param rotation Rotation.
	 * @param extents Box extents.
	 * @param staticFriction Surface static friction.
	 * @param dynamicFriction Surface dynamic friction.
	 * @param restitution Surface restitution.
	 * @return Pointer to the created collider as a Physics rigid static.
	 */
	PHYSIC_EXPORT PhysicsRigidStatic* CreateBoxRigidStatic(const LibMath::Vector3& location,
	                                                       const LibMath::Quaternion& rotation,
	                                                       const LibMath::Vector3& extents,
	                                                       float staticFriction = .5f, float dynamicFriction = .5f,
	                                                       float restitution = .1f);

	/**
	 * Sets the default scene gravity.
	 * 
	 * @param gravity New scene gravity.
	 */
	PHYSIC_EXPORT void SetSceneGravity(LibMath::Vector3& gravity);

	/**
	 * Returns the current scene gravity.
	 * 
	 * @return Current scene gravity.
	 */
	PHYSIC_EXPORT LibMath::Vector3 GetSceneGravity();

	/**
	 * Enables or disables the physics to generate debug lines.
	 * 
	 * @param debug Boolean value indicating if physx should or not generate debug lines.
	 */
	PHYSIC_EXPORT void EnableDebugVisualization(bool debug);

	/**
	 * Adds physics debug lines to the given vector.
	 * 
	 * @param debugLines Vector of current physics debug lines.
	 */
	PHYSIC_EXPORT void GetDebugLines(std::vector<DebugLine>& debugLines);

	/**
	 * Clean the scene of all its actors and create a new one.
	 */
	PHYSIC_EXPORT void CleanScene();
}
