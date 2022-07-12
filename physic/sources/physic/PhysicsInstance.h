#pragma once
#include "physic_export.h"

#include <vector>

#define PX_FOUNDATION_DLL 0
#include "PxPhysicsAPI.h"
#include "Vector/Vector3.h"

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

	class PhysicsInstance
	{
	public:
		PhysicsInstance(const PhysicsInstance&) = delete;
		PhysicsInstance(PhysicsInstance&&) = delete;
		PhysicsInstance& operator=(const PhysicsInstance&) = delete;
		PhysicsInstance& operator=(PhysicsInstance&&) = delete;

		~PhysicsInstance();

		void Init();

		static void Advance(float delta);

		static void ClearScene();

		static void SetDefaultGravity(const physx::PxVec3& gravity);

		void CreateRigidDynamic(PhysicsRigidDynamic* rigidDynamic, const physx::PxTransform& transform,
		                        const physx::PxGeometry& geometry,
		                        const EGeometryType& geometryType, physx::PxMaterial& material,
		                        const physx::PxVec3& velocity, float density, float angularDamping) const;

		void CreateRigidDynamicCapsule(PhysicsRigidDynamic* rigidDynamic, const physx::PxTransform& transform,
		                               physx::PxMaterial& material,
		                               float radius, float halfHeight, const physx::PxVec3& velocity,
		                               float density, float angularDamping) const;

		void CreateRigidStatic(PhysicsRigidStatic* rigidStatic, const physx::PxTransform& transform,
		                       const physx::PxGeometry& geometry,
		                       const EGeometryType& geometryType, physx::PxMaterial& material) const;

		static void CreatePlane();
		static void DestroyPlane();

        static void	SetRigidStaticUserData(PhysicsRigidStatic* rigidStatic, physx::PxRigidStatic* staticActor);
		static void	SetRigidDynamicUserData(PhysicsRigidDynamic* rigidDynamic, physx::PxRigidDynamic* dynamicActor);

		static physx::PxConvexMesh* ConvexMeshFromMesh(const std::vector<Model::Vertex>& vertices);
		static physx::PxTriangleMesh* TriangleMeshFromMesh(const std::vector<Model::Vertex>& vertices,
		                                                    const std::vector<unsigned int>& indices);

		static physx::PxTransform TransformFromLocationAndRotation(const LibMath::Vector3& location,
		                                                           const LibMath::Quaternion& rotation);

		static physx::PxScene* GetScene() { return physicsInstance.scene; }
		static physx::PxPhysics* GetPhysics() { return physicsInstance.physics; }
		static physx::PxCooking* GetCooking() { return physicsInstance.cooking; }

		PHYSIC_EXPORT static PhysicsInstance physicsInstance;

	private:
		friend PhysicsRigidDynamic;
		friend PhysicsRigidStatic;

		PhysicsInstance() = default;

        static void	MakeActorShapesDrivable(physx::PxRigidActor* actor);
		static void	MakeActorShapesNotDrivable(physx::PxRigidActor* actor);

		void CreateScene();

		static physx::PxFilterFlags ContactReportFilterShader(physx::PxFilterObjectAttributes /*attributes0*/,
		                                                      physx::PxFilterData /*filterData0*/,
		                                                      physx::PxFilterObjectAttributes /*attributes1*/,
		                                                      physx::PxFilterData /*filterData1*/,
		                                                      physx::PxPairFlags& pairFlags,
		                                                      const void* /*constantBlock*/,
		                                                      physx::PxU32 /*constantBlockSize*/);

		static void NotifyActiveActors();

		physx::PxFoundation* foundation = nullptr;
		physx::PxPhysics* physics = nullptr;
		physx::PxPvd* pvd = nullptr;
		physx::PxPvdTransport* transport = nullptr;
		physx::PxCooking* cooking = nullptr;
		physx::PxScene* scene = nullptr;
		physx::PxDefaultCpuDispatcher* dispatcher = nullptr;

		physx::PxRigidStatic* plane = nullptr;

		physx::PxReal objectsAverageLength = 100;
		physx::PxReal objectsAverageSpeed = 981;
		physx::PxVec3 defaultGravity{0.0f, -9.81f, 0.0f};

		int nbThreads = 2;
	};

	LibMath::Vector3 Vec3Convert(const physx::PxVec3& vec3);
	physx::PxVec3 Vec3Convert(const LibMath::Vector3& vec3);
	LibMath::Quaternion QuaternionConvert(const physx::PxQuat& quaternion);
	physx::PxQuat QuaternionConvert(const LibMath::Quaternion& quaternion);
}
