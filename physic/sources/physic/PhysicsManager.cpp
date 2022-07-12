#include "PhysicsManager.h"
#include "PhysicsRigidActor.h"
#include "PhysicsInstance.h"
#include "PhysicsVehicle.h"
#include "core/ResourceManager.h"
#include "core/ECS/Entity.h"
#include "core/scenegraph/SceneNode.h"
#include "model/Model.h"

using namespace physx;

namespace Physics
{
	DebugVertex::DebugVertex(const LibMath::Vector3& pos, const uint32_t col)
		: position(pos)
	{
		switch ((PxDebugColor::Enum)col)
		{
		case PxDebugColor::eARGB_BLACK:
			color = {0.f, 0.f, 0.f};
			break;
		case PxDebugColor::eARGB_RED:
			color = {1.f, 0.f, 0.f};
			break;
		case PxDebugColor::eARGB_GREEN:
			color = {0.f, 1.f, 0.f};
			break;
		case PxDebugColor::eARGB_BLUE:
			color = {0.f, 0.f, 1.f};
			break;
		case PxDebugColor::eARGB_YELLOW:
			color = {1.f, 1.f, 0.f};
			break;
		case PxDebugColor::eARGB_MAGENTA:
			color = {1.f, 0.f, 1.f};
			break;
		case PxDebugColor::eARGB_CYAN:
			color = {0.f, 1.f, 1.f};
			break;
		case PxDebugColor::eARGB_WHITE:
			color = {1.f, 1.f, 1.f};
			break;
		case PxDebugColor::eARGB_GREY:
			color = {0.5f, 0.5f, 0.5f};
			break;
		case PxDebugColor::eARGB_DARKRED:
			color = {0.5f, 0.f, 0.f};
			break;
		case PxDebugColor::eARGB_DARKGREEN:
			color = {0.f, 0.5f, 0.f};
			break;
		case PxDebugColor::eARGB_DARKBLUE:
			color = {0.f, 0.f, 0.5f};
			break;
		}
	}

	void InitPhysics()
	{
		PhysicsInstance::physicsInstance.Init();
	}

	void Advance(const float delta)
	{
		PhysicsInstance::Advance(delta);
	}

	void CreateConvexMeshRigidDynamic(PhysicsRigidDynamic* rigidDynamic, const Core::Entity* entity,
	                                  const std::vector<Model::Vertex>& vertices, const float staticFriction,
	                                  const float dynamicFriction, const float restitution,
	                                  const LibMath::Vector3& velocity, const float density, const float angularDamping)
	{
		if (!entity || !entity->IsInUse())
		{
			LOG(LOG_WARNING, "Tried to create physics component on invalid entity.", Core::ELogChannel::CLOG_PHYSICS);
			return;
		}

		PxConvexMesh* convexMesh = PhysicsInstance::ConvexMeshFromMesh(vertices);

		if (convexMesh == nullptr)
			return;

		PxMaterial* material = PhysicsInstance::GetPhysics()->createMaterial(
			staticFriction, dynamicFriction, restitution);

		const auto& transform = entity->GetAnchor()->GetWorldTransformNoCheck();

		PhysicsInstance::physicsInstance.CreateRigidDynamic(rigidDynamic,
		                                                    PhysicsInstance::TransformFromLocationAndRotation(
			                                                    transform.position, transform.rotation),
		                                                    PxConvexMeshGeometry(
			                                                    convexMesh, PxMeshScale(), PxConvexMeshGeometryFlags()),
		                                                    EGeometryType::CONVEX_MESH, *material,
		                                                    Vec3Convert(velocity), density, angularDamping);
	}

	void CreateSphereRigidDynamic(PhysicsRigidDynamic* rigidDynamic, const Core::Entity* entity, const float radius,
	                              const float staticFriction, const float dynamicFriction, const float restitution,
	                              const LibMath::Vector3& velocity, const float density, const float angularDamping)
	{
		if (!entity || !entity->IsInUse())
		{
			LOG(LOG_WARNING, "Tried to create physics component on invalid entity.", Core::ELogChannel::CLOG_PHYSICS);
			return;
		}

		PxMaterial* material = PhysicsInstance::GetPhysics()->createMaterial(
			staticFriction, dynamicFriction, restitution);

		const auto& transform = entity->GetAnchor()->GetWorldTransformNoCheck();

		PhysicsInstance::physicsInstance.CreateRigidDynamic(rigidDynamic,
		                                                    PhysicsInstance::TransformFromLocationAndRotation(
			                                                    transform.position, transform.rotation),
		                                                    PxSphereGeometry(radius),
		                                                    EGeometryType::SPHERE, *material, Vec3Convert(velocity),
		                                                    density,
		                                                    angularDamping);
	}

