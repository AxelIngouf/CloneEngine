#pragma once
#include "physic_export.h"

#include <vector>

#define PX_FOUNDATION_DLL 0
#include "PxPhysicsAPI.h"
#include "Vector/Vector3.h"

namespace Core
{
	class Entity;
}

namespace Model
{
	struct Vertex;
}

namespace LibMath
{
	struct Quaternion;
}

namespace Physics
{
	class PhysicsRigidStatic;
	class PhysicsRigidDynamic;
	enum class EGeometryType;
	class PhysicsInstance;

	/**
	 * Physics singleton used to encapsulate physx. This class should not be used by the used and is only intended for an engine internal use.
	 */
	class PhysicsInstance
	{
	public:
		PhysicsInstance(const PhysicsInstance&) = delete;
		PhysicsInstance(PhysicsInstance&&) = delete;
		PhysicsInstance& operator=(const PhysicsInstance&) = delete;
		PhysicsInstance& operator=(PhysicsInstance&&) = delete;

		~PhysicsInstance();

		/**
		 * Initializes physx context.
		 */
		void Init();

		/**
		 * Advance the simulation of given amount of time.
		 * 
		 * @param delta Amount of time to simulate.
		 */
		static void Advance(float delta);

		/**
		 * Completely clears the scene of all rigid actors and create a new one.
		 */
		static void ClearScene();

		/**
		 * Replaces default scene gravity by the given one.
		 * 
		 * @param gravity New gravity.
		 */
		static void SetDefaultGravity(const physx::PxVec3& gravity);

		/**
		 * Creates a rigid dynamic corresponding to the given parameters.
		 * 
		 * @param transform Transform (location, rotation).
		 * @param geometry Geometry (capsule, sphere, box or convex mesh).
		 * @param geometryType Geometry type enum value.
		 * @param material Physics material.
		 * @param velocity Initial velocity.
		 * @param density Density.
		 * @param angularDamping Angular damping.
		 * @return Pointer to the created rigid dynamic.
		 */
		PhysicsRigidDynamic* CreateRigidDynamic(const physx::PxTransform& transform, const physx::PxGeometry& geometry,
		                                        const EGeometryType& geometryType, physx::PxMaterial& material,
		                                        const physx::PxVec3& velocity, float density, float angularDamping);

		/**
		 * Creates a rigid dynamic with capsule shape that stays upright.
		 * 
		 * @param transform Transform (location, rotation).
		 * @param material Physics material.
		 * @param radius Capsule radius.
		 * @param halfHeight Capsule half height.
		 * @param velocity Initial velocity.
		 * @param density Density.
		 * @param angularDamping Angular damping.
		 * @return Pointer to the created rigid dynamic capsule.
		 */
		PhysicsRigidDynamic* CreateRigidDynamicCapsule(const physx::PxTransform& transform, physx::PxMaterial& material,
		                                               float radius, float halfHeight, const physx::PxVec3& velocity,
		                                               float density, float angularDamping);

		/**
		 * Creates a rigid static corresponding to the given parameters.
		 * 
		 * @param transform Transform (location, rotation).
		 * @param geometry Geometry (capsule, sphere, box or convex mesh).
		 * @param geometryType Geometry type enum value.
		 * @param material Physics material.
		 * @return Pointer to the created rigid static.
		 */
		PhysicsRigidStatic* CreateRigidStatic(const physx::PxTransform& transform, const physx::PxGeometry& geometry,
		                                      const EGeometryType& geometryType, physx::PxMaterial& material);

		/**
		 * Converts a vector of vertices to a physx readable convex mesh.
		 * 
		 * @param vertices Vector of vertices.
		 * @return physx convex mesh.
		 */
		static physx::PxConvexMesh* ConvexMeshFromMesh(const std::vector<Model::Vertex>& vertices);

		/**
		 * Converts a given location and rotation to a physx transform.
		 * 
		 * @param location Location vector.
		 * @param rotation Rotation quaternion.
		 * @return physx transform.
		 */
		static physx::PxTransform TransformFromLocationAndRotation(const LibMath::Vector3& location,
		                                                           const LibMath::Quaternion& rotation);

