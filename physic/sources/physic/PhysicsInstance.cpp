#include "PhysicsInstance.h"

#include "PhysicsRigidActor.h"
#include <core/CLog.h>
#include <Matrix/Matrix4.h>
#include <model/Vertex.h>
#include "core/ECS/Entity.h"
#include "core/scenegraph/SceneNode.h"

#include "ErrorCallback.h"
#include "PhysicsVehicle.h"
#include "SimulationEventCallback.h"

using namespace physx;

static Physics::ErrorCallback g_DefaultErrorCallback;
static PxDefaultAllocator g_DefaultAllocatorCallback;
static Physics::SimulationEventCallback g_DefaultEventCallback;

Physics::PhysicsInstance Physics::PhysicsInstance::physicsInstance;

namespace Physics
{

	PhysicsInstance::~PhysicsInstance()
	{
		if(foundation)
		{
			LOG(LOG_INFO, "Destroying physics.", Core::ELogChannel::CLOG_PHYSICS);

			PxCloseVehicleSDK();
			pvd->disconnect();
			scene->release();
			physics->release();

			LOG(LOG_INFO, "Physics destroyed.", Core::ELogChannel::CLOG_PHYSICS);
		}
	}

	void PhysicsInstance::Init()
	{
		LOG(LOG_INFO, "Creating physics...", Core::ELogChannel::CLOG_PHYSICS);

		foundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_DefaultAllocatorCallback, g_DefaultErrorCallback);
		ASSERT(foundation != nullptr, "PxCreateFoundation failed!", Core::ELogChannel::CLOG_PHYSICS);

#ifndef _SHIPPING
		pvd = PxCreatePvd(*foundation);
		transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif

		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);
		ASSERT(physics != nullptr, "PxCreatePhysics failed!", Core::ELogChannel::CLOG_PHYSICS);
		PxTolerancesScale tolerancesScale;
		tolerancesScale.length = objectsAverageLength;
		tolerancesScale.speed = objectsAverageSpeed;

		auto cookingParams = PxCookingParams(tolerancesScale);
		cookingParams.meshWeldTolerance = 0.001f;
		cookingParams.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);

		cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, cookingParams);
		ASSERT(cooking != nullptr, "PxCreateCooking failed!", Core::ELogChannel::CLOG_PHYSICS);

		dispatcher = PxDefaultCpuDispatcherCreate(nbThreads);

		CreateScene();

		scene->setFlag(PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);

		PhysicsVehicle::InitVehicles(&g_DefaultAllocatorCallback);

		LOG(LOG_INFO, "Successfully created physics.", Core::ELogChannel::CLOG_PHYSICS);
	}

	void PhysicsInstance::Advance(const float delta)
	{
		PhysicsVehicle::UpdateVehicles(delta);

		physicsInstance.scene->simulate(delta);
		physicsInstance.scene->fetchResults(true);

		NotifyActiveActors();
	}

	void PhysicsInstance::ClearScene()
	{
		physicsInstance.scene->release();

		physicsInstance.CreateScene();
	}

	void PhysicsInstance::SetDefaultGravity(const PxVec3& gravity)
	{
		physicsInstance.defaultGravity = gravity;
		physicsInstance.scene->setGravity(gravity);
	}

	void PhysicsInstance::CreateRigidDynamic(PhysicsRigidDynamic* rigidDynamic, const PxTransform& transform,
	                                         const PxGeometry& geometry, const EGeometryType& geometryType, PxMaterial& material,
	                                         const PxVec3& velocity, float density, float angularDamping) const
	{
		PxRigidDynamic* dynamic = PxCreateDynamic(*physics, transform, geometry, material, density);
		dynamic->setAngularDamping(angularDamping);
		dynamic->setLinearVelocity({velocity.x, velocity.y, velocity.z});
		scene->addActor(*dynamic);

		rigidDynamic->rigidDynamic = dynamic;
		rigidDynamic->rigidDynamic->userData = rigidDynamic;
		rigidDynamic->geometryType = geometryType;

		rigidDynamic->AttachToEntity();
	}

	void PhysicsInstance::CreateRigidDynamicCapsule(PhysicsRigidDynamic* rigidDynamic, const PxTransform& transform,
	                                                PxMaterial& material,
	                                                const float radius, const float halfHeight,
	                                                const PxVec3& velocity, const float density,
	                                                const float angularDamping) const
	{
		PxRigidDynamic* capsuleDynamic = physics->createRigidDynamic(transform);
		const PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
		PxShape* aCapsuleShape = PxRigidActorExt::createExclusiveShape(*capsuleDynamic,
		                                                               PxCapsuleGeometry(radius, halfHeight), material);
		aCapsuleShape->setLocalPose(relativePose);
		PxRigidBodyExt::updateMassAndInertia(*capsuleDynamic, density);
		capsuleDynamic->setLinearVelocity(velocity);
		capsuleDynamic->setAngularDamping(angularDamping);

		capsuleDynamic->setMassSpaceInertiaTensor(PxVec3(0.f, 1000.f, 0.f));

		scene->addActor(*capsuleDynamic);

		rigidDynamic->rigidDynamic = capsuleDynamic;
		rigidDynamic->rigidDynamic->userData = rigidDynamic;
		rigidDynamic->geometryType = EGeometryType::CAPSULE_UPRIGHT;

		rigidDynamic->AttachToEntity();
	}

	void PhysicsInstance::CreateRigidStatic(PhysicsRigidStatic* rigidStatic, const PxTransform& transform,
	                                        const PxGeometry& geometry, const EGeometryType& geometryType,
	                                        PxMaterial& material) const
	{
		PxRigidStatic* rigidStaticPhysX = PxCreateStatic(*physics, transform, geometry, material);
		scene->addActor(*rigidStaticPhysX);

		SetRigidStaticUserData(rigidStatic, rigidStaticPhysX);
		rigidStatic->geometryType = geometryType;

		rigidStatic->AttachToEntity();
	}

    void PhysicsInstance::CreatePlane()
    {
		physicsInstance.plane = PxCreatePlane(*physicsInstance.physics, PxPlane(PxVec3(0, 1, 0).getNormalized(), 0),
		                            *physicsInstance.physics->createMaterial(0.5f, 0.5f, 0.6f));

		MakeActorShapesDrivable(physicsInstance.plane);

		physicsInstance.scene->addActor(*physicsInstance.plane);
    }

    void PhysicsInstance::DestroyPlane()
    {
		if(physicsInstance.plane != nullptr)
		{
			physicsInstance.plane->release();
			physicsInstance.plane = nullptr;
		}
    }

    void PhysicsInstance::SetRigidStaticUserData(PhysicsRigidStatic* rigidStatic, PxRigidStatic* staticActor)
    {
		rigidStatic->rigidStatic = staticActor;
		rigidStatic->rigidStatic->userData = rigidStatic;

		MakeActorShapesDrivable(staticActor);
    }

    void PhysicsInstance::SetRigidDynamicUserData(PhysicsRigidDynamic* rigidDynamic,
                                                  PxRigidDynamic* dynamicActor)
    {
		rigidDynamic->rigidDynamic = dynamicActor;
		rigidDynamic->rigidDynamic->userData = rigidDynamic;
    }

    PxFilterFlags PhysicsInstance::ContactReportFilterShader(const PxFilterObjectAttributes attributes0,
                                                             const PxFilterData filterData0,
                                                             const PxFilterObjectAttributes attributes1,
                                                             const PxFilterData filterData1, PxPairFlags& pairFlags,
                                                             const void* constantBlock, const PxU32 constantBlockSize)
	{
		PX_UNUSED(constantBlockSize);
		PX_UNUSED(constantBlock);

		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eDEFAULT;
		}

		if (
			filterData0.word0 == COLLISION_FLAG_WHEEL ||
			filterData0.word0 == COLLISION_FLAG_CHASSIS &&
			filterData0.word1 == COLLISION_FLAG_WHEEL ||
			filterData0.word1 == COLLISION_FLAG_CHASSIS && (0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)))
			return PxFilterFlag::eSUPPRESS;

		pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT
			| PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_CONTACT_POINTS
			| PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlags(static_cast<PxU16>(filterData0.word2 | filterData1.word2));
		return PxFilterFlag::eDEFAULT;
	}

	void PhysicsInstance::NotifyActiveActors()
	{
		PxU32 nbActiveActors;
		auto* const activeActors = physicsInstance.scene->getActiveActors(nbActiveActors);

		for (PxU32 i = 0; i < nbActiveActors; ++i)
		{
			auto* actor = static_cast<PhysicsRigidActor*>(activeActors[i]->userData);

			if(actor->IsCanChangeEntityTransform())
			{
				actor->UpdatePhysicsRender();
			}
		}
	}

    void PhysicsInstance::MakeActorShapesDrivable(PxRigidActor* actor)
    {
		PxShape** shapes = new PxShape*[actor->getNbShapes()];
		actor->getShapes(shapes, actor->getNbShapes());


		PxFilterData qryFilterData;
		qryFilterData.word3 = static_cast<PxU32>(DRIVABLE_SURFACE);
		//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
		const PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);

		for(PxU32 i = 0; i < actor->getNbShapes() ; i++)
		{
			shapes[i]->setQueryFilterData(qryFilterData);
			shapes[i]->setSimulationFilterData(groundPlaneSimFilterData);
		}

		delete[] shapes;
    }

    void PhysicsInstance::MakeActorShapesNotDrivable(PxRigidActor* actor)
    {
		PxShape** shapes = new PxShape * [actor->getNbShapes()];
		actor->getShapes(shapes, actor->getNbShapes());


		PxFilterData qryFilterData;
		qryFilterData.word3 = static_cast<PxU32>(UNDRIVABLE_SURFACE);
		const PxFilterData groundPlaneSimFilterData(0, 0, 0, 0);

		for (PxU32 i = 0; i < actor->getNbShapes(); i++)
		{
			shapes[i]->setQueryFilterData(qryFilterData);
			shapes[i]->setSimulationFilterData(groundPlaneSimFilterData);
		}

		delete[] shapes;
    }

    void PhysicsInstance::CreateScene()
	{
		PxSceneDesc sceneDesc(physics->getTolerancesScale());
		sceneDesc.gravity = defaultGravity;
		sceneDesc.cpuDispatcher = dispatcher;
		sceneDesc.simulationEventCallback = &g_DefaultEventCallback;
		sceneDesc.filterShader = ContactReportFilterShader;
		scene = physics->createScene(sceneDesc);

#ifdef _DEBUG

		PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
#endif

	}

	PxConvexMesh* PhysicsInstance::ConvexMeshFromMesh(const std::vector<Model::Vertex>& vertices)
	{
		const size_t verticesNumber = vertices.size();

		PxConvexMeshDesc convexDesc;
		convexDesc.points.count = static_cast<PxU32>(verticesNumber);
		convexDesc.points.stride = sizeof(Model::Vertex);
		convexDesc.points.data = &vertices[0];
		convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

		PxDefaultMemoryOutputStream buf;
		PxConvexMeshCookingResult::Enum result;

		if (!physicsInstance.cooking->cookConvexMesh(convexDesc, buf, &result))
		{
			LOG(LOG_WARNING, "Failed to create Convex Mesh from vertices.", Core::ELogChannel::CLOG_PHYSICS);
			return nullptr;
		}

		PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		PxConvexMesh* convexMesh = physicsInstance.physics->createConvexMesh(input);

		return convexMesh;
	}

	PxTriangleMesh* PhysicsInstance::TriangleMeshFromMesh(const std::vector<Model::Vertex>& vertices,
	                                                       const std::vector<unsigned int>& indices)
	{
		const size_t verticesNumber = vertices.size();
		const size_t indicesNumber = indices.size();

		const PxTolerancesScale scale;
		PxCookingParams params(scale);
		// disable mesh cleaning - perform mesh validation on development configurations
		params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
		// disable edge precompute, edges are set for each triangle, slows contact generation
		params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;

		physicsInstance.cooking->setParams(params);

		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = static_cast<PxU32>(verticesNumber);
		meshDesc.points.stride = sizeof(Model::Vertex);
		meshDesc.points.data = &vertices[0];

		meshDesc.triangles.count = static_cast<PxU32>(indicesNumber / 3);
		meshDesc.triangles.stride = 3 * sizeof(unsigned);
		meshDesc.triangles.data = &indices[0];

		if(meshDesc.isValid())
		{
			PxTriangleMesh* aTriangleMesh = physicsInstance.cooking->createTriangleMesh(meshDesc,
				physicsInstance.physics->getPhysicsInsertionCallback());

			return aTriangleMesh;
		}

		return nullptr;
	}

	PxTransform PhysicsInstance::TransformFromLocationAndRotation(const LibMath::Vector3& location,
	                                                              const LibMath::Quaternion& rotation)
	{
		return PxTransform{location.x, location.y, location.z, QuaternionConvert(rotation)};
	}

	LibMath::Vector3 Vec3Convert(const PxVec3& vec3)
	{
		return LibMath::Vector3(vec3.x, vec3.y, vec3.z);
	}

	PxVec3 Vec3Convert(const LibMath::Vector3& vec3)
	{
		return PxVec3(vec3.x, vec3.y, vec3.z);
	}

	LibMath::Quaternion QuaternionConvert(const PxQuat& quaternion)
	{
		return { quaternion.x, quaternion.y, quaternion.z, quaternion.w };
	}

	PxQuat QuaternionConvert(const LibMath::Quaternion& quaternion)
	{
		return {quaternion.X, quaternion.Y, quaternion.Z, quaternion.W};
	}
}