	void CreateCapsuleRigidDynamic(PhysicsRigidDynamic* rigidDynamic, const Core::Entity* entity,
	                               const float radius, const float halfHeight, const float staticFriction,
	                               const float dynamicFriction, const float restitution,
	                               const LibMath::Vector3& velocity,
	                               const float density, const float angularDamping)
	{
		if (!entity || !entity->IsInUse())
		{
			LOG(LOG_WARNING, "Tried to create physics component on invalid entity.", Core::ELogChannel::CLOG_PHYSICS);
			return;
		}

		PxMaterial* material = PhysicsInstance::GetPhysics()->createMaterial(
			staticFriction, dynamicFriction, restitution);

		const auto& transform = entity->GetAnchor()->GetWorldTransformNoCheck();

		PhysicsInstance::physicsInstance.CreateRigidDynamicCapsule(rigidDynamic,
		                                                           PhysicsInstance::TransformFromLocationAndRotation(
			                                                           transform.position, transform.rotation),
		                                                           *material, radius, halfHeight,
		                                                           Vec3Convert(velocity), density, angularDamping);
	}

	void CreateBoxRigidDynamic(PhysicsRigidDynamic* rigidDynamic, const Core::Entity* entity,
	                           const LibMath::Vector3& halfExtents, const float staticFriction,
	                           const float dynamicFriction, const float restitution,
	                           const LibMath::Vector3& velocity, const float density, const float angularDamping)
	{
		if (!entity || !entity->IsInUse())
		{
			LOG(LOG_WARNING, "Tried to create physics component on invalid entity.", Core::ELogChannel::CLOG_PHYSICS);
			return;
		}

		PxMaterial* material = PhysicsInstance::GetPhysics()->createMaterial(
			staticFriction, dynamicFriction, restitution);

		const auto& transform = entity->GetAnchor()->GetWorldTransformNoCheck();

		PhysicsInstance::physicsInstance.CreateRigidDynamic(rigidDynamic,
		                                                    PhysicsInstance::TransformFromLocationAndRotation(
			                                                    transform.position, transform.rotation),
		                                                    PxBoxGeometry(Vec3Convert(halfExtents)), EGeometryType::BOX,
		                                                    *material,
		                                                    Vec3Convert(velocity), density, angularDamping);
	}

	void CreateConvexMeshRigidStatic(PhysicsRigidStatic* rigidStatic, const Core::Entity* entity,
	                                 const std::vector<Model::Vertex>& vertices, float staticFriction,
	                                 float dynamicFriction, float restitution)
	{
		if (!entity || !entity->IsInUse())
		{
			LOG(LOG_WARNING, "Tried to create physics component on invalid entity.", Core::ELogChannel::CLOG_PHYSICS);
			return;
		}

		PxConvexMesh* convexMesh = PhysicsInstance::ConvexMeshFromMesh(vertices);

		if (convexMesh == nullptr)
			return;

		PxMaterial* material = PhysicsInstance::GetPhysics()->createMaterial(
			staticFriction, dynamicFriction, restitution);

		const auto& transform = entity->GetAnchor()->GetWorldTransformNoCheck();

		PhysicsInstance::physicsInstance.CreateRigidStatic(rigidStatic,
		                                                   PhysicsInstance::TransformFromLocationAndRotation(
			                                                   transform.position, transform.rotation),
		                                                   PxConvexMeshGeometry(convexMesh),
		                                                   EGeometryType::CONVEX_MESH, *material);
	}