		/**
		 * Returns the current physx scene pointer.
		 * 
		 * @return Current physx scene pointer
		 */
		static physx::PxScene* GetScene() { return physicsInstance.scene; }

		/**
		 * Returns the current physx physics object pointer.
		 * 
		 * @return Current physx physics object pointer.
		 */
		static physx::PxPhysics* GetPhysics() { return physicsInstance.physics; }

		/**
		 * PhysicsInstance singleton.
		 */
		PHYSIC_EXPORT static PhysicsInstance physicsInstance;

	private:
		PhysicsInstance() = default;

		/**
		 * Creates and initialize the physx scene.
		 */
		void CreateScene();

		/**
		 * Register and already generated physx rigid dynamic and puts inside a PhysicsRigidDynamic component.
		 * 
		 * @param rigidDynamic physx rigid dynamic pointer
		 * @param geometryType rigid dynamic geometry type (box, sphere, capsule, upright capsule or convex mesh)
		 * @return Physics rigid dynamic component pointer.
		 */
		PhysicsRigidDynamic* RegisterRigidDynamic(physx::PxRigidDynamic* rigidDynamic,
		                                          const EGeometryType& geometryType);

		/**
		 * Register and already generated physx rigid static and puts inside a PhysicsRigidStatic component.
		 *
		 * @param rigidStatic physx rigid static pointer
		 * @param geometryType rigid static geometry type (box, sphere, capsule or convex mesh)
		 * @return Physics rigid static component pointer.
		 */
		PhysicsRigidStatic* RegisterRigidStatic(physx::PxRigidStatic* rigidStatic, const EGeometryType& geometryType);

		/**
		 * physx internal function override used to determine whether or not two objects can collide.
		 * 
		 * @param pairFlags Collision pair flags (to indicate if a pair of rigids can trigger a collision notification).
		 * @return physx internal filter flags.
		 */
		static physx::PxFilterFlags ContactReportFilterShader(physx::PxFilterObjectAttributes /*attributes0*/,
		                                                      physx::PxFilterData /*filterData0*/,
		                                                      physx::PxFilterObjectAttributes /*attributes1*/,
		                                                      physx::PxFilterData /*filterData1*/,
		                                                      physx::PxPairFlags& pairFlags,
		                                                      const void* /*constantBlock*/,
		                                                      physx::PxU32 /*constantBlockSize*/);

		/**
		 * Notify all scene active actors to update their render transform.
		 */
		static void NotifyActiveActors();

		/* physx variables */
		physx::PxFoundation* foundation = nullptr;
		physx::PxPhysics* physics = nullptr;
		physx::PxPvd* pvd = nullptr;
		physx::PxPvdTransport* transport = nullptr;
		physx::PxCooking* cooking = nullptr;
		physx::PxScene* scene = nullptr;
		physx::PxDefaultCpuDispatcher* dispatcher = nullptr;

		/* constants */
		physx::PxReal objectsAverageLength = 100;
		physx::PxReal objectsAverageSpeed = 981;
		physx::PxVec3 defaultGravity{0.0f, -9.81f, 0.0f};
		int nbThreads = 2;
	};

	/**
	 * Converts a physx vector3 to a LibMath vector3.
	 * 
	 * @param vec3 physx vector3 to converts.
	 * @return Converted LibMath vector3.
	 */
	LibMath::Vector3 Vec3Convert(const physx::PxVec3& vec3);

	/**
	 * Converts a LibMath vector3 to a physx vector3.
	 *
	 * @param vec3 LibMath vector3 to converts.
	 * @return Converted physx vector3.
	 */
	physx::PxVec3 Vec3Convert(const LibMath::Vector3& vec3);

	/**
	 * Converts a physx quaternion to a LibMath quaternion.
	 *
	 * @param quaternion physx quaternion to converts.
	 * @return Converted LibMath quaternion.
	 */
	LibMath::Quaternion QuaternionConvert(const physx::PxQuat& quaternion);

	/**
	 * Converts a LibMath quaternion to a physx quaternion.
	 *
	 * @param quaternion LibMath quaternion to converts.
	 * @return Converted physx quaternion.
	 */
	physx::PxQuat QuaternionConvert(const LibMath::Quaternion& quaternion);
}
