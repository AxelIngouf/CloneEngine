#pragma once

#define PX_FOUNDATION_DLL 0
#include "PxPhysicsAPI.h"

namespace Physics
{
    physx::PxQueryHitType::Enum WheelSceneQueryPreFilterBlocking
	(physx::PxFilterData filterData0, physx::PxFilterData filterData1,
     const void* constantBlock, physx::PxU32 constantBlockSize,
     physx::PxHitFlags& queryFlags);


	class VehicleSceneQueryData
	{
	public:
		VehicleSceneQueryData();
		~VehicleSceneQueryData();

		//Allocate scene query data for up to maxNumVehicles and up to maxNumWheelsPerVehicle with numVehiclesInBatch per batch query.
		static VehicleSceneQueryData* Allocate
		(physx::PxU32 maxNumVehicles, physx::PxU32 maxNumWheelsPerVehicle, physx::PxU32 maxNumHitPointsPerWheel, physx::PxU32 numVehiclesInBatch,
         physx::PxBatchQueryPreFilterShader preFilterShader, physx::PxBatchQueryPostFilterShader postFilterShader,
         physx::PxAllocatorCallback& allocator);

		//Free allocated buffers.
		void Free(physx::PxAllocatorCallback& allocator);

		//Create a PxBatchQuery instance that will be used for a single specified batch.
		static physx::PxBatchQuery* SetUpBatchedSceneQuery(physx::PxU32 batchId, const VehicleSceneQueryData& vehicleSceneQueryData, physx::PxScene* scene);

		//Return an array of scene query results for a single specified batch.
        physx::PxRaycastQueryResult* GetRaycastQueryResultBuffer(physx::PxU32 batchId) const;

		//Return an array of scene query results for a single specified batch.
        physx::PxSweepQueryResult* GetSweepQueryResultBuffer(physx::PxU32 batchId) const;

		//Get the number of scene query results that have been allocated for a single batch.
        physx::PxU32 GetQueryResultBufferSize() const;

	private:

		//Number of queries per batch
        physx::PxU32 numQueriesPerBatch;

		//Number of hit results per query
        physx::PxU32 numHitResultsPerQuery;

		//One result for each wheel.
        physx::PxRaycastQueryResult* raycastResults;
        physx::PxSweepQueryResult* sweepResults{};

		//One hit for each wheel.
        physx::PxRaycastHit* raycastHitBuffer;
        physx::PxSweepHit* sweepHitBuffer{};

		//Filter shader used to filter drivable and non-drivable surfaces
        physx::PxBatchQueryPreFilterShader preFilterShader;

		//Filter shader used to reject hit shapes that initially overlap sweeps.
        physx::PxBatchQueryPostFilterShader postFilterShader;

	};
}