	void CreateTriangleMeshRigidStatic(PhysicsRigidStatic* rigidStatic, const Core::Entity* entity,
	                                   const std::vector<Model::Vertex>& vertices,
	                                   const std::vector<unsigned>& indices, float staticFriction,
	                                   float dynamicFriction, float restitution)
	{
		if (!entity || !entity->IsInUse())
		{
			LOG(LOG_WARNING, "Tried to create physics component on invalid entity.", Core::ELogChannel::CLOG_PHYSICS);
			return;
		}

		PxTriangleMesh* triangleMesh = PhysicsInstance::TriangleMeshFromMesh(vertices, indices);

		if (triangleMesh == nullptr)
			return;

		PxMaterial* material = PhysicsInstance::GetPhysics()->createMaterial(
			staticFriction, dynamicFriction, restitution);

		const auto& transform = entity->GetAnchor()->GetWorldTransformNoCheck();

		PhysicsInstance::physicsInstance.CreateRigidStatic(rigidStatic,
		                                                   PhysicsInstance::TransformFromLocationAndRotation(
			                                                   transform.position, transform.rotation),
		                                                   PxTriangleMeshGeometry(triangleMesh, PxMeshScale()),
		                                                   EGeometryType::TRIANGLE_MESH, *material);
	}

	void CreateConvexMeshRigidDynamicFromModel(PhysicsRigidDynamic* rigidDynamic, const Core::Entity* entity,
	                                           const std::string& modelPath, float staticFriction,
	                                           float dynamicFriction, float restitution,
	                                           const LibMath::Vector3& velocity, float angularDamping)
	{
		auto* model = ResourceManager::GetResource<Model::Model>(modelPath);

		if (model)
		{
			const auto& transform = entity->GetAnchor()->GetWorldTransformNoCheck();
			PxRigidDynamic* modelStatic = PhysicsInstance::GetPhysics()->createRigidDynamic(
				PhysicsInstance::TransformFromLocationAndRotation(transform.position, transform.rotation));

			PxMaterial* material = PhysicsInstance::GetPhysics()->createMaterial(
				staticFriction, dynamicFriction, restitution);

			for (const Model::Mesh& mesh : model->meshes)
			{
				auto* convex = PhysicsInstance::ConvexMeshFromMesh(mesh.vertices);

				if (convex)
				{
					auto geometry = PxConvexMeshGeometry(convex);

					PxRigidActorExt::createExclusiveShape(*modelStatic, geometry, *material);
				}
			}
			PhysicsInstance::GetScene()->addActor(*modelStatic);

			PhysicsInstance::SetRigidDynamicUserData(rigidDynamic, modelStatic);
			rigidDynamic->AttachToEntity();

			rigidDynamic->SetGeometryType(EGeometryType::CONVEX_MESH);
			rigidDynamic->SetLinearVelocity(velocity);
			rigidDynamic->SetAngularDamping(angularDamping);
		}
		else
		{
			LOG(LOG_ERROR, "Could not find model to create a convex mesh.", Core::ELogChannel::CLOG_PHYSICS);
		}
	}

	void CreateConvexMeshRigidStaticFromModel(PhysicsRigidStatic* rigidStatic, const Core::Entity* entity,
	                                          const std::string& modelPath, float staticFriction, float dynamicFriction,
	                                          float restitution)
	{
		auto* model = ResourceManager::GetResource<Model::Model>(modelPath);

		if (model)
		{
			const auto& transform = entity->GetAnchor()->GetWorldTransformNoCheck();
			PxRigidStatic* modelStatic = PhysicsInstance::GetPhysics()->createRigidStatic(
				PhysicsInstance::TransformFromLocationAndRotation(transform.position, transform.rotation));

			PxMaterial* material = PhysicsInstance::GetPhysics()->createMaterial(
				staticFriction, dynamicFriction, restitution);

			for (const Model::Mesh& mesh : model->meshes)
			{
				auto* convex = PhysicsInstance::ConvexMeshFromMesh(mesh.vertices);

				if (convex)
				{
					auto geometry = PxConvexMeshGeometry(convex);

					PxRigidActorExt::createExclusiveShape(*modelStatic, geometry, *material);
				}
			}
			PhysicsInstance::GetScene()->addActor(*modelStatic);

			PhysicsInstance::SetRigidStaticUserData(rigidStatic, modelStatic);

			rigidStatic->AttachToEntity();

			rigidStatic->SetGeometryType(EGeometryType::CONVEX_MESH);
		}
		else
		{
			LOG(LOG_ERROR, "Could not find model to create a convex mesh.", Core::ELogChannel::CLOG_PHYSICS);
		}
	}

