#pragma once
#include <string>
#include <vector>

#include "Vector/Vector3.h"
#include "Quaternion/Quaternion.h"

namespace Core
{
	class Entity;
}

namespace Model
{
	struct Vertex;
}

namespace Physics
{
	class PhysicsVehicleActor;
	class PhysicsRigidActor;
	class PhysicsRigidDynamic;
	class PhysicsRigidStatic;

	struct DebugVertex
	{
		DebugVertex() = default;

		DebugVertex(const LibMath::Vector3& pos, uint32_t col);

		LibMath::Vector3 position;
		LibMath::Vector3 color;
	};

	struct RaycastHit
	{
		const Core::Entity* hitEntity = nullptr;
		LibMath::Vector3 normal;
		LibMath::Vector3 position;
		float distance;
	};

	void InitPhysics();

	void Advance(float delta);

	void CreateConvexMeshRigidDynamic(PhysicsRigidDynamic* rigidDynamic, const Core::Entity*,
	                                  const std::vector<Model::Vertex>& vertices, float staticFriction = .5f,
	                                  float dynamicFriction = .5f, float restitution = .1f,
	                                  const LibMath::Vector3& velocity = LibMath::Vector3(0), float density = 10.f,
	                                  float angularDamping = 0.5f);

	void CreateSphereRigidDynamic(PhysicsRigidDynamic* rigidDynamic, const Core::Entity*, float radius,
	                              float staticFriction = .5f, float dynamicFriction = .5f, float restitution = .1f,
	                              const LibMath::Vector3& velocity = LibMath::Vector3(0), float density = 10.f,
	                              float angularDamping = 0.5f);

	void CreateCapsuleRigidDynamic(PhysicsRigidDynamic* rigidDynamic, const Core::Entity*, float radius,
	                               float halfHeight,
	                               float staticFriction = .5f, float dynamicFriction = .5f, float restitution = .1f,
	                               const LibMath::Vector3& velocity = LibMath::Vector3(0), float density = 10.f,
	                               float angularDamping = 0.5f);

	void CreateBoxRigidDynamic(PhysicsRigidDynamic* rigidDynamic, const Core::Entity*,
	                           const LibMath::Vector3& halfExtents,
	                           float staticFriction = .5f, float dynamicFriction = .5f, float restitution = .1f,
	                           const LibMath::Vector3& velocity = LibMath::Vector3(0), float density = 10.f,
	                           float angularDamping = 0.5f);

	void CreateConvexMeshRigidStatic(PhysicsRigidStatic* rigidStatic, const Core::Entity*,
	                                 const std::vector<Model::Vertex>& vertices, float staticFriction = .5f,
	                                 float dynamicFriction = .5f, float restitution = .1f);

	void CreateTriangleMeshRigidStatic(PhysicsRigidStatic* rigidStatic, const Core::Entity*,
	                                   const std::vector<Model::Vertex>& vertices,
	                                   const std::vector<unsigned int>& indices, float staticFriction = .5f,
	                                   float dynamicFriction = .5f, float restitution = .1f);

	void CreateConvexMeshRigidDynamicFromModel(PhysicsRigidDynamic* rigidDynamic, const Core::Entity* entity,
	                                           const std::string& modelPath, float staticFriction = 0.5f,
	                                           float dynamicFriction = 0.5f, float restitution = 0.6f,
	                                           const LibMath::Vector3& velocity = LibMath::Vector3(0),
	                                           float angularDamping = 0.5f);

	void CreateConvexMeshRigidStaticFromModel(PhysicsRigidStatic* rigidStatic, const Core::Entity*,
	                                          const std::string& modelPath, float staticFriction = .5f,
	                                          float dynamicFriction = .5f, float restitution = .1f);

	void CreateTriangleMeshRigidStaticFromModel(PhysicsRigidStatic* rigidStatic, const Core::Entity*,
	                                            const std::string& modelPath, float staticFriction = .5f,
	                                            float dynamicFriction = .5f, float restitution = .1f);

	void CreateSphereRigidStatic(PhysicsRigidStatic* rigidStatic, const Core::Entity*, float radius,
	                             float staticFriction = .5f, float dynamicFriction = .5f, float restitution = .1f);

	void CreateCapsuleRigidStatic(PhysicsRigidStatic* rigidStatic, const Core::Entity*, float radius,
	                              float halfHeight, float staticFriction = .5f, float dynamicFriction = .5f,
	                              float restitution = .1f);

	void CreateBoxRigidStatic(PhysicsRigidStatic* rigidStatic, const Core::Entity*, const LibMath::Vector3& extents,
	                          float staticFriction = .5f, float dynamicFriction = .5f, float restitution = .1f);

	void CreateVehicle(PhysicsVehicleActor* vehicleActor, const LibMath::Vector3& location = {0, 0, 0},
                       const LibMath::Quaternion& rotation = {0, 0, 0, 1}, float wheelsMass = 20.f,
                       float wheelsRadius = 0.25f, float wheelWidth = 0.3f, float chassisMass = 1000.f,
                       const LibMath::Vector3& chassisDimensions = {1.5f, 0.5f, 5.0f},
                       float chassisStaticFriction = .5f, float chassisDynamicFriction = .5f,
                       float chassisRestitution = .6f, float wheelStaticFriction = .5f,
                       float wheelDynamicFriction = .5f, float wheelRestitution = .6f);


	void CreatePlane();
	void DestroyPlane();

	void SetSceneGravity(const LibMath::Vector3& gravity);
	LibMath::Vector3 GetSceneGravity();

	void EnableDebugVisualization(bool debug);
	void GetDebugLines(std::vector<DebugVertex>& debugVertices);

	bool Raycast(const LibMath::Vector3& origin, const LibMath::Vector3& direction, float maxDistance, RaycastHit& hit);

	void CleanScene();
}
