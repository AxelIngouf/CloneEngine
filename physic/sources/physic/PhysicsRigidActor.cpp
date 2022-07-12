#include "PhysicsRigidActor.h"

#include <iostream>

#include "core/ECS/Entity.h"
#include "core/scenegraph/SceneNode.h"
#include "PhysicsInstance.h"
#include "PxPhysicsAPI.h"
#include "core/PoolAllocator.h"
#include "Quaternion/Quaternion.h"

namespace Physics
{
    void PhysicsRigidActor::SetPhysicsEntityHandle(const Core::EntityHandle otherHandle)
    {
        handle = otherHandle;
    }

    PhysicsRigidStatic::PhysicsRigidStatic(PhysicsRigidStatic&& other) noexcept :
        PhysicsRigidActor(other),
        rigidStatic(other.rigidStatic)
    {
        AttachToEntity();
        other.rigidStatic = nullptr;
        rigidStatic->userData = this;
    }

    void PhysicsRigidStatic::AttachToEntity()
    {
        Core::Entity::GetEntity(handle)->GetAnchor()->onCleaned.ClearDelegates();
        Core::Entity::GetEntity(handle)->GetAnchor()->onCleaned.Add(&PhysicsRigidStatic::UpdateTransform, this);
    }

    void PhysicsRigidStatic::UpdateWorldLocation()
    {
        const auto& newLocation = Core::Entity::GetEntity(handle)->GetAnchor()->GetWorldTransformNoCheck().position + localLocation;

        rigidStatic->setGlobalPose(physx::PxTransform(newLocation.x, newLocation.y, newLocation.z, rigidStatic->getGlobalPose().q));
    }

    void PhysicsRigidStatic::UpdateWorldRotation()
    {
        const auto& newRotation = Core::Entity::GetEntity(handle)->GetAnchor()->GetWorldTransformNoCheck().rotation * localRotation;

        rigidStatic->setGlobalPose(physx::PxTransform(rigidStatic->getGlobalPose().p, 
            physx::PxQuat(newRotation.X, newRotation.Y, newRotation.Z, newRotation.W)));
    }

    LibMath::Vector3 PhysicsRigidStatic::GetWorldLocation() const
    {
        return Vec3Convert(rigidStatic->getGlobalPose().p);
    }

    LibMath::Quaternion PhysicsRigidStatic::GetWorldRotation() const
    {
        return QuaternionConvert(rigidStatic->getGlobalPose().q);
    }

    void PhysicsRigidStatic::GetWorldTransform(LibMath::Vector3& location, LibMath::Quaternion& rotation) const
    {
        location = Vec3Convert(rigidStatic->getGlobalPose().p);
        rotation = QuaternionConvert(rigidStatic->getGlobalPose().q);
    }

    void PhysicsRigidStatic::SetIsTriggerShape(const bool isTrigger)
    {
        physx::PxShape* shape;
        rigidStatic->getShapes(&shape, 1);
        shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
        shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, isTrigger);

        isTriggerShape = isTrigger;

