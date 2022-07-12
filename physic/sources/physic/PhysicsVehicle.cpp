#include "PhysicsVehicle.h"

#include <vehicle/PxVehicleDrive4W.h>
#include <vehicle/PxVehicleUtilSetup.h>

#include "PhysicsRigidActor.h"

#include "PhysicsInstance.h"
#include "VehicleSceneQuerry.h"
#include "core/CLog.h"
#include "core/ECS/Entity.h"
#include "core/scenegraph/SceneNode.h"

using namespace physx;

namespace Physics
{
    class VehicleConcurrency;
    class VehicleWheelQueryResults;

    constexpr auto NUM_VEHICLES = 1024;

	PxBatchQuery* gBatchQuery = nullptr;

	PxVehicleWheels* gVehicles[NUM_VEHICLES];
	VehicleSceneQueryData* gVehicleSceneQueryData = nullptr;


	PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = nullptr;
	static PxF32 gTireFrictionMultipliers[MAX_NUM_SURFACE_TYPES][MAX_NUM_TIRE_TYPES] =
	{
		//NORMAL,	WORN
		{1.00f,		0.1f}//TARMAC
	};


	VehicleWheelQueryResults* gVehicleWheelQueryResults = nullptr;
	VehicleConcurrency* gVehicleConcurrency = nullptr;
	PxTaskManager* gTaskManager = nullptr;

