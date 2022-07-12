#pragma once
#include "Vector/Vector3.h"
#include "core/Delegate.h"
#include "core/ECS/Component.h"
#include "core/scenegraph/Transform.h"





namespace LibMath {
    struct Quaternion;
}

namespace physx {
    class PxVehicleDrive4WRawInputData;
    class PxVehicleDrive4W;
    class PxArticulationLink;
    class PxRigidDynamic;
    class PxRigidStatic;
    class PxRigidActor;
}

namespace Physics
{
    class PhysicsVehicle;
    class PhysicsInstance;
    class PhysicsRigidActor;

    enum class EGeometryType
    {
        CONVEX_MESH,
        TRIANGLE_MESH,
        SPHERE,
        CAPSULE,
        CAPSULE_UPRIGHT,
        BOX,
        VEHICLE,
        EMPTY
    };

    struct SweepHit
    {
        SweepHit() = default;

        SweepHit(const LibMath::Vector3& otherLocation, const LibMath::Vector3& otherNormal, const bool otherIsPenetrating) :
            location(otherLocation), normal(otherNormal), isPenetrating(otherIsPenetrating), isTriggerCollision(false)
        {}


        LibMath::Vector3    location;
        LibMath::Vector3    normal;
        bool                isPenetrating = true;
        bool                isTriggerCollision = true;
    };

    DELEGATE_Two_Params(EndOverlap, PhysicsRigidActor*, thisObject, PhysicsRigidActor*, otherObject);
    DELEGATE_Three_Params(BeginOverlap, PhysicsRigidActor*, thisObject, PhysicsRigidActor*, otherObject, SweepHit, sweepHit);

    class PhysicsRigidActor
    {
    public:
        PhysicsRigidActor() = default;
        virtual ~PhysicsRigidActor() = default;

        PhysicsRigidActor(const PhysicsRigidActor& other) = default;
        PhysicsRigidActor(PhysicsRigidActor&&) = delete;
        PhysicsRigidActor& operator=(const PhysicsRigidActor&) = delete;
        PhysicsRigidActor& operator=(PhysicsRigidActor&&) = delete;

        virtual void    UpdateWorldLocation() = 0;
        virtual void    UpdateWorldRotation() = 0;

        /**
         * Called internally to notify the actor that's its physics was updated. 
         */
        virtual void    UpdatePhysicsRender(){}

        void    UpdateTransform() { UpdateWorldLocation(); UpdateWorldRotation(); }

        void    SetLocalLocation(const LibMath::Vector3& newLocalLocation) { localLocation = newLocalLocation; UpdateWorldLocation(); }
        void    SetLocalRotation(const LibMath::Quaternion& newLocalRotation) { localRotation = newLocalRotation; UpdateWorldRotation(); }
        void    SetPhysicsEntityHandle(Core::EntityHandle otherHandle);
        void    SetIsCanChangerEntityTransform(const bool isChangeTransform) { isCanChangeEntityTransform = isChangeTransform; }

        void    GetLocalTransform(LibMath::Vector3& location, LibMath::Quaternion& rotation) const { location = localLocation; rotation = localRotation; }
        [[nodiscard]] const LibMath::Vector3&       GetLocalLocation() const { return localLocation; }
        [[nodiscard]] const LibMath::Quaternion&    GetLocalRotation() const { return localRotation; }
        [[nodiscard]] Core::EntityHandle                  GetPhysicsEntityHandle() const { return handle; }

        [[nodiscard]] bool          IsCanChangeEntityTransform() const { return isCanChangeEntityTransform; }
        [[nodiscard]] bool          IsTriggerShape() const { return isTriggerShape; }

        void    SetGeometryType(const EGeometryType newGeometry) { geometryType = newGeometry; }
        [[nodiscard]] EGeometryType GetGeometryType() const { return geometryType; }

        bool            dirtyTransform = false;
        bool            isCanOverlap = true;

        OnBeginOverlap  onBeginOverlap;
        OnEndOverlap    onEndOverlap;

    protected:
        Core::EntityHandle      handle;

        bool    isCanChangeEntityTransform = true; // Whether the physics actor should change its entity transform when moving
        bool    isTriggerShape = false;

        LibMath::Vector3    localLocation;
        LibMath::Quaternion localRotation;

        EGeometryType   geometryType = EGeometryType::EMPTY;
    };

    class PhysicsRigidStatic : public PhysicsRigidActor
    {
    public:
        PhysicsRigidStatic() = default;

        PhysicsRigidStatic(const PhysicsRigidStatic&) = default;
        PhysicsRigidStatic(PhysicsRigidStatic&&) noexcept;
        PhysicsRigidStatic& operator=(const PhysicsRigidStatic&) = delete;
        PhysicsRigidStatic& operator=(PhysicsRigidStatic&&) = delete;

        void    AttachToEntity();

        void UpdateWorldLocation() override;
        void UpdateWorldRotation() override;

        [[nodiscard]] LibMath::Vector3      GetWorldLocation() const;
        [[nodiscard]] LibMath::Quaternion   GetWorldRotation() const;
        void                                GetWorldTransform(LibMath::Vector3& location, LibMath::Quaternion& rotation) const;

        void    SetIsTriggerShape(bool isTrigger);

        void    Release() const;

    protected:

        physx::PxRigidStatic*   rigidStatic = nullptr;
    private:
        friend PhysicsInstance;
    };