	void CreateTriangleMeshRigidStaticFromModel(PhysicsRigidStatic* rigidStatic, const Core::Entity* entity,
	                                            const std::string& modelPath, float staticFriction,
	                                            float dynamicFriction,
	                                            float restitution)
	{
		auto* model = ResourceManager::GetResource<Model::Model>(modelPath);

		if (model)
		{
			const auto& transform = entity->GetAnchor()->GetWorldTransformNoCheck();
			PxRigidStatic* modelStatic = PhysicsInstance::GetPhysics()->createRigidStatic(
				PhysicsInstance::TransformFromLocationAndRotation(transform.position, transform.rotation));

			PxMaterial* material = PhysicsInstance::GetPhysics()->createMaterial(
				staticFriction, dynamicFriction, restitution);

			for (const Model::Mesh& mesh : model->meshes)
			{
				auto* triangle = PhysicsInstance::TriangleMeshFromMesh(mesh.vertices, mesh.indices);

				if (triangle)
				{
					auto geometry = PxTriangleMeshGeometry(triangle);

					PxRigidActorExt::createExclusiveShape(*modelStatic, geometry, *material);
				}
			}
			PhysicsInstance::GetScene()->addActor(*modelStatic);

			PhysicsInstance::SetRigidStaticUserData(rigidStatic, modelStatic);
			rigidStatic->SetGeometryType(EGeometryType::TRIANGLE_MESH);

			rigidStatic->AttachToEntity();
		}
		else
		{
			LOG(LOG_ERROR, "Could not find model to create a triangle mesh.", Core::ELogChannel::CLOG_PHYSICS);
		}
	}

	void CreateSphereRigidStatic(PhysicsRigidStatic* rigidStatic, const Core::Entity* entity, float radius,
	                             float staticFriction, float dynamicFriction, float restitution)
	{
		if (!entity || !entity->IsInUse())
		{
			LOG(LOG_WARNING, "Tried to create physics component on invalid entity.", Core::ELogChannel::CLOG_PHYSICS);
			return;
		}

		PxMaterial* material = PhysicsInstance::GetPhysics()->createMaterial(
			staticFriction, dynamicFriction, restitution);

		const auto& transform = entity->GetAnchor()->GetWorldTransformNoCheck();

		PhysicsInstance::physicsInstance.CreateRigidStatic(rigidStatic,
		                                                   PhysicsInstance::TransformFromLocationAndRotation(
			                                                   transform.position, transform.rotation),
		                                                   PxSphereGeometry(radius),
		                                                   EGeometryType::SPHERE, *material);
	}

	void CreateCapsuleRigidStatic(PhysicsRigidStatic* rigidStatic, const Core::Entity* entity, float radius,
	                              float halfHeight, float staticFriction, float dynamicFriction, float restitution)
	{
		if (!entity || !entity->IsInUse())
		{
			LOG(LOG_WARNING, "Tried to create physics component on invalid entity.", Core::ELogChannel::CLOG_PHYSICS);
			return;
		}

		PxMaterial* material = PhysicsInstance::GetPhysics()->createMaterial(
			staticFriction, dynamicFriction, restitution);

		const auto& transform = entity->GetAnchor()->GetWorldTransformNoCheck();

		PhysicsInstance::physicsInstance.CreateRigidStatic(rigidStatic,
		                                                   PhysicsInstance::TransformFromLocationAndRotation(
			                                                   transform.position, transform.rotation),
		                                                   PxCapsuleGeometry(radius, halfHeight),
		                                                   EGeometryType::CAPSULE, *material);
	}

	void CreateBoxRigidStatic(PhysicsRigidStatic* rigidStatic, const Core::Entity* entity,
	                          const LibMath::Vector3& extents, float staticFriction, float dynamicFriction,
	                          float restitution)
	{
		if (!entity || !entity->IsInUse())
		{
			LOG(LOG_WARNING, "Tried to create physics component on invalid entity.", Core::ELogChannel::CLOG_PHYSICS);
			return;
		}

		PxMaterial* material = PhysicsInstance::GetPhysics()->createMaterial(
			staticFriction, dynamicFriction, restitution);

		const auto& transform = entity->GetAnchor()->GetWorldTransformNoCheck();

		PhysicsInstance::physicsInstance.CreateRigidStatic(rigidStatic,
		                                                   PhysicsInstance::TransformFromLocationAndRotation(
			                                                   transform.position, transform.rotation),
		                                                   PxBoxGeometry(Vec3Convert(extents)), EGeometryType::BOX,
		                                                   *material);
	}

