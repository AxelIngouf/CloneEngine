#pragma once

#define PX_FOUNDATION_DLL 0
#include <vector>

#include "PxPhysicsAPI.h"

namespace LibMath {
    struct Quaternion;
    struct Vector3;
}

namespace Physics
{
    class PhysicsVehicleActor;

    struct ActorUserData
    {
        ActorUserData()
            : vehicle(nullptr),
            actor(nullptr)
        {
        }

        const physx::PxVehicleWheels* vehicle;
        const physx::PxActor* actor;
    };
    struct ShapeUserData
    {
        ShapeUserData()
            : isWheel(false),
            wheelId(0xffffffff)
        {
        }

        bool    isWheel;
        int     wheelId;
    };

    enum EDrivableSurfaceType
    {
        SURFACE_TYPE_TARMAC,
        MAX_NUM_SURFACE_TYPES
    };

    enum EVehicleCollisionType
    {
        COLLISION_FLAG_GROUND = 1 << 4,
        COLLISION_FLAG_WHEEL = 1 << 5,
        COLLISION_FLAG_CHASSIS = 1 << 6,
        COLLISION_FLAG_OBSTACLE = 1 << 7,
        COLLISION_FLAG_DRIVABLE_OBSTACLE = 1 << 8,

        COLLISION_FLAG_GROUND_AGAINST = COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
        COLLISION_FLAG_WHEEL_AGAINST = COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
        COLLISION_FLAG_CHASSIS_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
        COLLISION_FLAG_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
        COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE
    };

    struct VehicleDesc
    {
        VehicleDesc()
            : chassisMass(0.0f),
            chassisDims(physx::PxVec3(0.0f, 0.0f, 0.0f)),
            chassisMOI(physx::PxVec3(0.0f, 0.0f, 0.0f)),
            chassisCMOffset(physx::PxVec3(0.0f, 0.0f, 0.0f)),
            chassisMaterial(nullptr),
            wheelMass(0.0f),
            wheelWidth(0.0f),
            wheelRadius(0.0f),
            wheelMOI(0.0f),
            wheelMaterial(nullptr),
            actorUserData(nullptr),
            shapeUserDatas(nullptr)
        {
        }

        VehicleDesc(physx::PxMaterial* chassMaterial, physx::PxMaterial* whMaterial,
            float whMass = 20.0f, float whRadius = 0.25f, float whWidth = 0.3f, float chassMass = 1000.f,
            const physx::PxVec3& chassDimensions = { 1.5f,0.5f,5.0f })
                : chassisMass(chassMass), chassisDims(chassDimensions),
        chassisMaterial(chassMaterial), wheelMass(whMass), wheelWidth(whWidth), wheelRadius(whRadius), wheelMaterial(whMaterial),
            actorUserData(nullptr),
            shapeUserDatas(nullptr)
        {
            wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;

            chassisCMOffset = { 0.f, -chassisDims.y * 0.5f + 0.65f, 0.25f };

            chassisMOI = { (chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
                (chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
                (chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f };

            chassisSimFilterData = physx::PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);
            wheelSimFilterData = physx::PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);
        }

        float chassisMass;
        physx::PxVec3 chassisDims;
        physx::PxVec3 chassisMOI;
        physx::PxVec3 chassisCMOffset;
        physx::PxMaterial* chassisMaterial;
        physx::PxFilterData chassisSimFilterData;  //word0 = collide type, word1 = collide against types, word2 = PxPairFlags

        float wheelMass;
        float wheelWidth;
        float wheelRadius;
        float wheelMOI;
        physx::PxMaterial* wheelMaterial;
        unsigned numWheels=4;
        physx::PxFilterData wheelSimFilterData;	//word0 = collide type, word1 = collide against types, word2 = PxPairFlags

        ActorUserData* actorUserData;
        ShapeUserData* shapeUserDatas;
    };

    enum
    {
        DRIVABLE_SURFACE = 0xffff0000,
        UNDRIVABLE_SURFACE = 0x0000ffff
    };

    enum
    {
        TIRE_TYPE_NORMAL = 0,
        TIRE_TYPE_WORN,
        MAX_NUM_TIRE_TYPES
    };

    class PhysicsVehicle
    {
    public:
        static void InitVehicles(physx::PxDefaultAllocator* defaultAllocator);

        static void CreateVehicle(PhysicsVehicleActor* vehicleActor, const VehicleDesc& vehicle4WDesc, const physx::PxVec3& location, const physx::PxQuat& rotation);

        static void	UpdateVehicles(float delta);

        static void ReleaseVehicle(int vehicleId);

    private:

        static physx::PxVehicleDrivableSurfaceToTireFrictionPairs*  CreateFrictionPairs(const physx::PxMaterial* defaultMaterial);

        static physx::PxRigidDynamic* CreateVehicleActor(const physx::PxVehicleChassisData& chassisData,
            physx::PxMaterial** wheelMaterials,
            physx::PxConvexMesh** wheelConvexMeshes, int numWheels,
            const physx::PxFilterData& wheelSimFilterData,
            physx::PxMaterial** chassisMaterials,
            physx::PxConvexMesh** chassisConvexMeshes, int numChassisMeshes,
            const physx::PxFilterData& chassisSimFilterData);

        static void SetupNonDrivableSurface(physx::PxFilterData& pxFilterData);

        static physx::PxConvexMesh* CreateConvexMesh(const physx::PxVec3* vertices, int verticesCount);

        static physx::PxConvexMesh* CreateWheelMesh(float width, float radius);

        static physx::PxConvexMesh* CreateChassisMesh(const physx::PxVec3& dims);

        static void ConfigureUserData(physx::PxVehicleWheels* vehicle, ActorUserData* actorUserData, ShapeUserData* shapeUserData);

        static void ComputeWheelCenterActorOffsets4W(float wheelFrontZ, float wheelRearZ, const physx::PxVec3& chassisDims, float wheelWidth, float wheelRadius, int numWheels, physx::PxVec3* wheelCentreOffsets);

        static void SetupWheelsSimulationData(float wheelMass, float wheelMOI, float wheelRadius, float wheelWidth, int numWheels, const physx::PxVec3* wheelCenterActorOffsets, const physx::PxVec3& chassisCMOffset, float chassisMass, physx::PxVehicleWheelsSimData* wheelsSimData);

        static std::vector<physx::PxVehicleWheels*> registeredVehicles;

        static int createdVehiclesCount;
    };
}