    class PhysicsRigidDynamic : public PhysicsRigidActor
    {
    public:
        PhysicsRigidDynamic() = default;

        PhysicsRigidDynamic(const PhysicsRigidDynamic&) = default;
        PhysicsRigidDynamic(PhysicsRigidDynamic&&) noexcept;
        PhysicsRigidDynamic& operator=(const PhysicsRigidDynamic&) = delete;
        PhysicsRigidDynamic& operator=(PhysicsRigidDynamic&&) = delete;

        void    AttachToEntity();

        void    UpdatePhysicsRender() override;

        void    UpdateWorldLocation() override;
        void    UpdateWorldRotation() override;

        [[nodiscard]] LibMath::Vector3      GetWorldLocation() const;
        [[nodiscard]] LibMath::Quaternion   GetWorldRotation() const;
        void                                GetWorldTransform(LibMath::Vector3& location, LibMath::Quaternion& rotation) const;

        void    SetMass(float mass) const;
        void    SetKinematic(bool isKinematic) const;
        void    SetLinearDamping(float angularDamping) const;
        void    SetAngularDamping(float angularDamping) const;
        void    SetLinearVelocity(const LibMath::Vector3& velocity) const;
        void    SetAngularVelocity(const LibMath::Vector3& velocity) const;
        void    SetMassSpaceInertiaTensor(const LibMath::Vector3& tensor) const;

        [[nodiscard]] float GetRigidMass() const;
        [[nodiscard]] bool  IsKinematic() const;
        [[nodiscard]] float GetRigidAngularDamping() const;
        [[nodiscard]] float GetRigidLinearDamping() const;
        [[nodiscard]] LibMath::Vector3  GetRigidLinearVelocity() const;
        [[nodiscard]] LibMath::Vector3  GetRigidAngularVelocity() const;

        void    AddForce(const LibMath::Vector3& force) const;
        void    AddImpulse(const LibMath::Vector3& impulse) const;
        void    AddAcceleration(const LibMath::Vector3& acceleration) const;
        void    AddForceAtLocation(const LibMath::Vector3& force, const LibMath::Vector3& location) const;
        void    AddForceAtLocalLocation(const LibMath::Vector3& force, const LibMath::Vector3& location) const;
        void    AddLocalForceAtLocation(const LibMath::Vector3& force, const LibMath::Vector3& location) const;
        void    AddLocalForceAtLocalLocation(const LibMath::Vector3& force, const LibMath::Vector3& location) const;

        void    SetIsTriggerShape(bool isTrigger);

        void    EnableGravity(bool gravity) const;
        [[nodiscard]] bool    IsGravityEnabled() const;

        void    Release() const;
    protected:

        physx::PxRigidDynamic*  rigidDynamic = nullptr;

    private:
        friend PhysicsInstance;
    };

    class PhysicsVehicleActor : public PhysicsRigidActor
    {
    public:
        PhysicsVehicleActor();

        PhysicsVehicleActor(const PhysicsVehicleActor&) = default;
        PhysicsVehicleActor(PhysicsVehicleActor&&) noexcept;
        PhysicsVehicleActor& operator=(const PhysicsVehicleActor&) = delete;
        PhysicsVehicleActor& operator=(PhysicsVehicleActor&&) = delete;

        void    UpdatePhysicsRender() override;

        void    UpdateWorldLocation() override;
        void    UpdateWorldRotation() override;

        [[nodiscard]] LibMath::Vector3      GetWorldLocation() const;
        [[nodiscard]] LibMath::Quaternion   GetWorldRotation() const;
        void                                GetWorldTransform(LibMath::Vector3& location, LibMath::Quaternion& rotation) const;

        [[nodiscard]] bool  IsInTheAir() const { return isInTheAir; }
        void                SetIsInTheAir(const bool air) { isInTheAir = air; }

        void    MoveForward(float forwardValue);
        void    MoveRight(float rightValue);
        void    ReleaseInputs() const;

        [[nodiscard]] uint32_t  GetWheelsCount() const { return wheelsCount; }
        [[nodiscard]] Core::Transform         GetChassisTransform() const;
        void                    GetWheelsLocalTransforms(std::vector<Core::Transform>& wheelsLocations) const;

        [[nodiscard]] float     GetForwardInputValue() const { return forwardInputValue; }
        [[nodiscard]] float     GetRightInputValue() const { return rightInputValue; }

        [[nodiscard]] int       GetVehicleId() const { return vehicleId; }

        void    Release() const;

    protected:
        friend PhysicsVehicle;

        void    SetVehicleId(const int id) { vehicleId = id; }

        bool    isInTheAir = false;

        float   rightInputValue = 0.f;
        float   forwardInputValue = 0.f;

        bool    isAnalogInputs = false;
        bool    isReversed = false;

        uint32_t    wheelsCount = 4;

    private:
        physx::PxVehicleDrive4W* vehicle;
        physx::PxVehicleDrive4WRawInputData* inputData;

        int     vehicleId = -1;
    };



    namespace Wrapper // todo: replace with real reflection data
    {
        inline static const Core::Type PhysicsRigidDynamic_("PhysicsRigidDynamic", Core::ETypeCategory::WRAPPER, sizeof(PhysicsRigidDynamic), 8);
        inline static const Core::Type PhysicsRigidStatic_("PhysicsRigidStatic", Core::ETypeCategory::WRAPPER, sizeof(PhysicsRigidStatic), 8);
    }
}