        if(!isTrigger)
        {
            PhysicsInstance::MakeActorShapesDrivable(rigidStatic);
        }
        else
        {
            PhysicsInstance::MakeActorShapesNotDrivable(rigidStatic);
        }
    }

    void PhysicsRigidStatic::Release() const
    {
        if(rigidStatic)
            rigidStatic->release();
    }

    PhysicsRigidDynamic::PhysicsRigidDynamic(PhysicsRigidDynamic&& other) noexcept :
        PhysicsRigidActor(other),
        rigidDynamic(other.rigidDynamic)
    {
        AttachToEntity();
        other.rigidDynamic = nullptr;
        rigidDynamic->userData = this;
    }

    void PhysicsRigidDynamic::AttachToEntity()
    {
        Core::Entity::GetEntity(handle)->GetAnchor()->onCleaned.ClearDelegates();
        Core::Entity::GetEntity(handle)->GetAnchor()->onCleaned.Add(&PhysicsRigidStatic::UpdateTransform, this);
    }

    void PhysicsRigidDynamic::UpdatePhysicsRender()
    {
        auto* anchor = Core::Entity::GetEntity(handle)->GetAnchor();

        anchor->SetWorldPosition(Vec3Convert(rigidDynamic->getGlobalPose().p) - localLocation);
        anchor->SetWorldRotation(QuaternionConvert(rigidDynamic->getGlobalPose().q));
    }

    void PhysicsRigidDynamic::UpdateWorldLocation()
    {
        const auto& newLocation = Core::Entity::GetEntity(handle)->GetAnchor()->GetWorldTransformNoCheck().position + localLocation;

        rigidDynamic->setGlobalPose(physx::PxTransform(newLocation.x, newLocation.y, newLocation.z, rigidDynamic->getGlobalPose().q));
    }

    void PhysicsRigidDynamic::UpdateWorldRotation()
    {
        const auto& newRotation = Core::Entity::GetEntity(handle)->GetAnchor()->GetWorldTransformNoCheck().rotation * localRotation;

        rigidDynamic->setGlobalPose(physx::PxTransform(rigidDynamic->getGlobalPose().p,
            physx::PxQuat(newRotation.X, newRotation.Y, newRotation.Z, newRotation.W)));
    }

    LibMath::Vector3 PhysicsRigidDynamic::GetWorldLocation() const
    {
        return Vec3Convert(rigidDynamic->getGlobalPose().p);
    }

    LibMath::Quaternion PhysicsRigidDynamic::GetWorldRotation() const
    {
        return QuaternionConvert(rigidDynamic->getGlobalPose().q);
    }

    void PhysicsRigidDynamic::GetWorldTransform(LibMath::Vector3& location, LibMath::Quaternion& rotation) const
    {
        location = Vec3Convert(rigidDynamic->getGlobalPose().p);
        rotation = QuaternionConvert(rigidDynamic->getGlobalPose().q);
    }

    void PhysicsRigidDynamic::SetMass(const float mass) const
    {
        rigidDynamic->setMass(mass);
    }

    void PhysicsRigidDynamic::SetKinematic(const bool isKinematic) const
    {
        rigidDynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, isKinematic);
        if(!isKinematic)
            rigidDynamic->wakeUp();
    }

    void PhysicsRigidDynamic::SetLinearDamping(const float angularDamping) const
    {
        rigidDynamic->setLinearDamping(angularDamping);
    }

    void PhysicsRigidDynamic::SetAngularDamping(const float angularDamping) const
    {
        rigidDynamic->setAngularDamping(angularDamping);
    }

    void PhysicsRigidDynamic::SetLinearVelocity(const LibMath::Vector3& velocity) const
    {
        rigidDynamic->setLinearVelocity({ velocity.x, velocity.y, velocity.z });
    }

    void PhysicsRigidDynamic::SetAngularVelocity(const LibMath::Vector3& velocity) const
    {
        rigidDynamic->setAngularVelocity({ velocity.x, velocity.y, velocity.z });
    }

    void PhysicsRigidDynamic::SetMassSpaceInertiaTensor(const LibMath::Vector3& tensor) const
    {
        rigidDynamic->setMassSpaceInertiaTensor({ tensor.x, tensor.y, tensor.z });
    }

    float PhysicsRigidDynamic::GetRigidMass() const
    {
        return rigidDynamic->getMass();
    }

    bool PhysicsRigidDynamic::IsKinematic() const
    {
        return rigidDynamic->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC);
    }

    float PhysicsRigidDynamic::GetRigidAngularDamping() const
    {
        return rigidDynamic->getAngularDamping();
    }

    float PhysicsRigidDynamic::GetRigidLinearDamping() const
    {
        return rigidDynamic->getLinearDamping();
    }

    LibMath::Vector3 PhysicsRigidDynamic::GetRigidLinearVelocity() const
    {
        return Vec3Convert(rigidDynamic->getLinearVelocity());
    }

    LibMath::Vector3 PhysicsRigidDynamic::GetRigidAngularVelocity() const
    {
        return Vec3Convert(rigidDynamic->getAngularVelocity());
    }

    void PhysicsRigidDynamic::AddForce(const LibMath::Vector3& force) const
    {
        rigidDynamic->addForce({ force.x, force.y, force.z }, physx::PxForceMode::eFORCE);
    }

    void PhysicsRigidDynamic::AddImpulse(const LibMath::Vector3& impulse) const
    {
        rigidDynamic->addForce({ impulse.x, impulse.y, impulse.z }, physx::PxForceMode::eIMPULSE);
    }

    void PhysicsRigidDynamic::AddAcceleration(const LibMath::Vector3& acceleration) const
    {
        rigidDynamic->addForce({ acceleration.x, acceleration.y, acceleration.z }, physx::PxForceMode::eACCELERATION);
    }

    void PhysicsRigidDynamic::AddForceAtLocation(const LibMath::Vector3& force, const LibMath::Vector3& location) const
    {
        physx::PxRigidBodyExt::addForceAtPos(*rigidDynamic, { force.x, force.y, force.z }, { location.x, location.y, location.z });
    }

    void PhysicsRigidDynamic::AddForceAtLocalLocation(const LibMath::Vector3& force, const LibMath::Vector3& location) const
    {
        physx::PxRigidBodyExt::addForceAtLocalPos(*rigidDynamic, { force.x, force.y, force.z }, { location.x, location.y, location.z });
    }

    void PhysicsRigidDynamic::AddLocalForceAtLocation(const LibMath::Vector3& force, const LibMath::Vector3& location) const
    {
        physx::PxRigidBodyExt::addLocalForceAtPos(*rigidDynamic, { force.x, force.y, force.z }, { location.x, location.y, location.z });
    }

    void PhysicsRigidDynamic::AddLocalForceAtLocalLocation(const LibMath::Vector3& force,
        const LibMath::Vector3& location) const
    {
        physx::PxRigidBodyExt::addLocalForceAtLocalPos(*rigidDynamic, { force.x, force.y, force.z }, { location.x, location.y, location.z });
    }

    void PhysicsRigidDynamic::SetIsTriggerShape(const bool isTrigger)
    {
        physx::PxShape* shape;
        rigidDynamic->getShapes(&shape, 1);
        if(isTrigger)
        {
            shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
            shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
        }
        else
        {
            shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
            shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
        }

        isTriggerShape = isTrigger;
    }

    void PhysicsRigidDynamic::EnableGravity(const bool gravity) const
    {
        if(!IsKinematic())
        {
            rigidDynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !gravity);
            if (gravity)
                rigidDynamic->wakeUp();
        }
    }

    bool PhysicsRigidDynamic::IsGravityEnabled() const
    {
        return !rigidDynamic->getActorFlags().isSet(physx::PxActorFlag::eDISABLE_GRAVITY);
    }

    void PhysicsRigidDynamic::Release() const
    {
        if(rigidDynamic)
            rigidDynamic->release();
    }

    PhysicsVehicleActor::PhysicsVehicleActor(): vehicle(nullptr)
    {
        inputData = Core::MemoryPool::Alloc<physx::PxVehicleDrive4WRawInputData>();
    }

    PhysicsVehicleActor::PhysicsVehicleActor(PhysicsVehicleActor&& other) noexcept :
        PhysicsRigidActor(other),
        isInTheAir(other.isInTheAir),
        rightInputValue(other.rightInputValue),
        forwardInputValue(other.forwardInputValue),
        isAnalogInputs(other.isAnalogInputs),
        isReversed(other.isReversed),
        vehicle(other.vehicle),
        inputData(other.inputData)
    {
        vehicle->getRigidDynamicActor()->userData = this;
        other.vehicle = nullptr;
        other.inputData = nullptr;
    }

    void PhysicsVehicleActor::UpdatePhysicsRender()
    {
        auto* anchor = Core::Entity::GetEntity(handle)->GetAnchor();

        anchor->SetWorldPosition(Vec3Convert(vehicle->getRigidDynamicActor()->getGlobalPose().p) - localLocation);
        anchor->SetWorldRotation(QuaternionConvert(vehicle->getRigidDynamicActor()->getGlobalPose().q));
    }

    void PhysicsVehicleActor::UpdateWorldLocation()
    {
        const auto& newLocation = Core::Entity::GetEntity(handle)->GetAnchor()->GetWorldTransformNoCheck().position + localLocation;

        vehicle->getRigidDynamicActor()->setGlobalPose(physx::PxTransform(newLocation.x, newLocation.y, newLocation.z, vehicle->getRigidDynamicActor()->getGlobalPose().q));
    }

    void PhysicsVehicleActor::UpdateWorldRotation()
    {
        const auto& newRotation = Core::Entity::GetEntity(handle)->GetAnchor()->GetWorldTransformNoCheck().rotation * localRotation;

        vehicle->getRigidDynamicActor()->setGlobalPose(physx::PxTransform(vehicle->getRigidDynamicActor()->getGlobalPose().p,
            physx::PxQuat(newRotation.X, newRotation.Y, newRotation.Z, newRotation.W)));
    }

    LibMath::Vector3 PhysicsVehicleActor::GetWorldLocation() const
    {
        return Vec3Convert(vehicle->getRigidDynamicActor()->getGlobalPose().p);
    }

    LibMath::Quaternion PhysicsVehicleActor::GetWorldRotation() const
    {
        return QuaternionConvert(vehicle->getRigidDynamicActor()->getGlobalPose().q);
    }

    void PhysicsVehicleActor::GetWorldTransform(LibMath::Vector3& location, LibMath::Quaternion& rotation) const
    {
        location = Vec3Convert(vehicle->getRigidDynamicActor()->getGlobalPose().p);
        rotation = QuaternionConvert(vehicle->getRigidDynamicActor()->getGlobalPose().q);
    }

    void PhysicsVehicleActor::MoveForward(const float forwardValue)
    {
        forwardInputValue = forwardValue;
        if(forwardValue > 0.f)
        {
            if(isReversed)
            {
                vehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
                vehicle->mDriveDynData.setUseAutoGears(true);
                isReversed = false;
            }

            if(isAnalogInputs)
            {
                inputData->setAnalogAccel(forwardValue);
            }
            else
            {
                inputData->setDigitalAccel(true);
            }
        }
        else if(forwardValue < 0.f)
        {
            if (!isReversed)
            {
                vehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eREVERSE);
                isReversed = true;
            }

            if (isAnalogInputs)
            {
                inputData->setAnalogAccel(forwardValue);
            }
            else
            {
                inputData->setDigitalAccel(true);
            }
        }
        else
        {
            inputData->setAnalogAccel(0);
            inputData->setDigitalAccel(false);
        }
    }

    void PhysicsVehicleActor::MoveRight(const float rightValue)
    {
        rightInputValue = rightValue;

        if(rightValue > 0.f)
        {
            if(isAnalogInputs)
            {
                inputData->setAnalogSteer(rightValue);
            }
            else
            {
                inputData->setDigitalSteerRight(true);
            }
        }
        else if(rightValue < 0.f)
        {
            if (isAnalogInputs)
            {
                inputData->setAnalogSteer(rightValue);
            }
            else
            {
                inputData->setDigitalSteerLeft(true);
            }
        }
        else
        {
            inputData->setAnalogSteer(0);
            inputData->setDigitalSteerLeft(false);
            inputData->setDigitalSteerRight(false);
        }
    }

    Core::Transform PhysicsVehicleActor::GetChassisTransform() const
    {
        auto* vehicleActor = vehicle->getRigidDynamicActor();
        auto* shapes = new physx::PxShape * [vehicleActor->getNbShapes()];
        vehicleActor->getShapes(shapes, vehicleActor->getNbShapes());

        Core::Transform transform;
        transform.position = Vec3Convert(shapes[wheelsCount]->getLocalPose().p);
        transform.rotation = QuaternionConvert(shapes[wheelsCount]->getLocalPose().q);
        transform.scale = { 1, 1, 1 };

        return transform;
    }

    void PhysicsVehicleActor::GetWheelsLocalTransforms(std::vector<Core::Transform>& wheelsLocations) const
    {
        auto* vehicleActor = vehicle->getRigidDynamicActor();
        auto* shapes = new physx::PxShape*[vehicleActor->getNbShapes()];
        vehicleActor->getShapes(shapes, vehicleActor->getNbShapes());

        for(uint32_t i = 0; i < wheelsCount; i++)
        {
            Core::Transform transform;
            transform.position = Vec3Convert(shapes[i]->getLocalPose().p);
            transform.rotation = QuaternionConvert(shapes[i]->getLocalPose().q);
            transform.scale = { 1, 1, 1 };
            wheelsLocations.emplace_back(transform);
        }
        delete[] shapes;
    }

    void PhysicsVehicleActor::Release() const
    {
        if(inputData)
            Core::MemoryPool::Free(inputData);

        if(vehicle)
        {
            vehicle->getRigidDynamicActor()->release();
            vehicle->release();
        }
    }

    void PhysicsVehicleActor::ReleaseInputs() const
    {
        if (isAnalogInputs)
        {
            inputData->setAnalogSteer(0.f);
            inputData->setAnalogAccel(0.f);
            inputData->setAnalogBrake(0.f);
            inputData->setAnalogHandbrake(0.f);
        }
        else
        {
            inputData->setDigitalSteerLeft(false);
            inputData->setDigitalSteerRight(false);
            inputData->setDigitalAccel(false);
            inputData->setDigitalBrake(false);
            inputData->setDigitalHandbrake(false);
        }
    }
}
