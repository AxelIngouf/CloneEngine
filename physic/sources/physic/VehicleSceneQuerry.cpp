#include "VehicleSceneQuerry.h"

#include "PhysicsVehicle.h"

using namespace physx;

namespace Physics
{
    PxQueryHitType::Enum WheelSceneQueryPreFilterBlocking(PxFilterData filterData0, PxFilterData filterData1,
        const void* constantBlock, PxU32 constantBlockSize, PxHitFlags& queryFlags)
    {
        PX_UNUSED(filterData0);
        PX_UNUSED(constantBlock);
        PX_UNUSED(constantBlockSize);
        PX_UNUSED(queryFlags);
        return 0 == (filterData1.word3 & DRIVABLE_SURFACE) ? PxQueryHitType::eNONE : PxQueryHitType::eBLOCK;
    }

    VehicleSceneQueryData::VehicleSceneQueryData()
        : numQueriesPerBatch(0),
        numHitResultsPerQuery(0),
        raycastResults(nullptr),
        raycastHitBuffer(nullptr),
        preFilterShader(nullptr),
        postFilterShader(nullptr)
    {
    }

    VehicleSceneQueryData::~VehicleSceneQueryData() = default;

    VehicleSceneQueryData* VehicleSceneQueryData::Allocate(PxU32 maxNumVehicles, PxU32 maxNumWheelsPerVehicle,
        PxU32 maxNumHitPointsPerWheel, PxU32 numVehiclesInBatch, PxBatchQueryPreFilterShader preFilterShader,
        PxBatchQueryPostFilterShader postFilterShader, PxAllocatorCallback& allocator)
    {
        const PxU32 sqDataSize = sizeof(VehicleSceneQueryData) + 15 & ~15;

        const PxU32 maxNumWheels = maxNumVehicles * maxNumWheelsPerVehicle;
        const PxU32 raycastResultSize = sizeof(PxRaycastQueryResult) * maxNumWheels + 15 & ~15;
        const PxU32 sweepResultSize = sizeof(PxSweepQueryResult) * maxNumWheels + 15 & ~15;

        const PxU32 maxNumHitPoints = maxNumWheels * maxNumHitPointsPerWheel;
        const PxU32 raycastHitSize = sizeof(PxRaycastHit) * maxNumHitPoints + 15 & ~15;
        const PxU32 sweepHitSize = sizeof(PxSweepHit) * maxNumHitPoints + 15 & ~15;

        const PxU32 size = sqDataSize + raycastResultSize + raycastHitSize + sweepResultSize + sweepHitSize;
        PxU8* buffer = static_cast<PxU8*>(allocator.allocate(size, nullptr, nullptr, 0));

        auto* queryData = new(buffer) VehicleSceneQueryData();
        queryData->numQueriesPerBatch = numVehiclesInBatch * maxNumWheelsPerVehicle;
        queryData->numHitResultsPerQuery = maxNumHitPointsPerWheel;
        buffer += sqDataSize;

        queryData->raycastResults = reinterpret_cast<PxRaycastQueryResult*>(buffer);
        buffer += raycastResultSize;

        queryData->raycastHitBuffer = reinterpret_cast<PxRaycastHit*>(buffer);
        buffer += raycastHitSize;

        queryData->sweepResults = reinterpret_cast<PxSweepQueryResult*>(buffer);
        buffer += sweepResultSize;

        queryData->sweepHitBuffer = reinterpret_cast<PxSweepHit*>(buffer);

        for (PxU32 i = 0; i < maxNumWheels; i++)
        {
            new(queryData->raycastResults + i) PxRaycastQueryResult();
            new(queryData->sweepResults + i) PxSweepQueryResult();
        }

        for (PxU32 i = 0; i < maxNumHitPoints; i++)
        {
            new(queryData->raycastHitBuffer + i) PxRaycastHit();
            new(queryData->sweepHitBuffer + i) PxSweepHit();
        }

        queryData->preFilterShader = preFilterShader;
        queryData->postFilterShader = postFilterShader;

        return queryData;
    }

    void VehicleSceneQueryData::Free(PxAllocatorCallback& allocator)
    {
        allocator.deallocate(this);
    }

    PxBatchQuery* VehicleSceneQueryData::SetUpBatchedSceneQuery(PxU32 batchId,
        const VehicleSceneQueryData& vehicleSceneQueryData, PxScene* scene)
    {
        const PxU32 maxNumQueriesInBatch = vehicleSceneQueryData.numQueriesPerBatch;
        const PxU32 maxNumHitResultsInBatch = vehicleSceneQueryData.numQueriesPerBatch * vehicleSceneQueryData.numHitResultsPerQuery;

        PxBatchQueryDesc sqDesc(maxNumQueriesInBatch, maxNumQueriesInBatch, 0);

        sqDesc.queryMemory.userRaycastResultBuffer = vehicleSceneQueryData.raycastResults + batchId * maxNumQueriesInBatch;
        sqDesc.queryMemory.userRaycastTouchBuffer = vehicleSceneQueryData.raycastHitBuffer + batchId * maxNumHitResultsInBatch;
        sqDesc.queryMemory.raycastTouchBufferSize = maxNumHitResultsInBatch;

        sqDesc.queryMemory.userSweepResultBuffer = vehicleSceneQueryData.sweepResults + batchId * maxNumQueriesInBatch;
        sqDesc.queryMemory.userSweepTouchBuffer = vehicleSceneQueryData.sweepHitBuffer + batchId * maxNumHitResultsInBatch;
        sqDesc.queryMemory.sweepTouchBufferSize = maxNumHitResultsInBatch;

        sqDesc.preFilterShader = vehicleSceneQueryData.preFilterShader;

        sqDesc.postFilterShader = vehicleSceneQueryData.postFilterShader;

        return scene->createBatchQuery(sqDesc);
    }

    PxRaycastQueryResult* VehicleSceneQueryData::GetRaycastQueryResultBuffer(PxU32 batchId) const
    {
        return raycastResults + batchId * numQueriesPerBatch;
    }

    PxSweepQueryResult* VehicleSceneQueryData::GetSweepQueryResultBuffer(PxU32 batchId) const
    {
        return sweepResults + batchId * numQueriesPerBatch;
    }

    PxU32 VehicleSceneQueryData::GetQueryResultBufferSize() const
    {
        return numQueriesPerBatch;
    }
}