	void CreateVehicle(PhysicsVehicleActor* vehicleActor, const LibMath::Vector3& location,
	                   const LibMath::Quaternion& rotation, float wheelsMass, float wheelsRadius, float wheelWidth,
	                   float chassisMass, const LibMath::Vector3& chassisDimensions, float chassisStaticFriction,
	                   float chassisDynamicFriction, float chassisRestitution, float wheelStaticFriction,
	                   float wheelDynamicFriction, float wheelRestitution)
	{
		PxMaterial* chassisMaterial = PxGetPhysics().createMaterial(chassisStaticFriction, chassisDynamicFriction,
		                                                            chassisRestitution);
		PxMaterial* wheelMaterial = PxGetPhysics().createMaterial(wheelStaticFriction, wheelDynamicFriction,
		                                                          wheelRestitution);
		const VehicleDesc vehicleDesc(chassisMaterial, wheelMaterial,
		                              wheelsMass, wheelsRadius, wheelWidth, chassisMass,
		                              Vec3Convert(chassisDimensions));


		PhysicsVehicle::CreateVehicle(vehicleActor, vehicleDesc, Vec3Convert(location), QuaternionConvert(rotation));
	}

	void CreatePlane()
	{
		PhysicsInstance::CreatePlane();
	}

	void DestroyPlane()
	{
		PhysicsInstance::DestroyPlane();
	}

	LibMath::Vector3 GetSceneGravity()
	{
		return Vec3Convert(PhysicsInstance::GetScene()->getGravity());
	}

	void EnableDebugVisualization(const bool debug)
	{
		if (debug)
		{
			PhysicsInstance::GetScene()->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			PhysicsInstance::GetScene()->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_EDGES, 2.0f);
			PhysicsInstance::GetScene()->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 2.0f);
			PhysicsInstance::GetScene()->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.0f);
		}
		else
		{
			PhysicsInstance::GetScene()->setVisualizationParameter(PxVisualizationParameter::eSCALE, 0.f);
			PhysicsInstance::GetScene()->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_EDGES, 0.f);
			PhysicsInstance::GetScene()->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 0.f);
			PhysicsInstance::GetScene()->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 0.f);
		}
	}

	void GetDebugLines(std::vector<DebugVertex>& debugVertices)
	{
		const PxRenderBuffer& rb = PhysicsInstance::GetScene()->getRenderBuffer();

		debugVertices.reserve(rb.getNbLines());

		for (PxU32 iLine = 0; iLine < rb.getNbLines(); iLine++)
		{
			const PxDebugLine& line = rb.getLines()[iLine];

			debugVertices.emplace_back(Vec3Convert(line.pos0), line.color0);
			debugVertices.emplace_back(Vec3Convert(line.pos1), line.color1);
		}
	}

	bool Raycast(const LibMath::Vector3& origin, const LibMath::Vector3& direction, float maxDistance, RaycastHit& hit)
	{
		// todo test this function

		PxRaycastBuffer raycastBuffer;
		const bool isHit = PhysicsInstance::GetScene()->raycast(Vec3Convert(origin), Vec3Convert(direction),
		                                                        maxDistance, raycastBuffer);

		if (!isHit)
			return false;

		hit.distance = raycastBuffer.block.distance;
		hit.normal = Vec3Convert(raycastBuffer.block.normal);
		hit.position = Vec3Convert(raycastBuffer.block.position);
		hit.hitEntity = Core::Entity::GetEntity(
			static_cast<PhysicsRigidActor*>(raycastBuffer.block.actor->userData)->GetPhysicsEntityHandle());

		return true;
	}

	void CleanScene()
	{
		PhysicsInstance::ClearScene();
	}

	void SetSceneGravity(const LibMath::Vector3& gravity)
	{
		PhysicsInstance::SetDefaultGravity(Vec3Convert(gravity));
	}
}