	/* Vehicle controller globals */
	PxVehicleKeySmoothingData gDefaultKeySmoothing =
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
			6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
			2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
			2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
			10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
			5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
			5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};

	PxF32 gSteerVsForwardSpeedData[2 * 8] =
	{
		0.0f,		0.75f,
		5.0f,		0.75f,
		30.0f,		0.125f,
		120.0f,		0.1f,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32
	};
	PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

	/* All registered vehicles */
	std::vector<PxVehicleWheels*> PhysicsVehicle::registeredVehicles;
	int PhysicsVehicle::createdVehiclesCount = 0;

    void PhysicsVehicle::InitVehicles(PxDefaultAllocator* defaultAllocator)
    {
		PxInitVehicleSDK(*PhysicsInstance::GetPhysics());
		PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
		PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

		gVehicleSceneQueryData = VehicleSceneQueryData::Allocate(NUM_VEHICLES, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, *defaultAllocator);

		gBatchQuery = VehicleSceneQueryData::SetUpBatchedSceneQuery(0, *gVehicleSceneQueryData, PhysicsInstance::GetScene());

		gFrictionPairs = CreateFrictionPairs(PxGetPhysics().createMaterial(0.5f, 0.5f, 0.6f));
    }

	void PhysicsVehicle::CreateVehicle(PhysicsVehicleActor* vehicleActor, const VehicleDesc& vehicle4WDesc, const PxVec3& location, const PxQuat& rotation)
	{
		const PxVec3 chassisDims = vehicle4WDesc.chassisDims;
		const PxF32 wheelWidth = vehicle4WDesc.wheelWidth;
		const PxF32 wheelRadius = vehicle4WDesc.wheelRadius;
		const PxU32 numWheels = vehicle4WDesc.numWheels;

		const PxFilterData& chassisSimFilterData = vehicle4WDesc.chassisSimFilterData;
		const PxFilterData& wheelSimFilterData = vehicle4WDesc.wheelSimFilterData;

		//Construct a physx actor with shapes for the chassis and wheels.
		//Set the rigid body mass, moment of inertia, and center of mass offset.
		PxRigidDynamic* veh4WActor = nullptr;
		{
			//Construct a convex mesh for a cylindrical wheel.
			PxConvexMesh* wheelMesh = CreateWheelMesh(wheelWidth, wheelRadius);
			//Assume all wheels are identical for simplicity.
			PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
			PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

			//Set the meshes and materials for the driven wheels.
			for (PxU32 i = PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
			{
				wheelConvexMeshes[i] = wheelMesh;
				wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
			}
			//Set the meshes and materials for the non-driven wheels
			for (PxU32 i = PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < numWheels; i++)
			{
				wheelConvexMeshes[i] = wheelMesh;
				wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
			}

			//Chassis just has a single convex shape for simplicity.
			PxConvexMesh* chassisConvexMesh = CreateChassisMesh(chassisDims);
			PxConvexMesh* chassisConvexMeshes[1] = { chassisConvexMesh };
			PxMaterial* chassisMaterials[1] = { vehicle4WDesc.chassisMaterial };

			//Rigid body data.
			PxVehicleChassisData rigidBodyData;
			rigidBodyData.mMOI = vehicle4WDesc.chassisMOI;
			rigidBodyData.mMass = vehicle4WDesc.chassisMass;
			rigidBodyData.mCMOffset = vehicle4WDesc.chassisCMOffset;

			veh4WActor = CreateVehicleActor(rigidBodyData, wheelMaterials, wheelConvexMeshes, numWheels,
				wheelSimFilterData, chassisMaterials, chassisConvexMeshes, 1,
				chassisSimFilterData);
		}

		//Set up the sim data for the wheels.
		PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(numWheels);
		{
			//Compute the wheel center offsets from the origin.
			PxVec3 wheelCenterActorOffsets[PX_MAX_NB_WHEELS];
			const PxF32 frontZ = chassisDims.z * 0.3f;
			const PxF32 rearZ = -chassisDims.z * 0.3f;
			ComputeWheelCenterActorOffsets4W(frontZ, rearZ, chassisDims, wheelWidth, wheelRadius, numWheels,
				wheelCenterActorOffsets);

			//Set up the simulation data for all wheels.
			SetupWheelsSimulationData(vehicle4WDesc.wheelMass, vehicle4WDesc.wheelMOI, wheelRadius, wheelWidth,
				numWheels, wheelCenterActorOffsets, vehicle4WDesc.chassisCMOffset,
				vehicle4WDesc.chassisMass, wheelsSimData);
		}

		//Set up the sim data for the vehicle drive model.
		PxVehicleDriveSimData4W driveSimData;
		{
			//Diff
			PxVehicleDifferential4WData diff;
			diff.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
			driveSimData.setDiffData(diff);

			//Engine
			PxVehicleEngineData engine;
			engine.mPeakTorque = 500.0f;
			engine.mMaxOmega = 600.0f;//approx 6000 rpm
			driveSimData.setEngineData(engine);

			//Gears
			PxVehicleGearsData gears;
			gears.mSwitchTime = 0.5f;
			driveSimData.setGearsData(gears);

			//Clutch
			PxVehicleClutchData clutch;
			clutch.mStrength = 10.0f;
			driveSimData.setClutchData(clutch);

			//Ackermann steer accuracy
			PxVehicleAckermannGeometryData ackermann;
			ackermann.mAccuracy = 1.0f;
			ackermann.mAxleSeparation =
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z -
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).z;
			ackermann.mFrontWidth =
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x -
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
			ackermann.mRearWidth =
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
			driveSimData.setAckermannGeometryData(ackermann);
		}

		//Create a vehicle from the wheels and drive sim data.
		PxVehicleDrive4W* vehDrive4W = PxVehicleDrive4W::allocate(numWheels);
		vehDrive4W->setup(PhysicsInstance::GetPhysics(), veh4WActor, *wheelsSimData, driveSimData, numWheels - 4);

		//Configure the userdata
		ConfigureUserData(vehDrive4W, vehicle4WDesc.actorUserData, vehicle4WDesc.shapeUserDatas);

		//Free the sim data because we don't need that any more.
		wheelsSimData->free();

		vehDrive4W->getRigidDynamicActor()->setGlobalPose(PxTransform{ location, rotation });

		PhysicsInstance::GetScene()->addActor(*vehDrive4W->getRigidDynamicActor());

		vehicleActor->vehicle = vehDrive4W;
		vehicleActor->wheelsCount = vehicle4WDesc.numWheels;
		vehicleActor->SetVehicleId(createdVehiclesCount++);
		vehicleActor->geometryType = EGeometryType::VEHICLE;

		Core::Entity::GetEntity(vehicleActor->handle)->GetAnchor()->onCleaned.ClearDelegates();
		Core::Entity::GetEntity(vehicleActor->handle)->GetAnchor()->onCleaned.Add(&PhysicsRigidDynamic::UpdateTransform, vehicleActor);

		vehDrive4W->getRigidDynamicActor()->userData = vehicleActor;


		//vehDrive4W->setToRestState();
		vehDrive4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		vehDrive4W->mDriveDynData.setUseAutoGears(true);
		vehDrive4W->mWheelsDynData.setToRestState();
		vehDrive4W->mDriveDynData.setGearSwitchTime(0.001f);

		registeredVehicles.emplace_back(vehDrive4W);
	}

	PxRigidDynamic* PhysicsVehicle::CreateVehicleActor(const PxVehicleChassisData& chassisData,
                                                       PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, int numWheels,
                                                       const PxFilterData& wheelSimFilterData, PxMaterial** chassisMaterials,
                                                       PxConvexMesh** chassisConvexMeshes, int numChassisMeshes,
                                                       const PxFilterData& chassisSimFilterData)
    {
        PxRigidDynamic* vehActor = PhysicsInstance::GetPhysics()->createRigidDynamic(PxTransform(PxIdentity));

        PxFilterData wheelQryFilterData;
        SetupNonDrivableSurface(wheelQryFilterData);
        PxFilterData chassisQryFilterData;
        SetupNonDrivableSurface(chassisQryFilterData);

        //Add all the wheel shapes to the actor.
        for (int i = 0; i < numWheels; i++)
        {
            PxConvexMeshGeometry geom(wheelConvexMeshes[i]);
            PxShape* wheelShape = PxRigidActorExt::createExclusiveShape(*vehActor, geom, *wheelMaterials[i]);
            wheelShape->setQueryFilterData(wheelQryFilterData);
            wheelShape->setSimulationFilterData(wheelSimFilterData);
            wheelShape->setLocalPose(PxTransform(PxIdentity));
        }

        //Add the chassis shapes to the actor.
        for (int i = 0; i < numChassisMeshes; i++)
        {
            PxShape* chassisShape = PxRigidActorExt::createExclusiveShape(*vehActor, PxConvexMeshGeometry(chassisConvexMeshes[i]), *chassisMaterials[i]);
            chassisShape->setQueryFilterData(chassisQryFilterData);
            chassisShape->setSimulationFilterData(chassisSimFilterData);
            chassisShape->setLocalPose(PxTransform(PxIdentity));
        }

        vehActor->setMass(chassisData.mMass);
        vehActor->setMassSpaceInertiaTensor(chassisData.mMOI);
        vehActor->setCMassLocalPose(PxTransform(chassisData.mCMOffset, PxQuat(PxIdentity)));

        return vehActor;
    }

	class VehicleConcurrency
	{
	public:

		VehicleConcurrency()
			: maxNumVehicles(0),
			maxNumWheelsPerVehicle(0),
			vehicleConcurrentUpdates(nullptr)
		{
		}

		~VehicleConcurrency() = default;

		static VehicleConcurrency* Allocate(const PxU32 maxNumVehicles, const PxU32 maxNumWheelsPerVehicle, PxAllocatorCallback& allocator)
		{
			const PxU32 byteSize =
				sizeof(VehicleConcurrency) +
				sizeof(PxVehicleConcurrentUpdateData) * maxNumVehicles +
				sizeof(PxVehicleWheelConcurrentUpdateData) * maxNumWheelsPerVehicle * maxNumVehicles;

			PxU8* buffer = static_cast<PxU8*>(allocator.allocate(byteSize, nullptr, nullptr, 0));

			auto* vc = reinterpret_cast<VehicleConcurrency*>(buffer);
			new(vc) VehicleConcurrency();
			buffer += sizeof(VehicleConcurrency);

			vc->maxNumVehicles = maxNumVehicles;
			vc->maxNumWheelsPerVehicle = maxNumWheelsPerVehicle;

			vc->vehicleConcurrentUpdates = reinterpret_cast<PxVehicleConcurrentUpdateData*>(buffer);
			buffer += sizeof(PxVehicleConcurrentUpdateData) * maxNumVehicles;

			for (PxU32 i = 0; i < maxNumVehicles; i++)
			{
				new(vc->vehicleConcurrentUpdates + i) PxVehicleConcurrentUpdateData();

				vc->vehicleConcurrentUpdates[i].nbConcurrentWheelUpdates = maxNumWheelsPerVehicle;

				vc->vehicleConcurrentUpdates[i].concurrentWheelUpdates = reinterpret_cast<PxVehicleWheelConcurrentUpdateData*>(buffer);
				buffer += sizeof(PxVehicleWheelConcurrentUpdateData) * maxNumWheelsPerVehicle;

				for (PxU32 j = 0; j < maxNumWheelsPerVehicle; j++)
				{
					new(vc->vehicleConcurrentUpdates[i].concurrentWheelUpdates + j) PxVehicleWheelConcurrentUpdateData();
				}

			}


			return vc;
		}

		//Free allocated buffer for scene queries of suspension raycasts.
		void Free(PxAllocatorCallback& allocator)
		{
			allocator.deallocate(this);
		}

		//Return the PxVehicleConcurrentUpdate for a vehicle specified by an index.
        [[nodiscard]] PxVehicleConcurrentUpdateData* GetVehicleConcurrentUpdate(const PxU32 id) const
        {
			return vehicleConcurrentUpdates + id;
		}

		//Return the entire array of PxVehicleConcurrentUpdates
        [[nodiscard]] PxVehicleConcurrentUpdateData* GetVehicleConcurrentUpdateBuffer() const
        {
			return vehicleConcurrentUpdates;
		}

	private:

		PxU32 maxNumVehicles;
		PxU32 maxNumWheelsPerVehicle;
		PxVehicleConcurrentUpdateData* vehicleConcurrentUpdates;
	};
	class VehicleWheelQueryResults
	{
	public:

		VehicleWheelQueryResults()
			: vehicleWheelQueryResults(nullptr)
		{
		}

		~VehicleWheelQueryResults() = default;

		//Allocate wheel results for up to maxNumVehicles with up to maxNumWheelsPerVehicle.
		static VehicleWheelQueryResults* Allocate(const PxU32 maxNumVehicles, const PxU32 maxNumWheelsPerVehicle, PxAllocatorCallback& allocator)
		{
			const PxU32 byteSize = sizeof(VehicleWheelQueryResults) + sizeof(PxVehicleWheelQueryResult) * maxNumVehicles + sizeof(PxWheelQueryResult) * maxNumWheelsPerVehicle * maxNumVehicles;

			PxU8* buffer = static_cast<PxU8*>(allocator.allocate(byteSize, nullptr, nullptr, 0));

			auto* queryResults = reinterpret_cast<VehicleWheelQueryResults*>(buffer);
			buffer += sizeof(VehicleWheelQueryResults);

			queryResults->vehicleWheelQueryResults = reinterpret_cast<PxVehicleWheelQueryResult*>(buffer);
			buffer += sizeof(PxVehicleWheelQueryResult) * maxNumVehicles;

			for (PxU32 i = 0; i < maxNumVehicles; i++)
			{
				new(buffer) PxWheelQueryResult();
				queryResults->vehicleWheelQueryResults[i].wheelQueryResults = reinterpret_cast<PxWheelQueryResult*>(buffer);
				queryResults->vehicleWheelQueryResults[i].nbWheelQueryResults = maxNumWheelsPerVehicle;
				buffer += sizeof(PxWheelQueryResult) * maxNumWheelsPerVehicle;
			}

			return queryResults;
		}

		//Free allocated buffer for scene queries of suspension raycasts.
		void Free(PxAllocatorCallback& allocator)
		{
			allocator.deallocate(this);
		}

		//Return the PxVehicleWheelQueryResult for a vehicle specified by an index.
        [[nodiscard]] PxVehicleWheelQueryResult* GetVehicleWheelQueryResults(const PxU32 id) const
        {
			return vehicleWheelQueryResults + id;
		}

	private:

		PxVehicleWheelQueryResult* vehicleWheelQueryResults;
	};

   bool PxVehicleIsInAir(const PxVehicleWheelQueryResult& vehWheelQueryResults)
   {
	   if (!vehWheelQueryResults.wheelQueryResults)
	   {
		   return true;
	   }

	   for (PxU32 i = 0; i < vehWheelQueryResults.nbWheelQueryResults; i++)
	   {
		   if (!vehWheelQueryResults.wheelQueryResults[i].isInAir)
		   {
			   return false;
		   }
	   }
	   return true;
   }

   void PhysicsVehicle::UpdateVehicles(const float delta)
   {
	   if(!registeredVehicles.empty())
	   {
		   for (const auto& registeredVehicle : registeredVehicles)
           {
			   auto* vehicleActor = static_cast<PhysicsVehicleActor*>(registeredVehicle->getRigidDynamicActor()->userData);

			   PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gDefaultKeySmoothing, gSteerVsForwardSpeedTable, *vehicleActor->inputData, delta, vehicleActor->IsInTheAir(), *vehicleActor->vehicle);

		   }

		   PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->GetRaycastQueryResultBuffer(0);
		   const PxU32 raycastResultsSize = gVehicleSceneQueryData->GetQueryResultBufferSize();
		   PxVehicleSuspensionRaycasts(gBatchQuery, static_cast<int>(registeredVehicles.size()), &registeredVehicles[0], raycastResultsSize, raycastResults);

		   auto* wheelQueryResults = new PxWheelQueryResult * [registeredVehicles.size()];
		   auto* vehicleQueryResults = new PxVehicleWheelQueryResult[registeredVehicles.size()];

		   for (size_t i = 0; i < registeredVehicles.size(); i++)
		   {
			   wheelQueryResults[i] = new PxWheelQueryResult[registeredVehicles[i]->mWheelsSimData.getNbWheels()];

			   vehicleQueryResults[i] = { wheelQueryResults[i], registeredVehicles[i]->mWheelsSimData.getNbWheels()};

			   auto* vehicleActor = static_cast<PhysicsVehicleActor*>(registeredVehicles[i]->getRigidDynamicActor()->userData);

			   vehicleActor->SetIsInTheAir(registeredVehicles[i]->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]));
		   }

		   //Vehicle update.
		   const PxVec3 gravity = PhysicsInstance::GetScene()->getGravity();
		   PxVehicleUpdates(delta, gravity, *gFrictionPairs, static_cast<int>(registeredVehicles.size()), &registeredVehicles[0], vehicleQueryResults);

		   delete[] vehicleQueryResults;

		   for(size_t i = 0; i < registeredVehicles.size(); i++)
		   {
			   delete[] wheelQueryResults[i];
		   }
		   delete[] wheelQueryResults;
	   }
   }

    void PhysicsVehicle::ReleaseVehicle(const int vehicleId)
    {
	   for(auto it = registeredVehicles.begin(); it != registeredVehicles.end(); ++it)
	   {
		   auto* vehicle = static_cast<PhysicsVehicleActor*>((*it)->getRigidDynamicActor()->userData);
	       if(vehicle->GetVehicleId() == vehicleId)
	       {
			   registeredVehicles.erase(it);
			   return;
	       }
	   }
    }

    PxVehicleDrivableSurfaceToTireFrictionPairs* PhysicsVehicle::CreateFrictionPairs(const PxMaterial* defaultMaterial)
    {
		PxVehicleDrivableSurfaceType surfaceTypes[1];
		surfaceTypes[0].mType = SURFACE_TYPE_TARMAC;

		const PxMaterial* surfaceMaterials[1];
		surfaceMaterials[0] = defaultMaterial;

		PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs =
			PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES);

		surfaceTirePairs->setup(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES, surfaceMaterials, surfaceTypes);

		for (PxU32 i = 0; i < MAX_NUM_SURFACE_TYPES; i++)
		{
			for (PxU32 j = 0; j < MAX_NUM_TIRE_TYPES; j++)
			{
				surfaceTirePairs->setTypePairFriction(i, j, gTireFrictionMultipliers[i][j]);
			}
		}

		return surfaceTirePairs;
    }

    void PhysicsVehicle::SetupNonDrivableSurface(PxFilterData& pxFilterData)
    {
        pxFilterData.word3 = UNDRIVABLE_SURFACE;
    }

    PxConvexMesh* PhysicsVehicle::CreateWheelMesh(const float width, const float radius)
    {
		PxVec3 points[2 * 16];
		for (PxU32 i = 0; i < 16; i++)
		{
			const PxF32 cosTheta = PxCos(i * PxPi * 2.0f / 16.0f);
			const PxF32 sinTheta = PxSin(i * PxPi * 2.0f / 16.0f);
			const PxF32 y = radius * cosTheta;
			const PxF32 z = radius * sinTheta;
			points[2 * i + 0] = PxVec3(-width / 2.0f, y, z);
			points[2 * i + 1] = PxVec3(+width / 2.0f, y, z);
		}

		return CreateConvexMesh(points, 32);
    }

    PxConvexMesh* PhysicsVehicle::CreateChassisMesh(const PxVec3& dims)
    {
		const PxF32 x = dims.x * 0.5f;
		const PxF32 y = dims.y * 0.5f;
		const PxF32 z = dims.z * 0.5f;
		PxVec3 verts[8] =
		{
			PxVec3(x,y,-z),
			PxVec3(x,y,z),
			PxVec3(x,-y,z),
			PxVec3(x,-y,-z),
			PxVec3(-x,y,-z),
			PxVec3(-x,y,z),
			PxVec3(-x,-y,z),
			PxVec3(-x,-y,-z)
		};

		return CreateConvexMesh(verts, 8);
    }

    void PhysicsVehicle::ConfigureUserData(PxVehicleWheels* vehicle, ActorUserData* actorUserData,
                                           ShapeUserData* shapeUserData)
    {
		if (actorUserData)
		{
			vehicle->getRigidDynamicActor()->userData = actorUserData;
			actorUserData->vehicle = vehicle;
		}

		if (shapeUserData)
		{
			PxShape* shapes[PX_MAX_NB_WHEELS + 1];
			vehicle->getRigidDynamicActor()->getShapes(shapes, PX_MAX_NB_WHEELS + 1);
			for (PxU32 i = 0; i < vehicle->mWheelsSimData.getNbWheels(); i++)
			{
				const PxI32 shapeId = vehicle->mWheelsSimData.getWheelShapeMapping(i);
				shapes[shapeId]->userData = &shapeUserData[i];
				shapeUserData[i].isWheel = true;
				shapeUserData[i].wheelId = i;
			}
		}
    }

    void PhysicsVehicle::ComputeWheelCenterActorOffsets4W(const float wheelFrontZ, const float wheelRearZ,
        const PxVec3& chassisDims, const float wheelWidth, const float wheelRadius, const int numWheels,
        PxVec3* wheelCentreOffsets)
    {
		const PxF32 numLeftWheels = static_cast<float>(numWheels) / 2.0f;
		const PxF32 deltaZ = (wheelFrontZ - wheelRearZ) / (numLeftWheels - 1.0f);
		//Set the outside of the left and right wheels to be flush with the chassis.
		//Set the top of the wheel to be just touching the underside of the chassis.
		//Begin by setting the rear-left/rear-right/front-left,front-right wheels.
		wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT] = PxVec3((-chassisDims.x + wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + 0 * deltaZ * 0.5f);
		wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = PxVec3((+chassisDims.x - wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + 0 * deltaZ * 0.5f);
		wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = PxVec3((-chassisDims.x + wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + (numLeftWheels - 1) * deltaZ);
		wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = PxVec3((+chassisDims.x - wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + (numLeftWheels - 1) * deltaZ);
		//Set the remaining wheels.
		for (int i = 2, wheelCount = 4; i < numWheels - 2; i += 2, wheelCount += 2)
		{
			wheelCentreOffsets[wheelCount + 0] = PxVec3((-chassisDims.x + wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + static_cast<float>(i) * deltaZ * 0.5f);
			wheelCentreOffsets[wheelCount + 1] = PxVec3((+chassisDims.x - wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + static_cast<float>(i) * deltaZ * 0.5f);
		}
    }

    void PhysicsVehicle::SetupWheelsSimulationData(const float wheelMass, const float wheelMOI, const float wheelRadius,
        const float wheelWidth, const int numWheels, const PxVec3* wheelCenterActorOffsets,
        const PxVec3& chassisCMOffset, const float chassisMass, PxVehicleWheelsSimData* wheelsSimData)
    {
		PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
		{
			//Set up the wheel data structures with mass, moi, radius, width.
			for (int i = 0; i < numWheels; i++)
			{
				wheels[i].mMass = wheelMass;
				wheels[i].mMOI = wheelMOI;
				wheels[i].mRadius = wheelRadius;
				wheels[i].mWidth = wheelWidth;
			}

			//Enable the handbrake for the rear wheels only.
			wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 4000.0f;
			wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 4000.0f;
			//Enable steering for the front wheels only.
			wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = PxPi * 0.3333f;
			wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = PxPi * 0.3333f;
		}

		//Set up the tires.
		PxVehicleTireData tires[PX_MAX_NB_WHEELS];
		{
			//Set up the tires.
			for (int i = 0; i < numWheels; i++)
			{
				tires[i].mType = TIRE_TYPE_NORMAL;
			}
		}

		//Set up the suspensions
		PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
		{
			//Compute the mass supported by each suspension spring.
			float suspensionSprungMasses[PX_MAX_NB_WHEELS];
			PxVehicleComputeSprungMasses
			(numWheels, wheelCenterActorOffsets,
				chassisCMOffset, chassisMass, 1, suspensionSprungMasses);

			//Set the suspension data.
			for (int i = 0; i < numWheels; i++)
			{
				suspensions[i].mMaxCompression = 0.3f;
				suspensions[i].mMaxDroop = 0.1f;
				suspensions[i].mSpringStrength = 35000.0f;
				suspensions[i].mSpringDamperRate = 4500.0f;
				suspensions[i].mSprungMass = suspensionSprungMasses[i];
			}

			//Set the camber angles.
			const PxF32 camberAngleAtRest = 0.0;
			const PxF32 camberAngleAtMaxDroop = 0.01f;
			const PxF32 camberAngleAtMaxCompression = -0.01f;
			for (int i = 0; i < numWheels; i += 2)
			{
				suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
				suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
				suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
				suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
				suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
				suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
			}
		}

		//Set up the wheel geometry.
		PxVec3 suspensionTravelDirections[PX_MAX_NB_WHEELS];
		PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
		PxVec3 suspensionForceAppCMOffsets[PX_MAX_NB_WHEELS];
		PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
		{
			//Set the geometry data.
			for (int i = 0; i < numWheels; i++)
			{
				//Vertical suspension travel.
				suspensionTravelDirections[i] = PxVec3(0, -1, 0);

				//Wheel center offset is offset from rigid body center of mass.
				wheelCentreCMOffsets[i] =
					wheelCenterActorOffsets[i] - chassisCMOffset;

				//Suspension force application point 0.3 meters below 
				//rigid body center of mass.
				suspensionForceAppCMOffsets[i] =
					PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);

				//Tire force application point 0.3 meters below 
				//rigid body center of mass.
				tireForceAppCMOffsets[i] =
					PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
			}
		}

		//Set up the filter data of the raycast that will be issued by each suspension.
		PxFilterData qryFilterData;
		SetupNonDrivableSurface(qryFilterData);

		//Set the wheel, tire and suspension data.
		//Set the geometry data.
		//Set the query filter data
		for (int i = 0; i < numWheels; i++)
		{
			wheelsSimData->setWheelData(i, wheels[i]);
			wheelsSimData->setTireData(i, tires[i]);
			wheelsSimData->setSuspensionData(i, suspensions[i]);
			wheelsSimData->setSuspTravelDirection(i, suspensionTravelDirections[i]);
			wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
			wheelsSimData->setSuspForceAppPointOffset(i, suspensionForceAppCMOffsets[i]);
			wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
			wheelsSimData->setSceneQueryFilterData(i, qryFilterData);
			wheelsSimData->setWheelShapeMapping(i, static_cast<PxI32>(i));
		}

		//Add a front and rear anti-roll bar
		PxVehicleAntiRollBarData barFront;
		barFront.mWheel0 = PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
		barFront.mWheel1 = PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
		barFront.mStiffness = 10000.0f;
		wheelsSimData->addAntiRollBarData(barFront);
		PxVehicleAntiRollBarData barRear;
		barRear.mWheel0 = PxVehicleDrive4WWheelOrder::eREAR_LEFT;
		barRear.mWheel1 = PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
		barRear.mStiffness = 10000.0f;
		wheelsSimData->addAntiRollBarData(barRear);
    }

    PxConvexMesh* PhysicsVehicle::CreateConvexMesh(const PxVec3* vertices, const int verticesCount)
    {
		PxConvexMeshDesc convexDesc;
		convexDesc.points.count = verticesCount;
		convexDesc.points.stride = sizeof(PxVec3);
		convexDesc.points.data = vertices;
		convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

		PxConvexMesh* convexMesh = nullptr;
		PxDefaultMemoryOutputStream buf;
		if (PhysicsInstance::GetCooking()->cookConvexMesh(convexDesc, buf))
		{
			PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
			convexMesh = PhysicsInstance::GetPhysics()->createConvexMesh(id);
		}

		return convexMesh;
    }
}

