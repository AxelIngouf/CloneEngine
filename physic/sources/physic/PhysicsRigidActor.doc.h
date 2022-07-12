#pragma once
#include "Vector/Vector3.h"
#include "core/Delegate.h"
#include "core/ECS/Component.h"

namespace LibMath {
    struct Quaternion;
}

namespace physx {
    class PxArticulationLink;
    class PxRigidDynamic;
    class PxRigidStatic;
    class PxRigidActor;
}

namespace Physics
{
    class PhysicsInstance;
    class PhysicsRigidActor;

    /**
     * Geometry type enum used by rigid actors.
     */
    enum class EGeometryType
    {
        MESH,
        SPHERE,
        CAPSULE,
        CAPSULE_UPRIGHT,
        BOX
    };

    /**
     * Struct used to symbolize a sweep hit. Contains hit location, normal and whether or not the two actors penetrated.
     */
    struct SweepHit
    {
        SweepHit(const LibMath::Vector3& otherLocation, const LibMath::Vector3& otherNormal, const bool otherIsPenetrating) :
            location(otherLocation), normal(otherNormal), isPenetrating(otherIsPenetrating)
        {}

        LibMath::Vector3    location;
        LibMath::Vector3    normal;
        bool                isPenetrating;
    };

    DELEGATE_Three_Params(Overlap, PhysicsRigidActor*, thisObject, PhysicsRigidActor*, otherObject, SweepHit, sweepHit);

    /**
     * Physics rigid actor component virtual class.
     */
    class PhysicsRigidActor
    {
    public:
        PhysicsRigidActor() = delete;
        virtual ~PhysicsRigidActor();

        PhysicsRigidActor(const PhysicsRigidActor&);
        PhysicsRigidActor(PhysicsRigidActor&&) = delete;
        PhysicsRigidActor& operator=(const PhysicsRigidActor&) = delete;
        PhysicsRigidActor& operator=(PhysicsRigidActor&&) = delete;

        /**
         * Sets rigid location.
         * 
         * @param location New location.
         */
        virtual void    SetRigidLocation(const LibMath::Vector3& location) = 0;

        /**
         * Sets rigid rotation.
         * 
         * @param rotation New rotation.
         */
        virtual void    SetRigidRotation(const LibMath::Quaternion& rotation) = 0;

        /**
         * Sets rigid rotation and location.
         * 
         * @param location New location.
         * @param rotation New rotation.
         */
        virtual void    SetRigidTransform(const LibMath::Vector3& location, const LibMath::Quaternion& rotation) = 0;

        /**
         * Returns the current rigid location.
         * 
         * @return Current rigid location.
         */
        [[nodiscard]] virtual LibMath::Vector3    GetRigidLocation() = 0;

        /**
         * Returns the current rigid rotation.
         * 
         * @return Current rigid rotation.
         */
        [[nodiscard]] virtual LibMath::Quaternion GetRigidRotation() = 0;

        /**
         * Returns the current rigid location and rotation.
         * 
         * @param location Current rigid location.
         * @param rotation Current rigid rotation.
         */
        virtual void    GetRigidTransform(LibMath::Vector3& location, LibMath::Quaternion& rotation) = 0;

        /**
         * Boolean indication that the rigid actor transform was changed and should be updated in the rendering.
         */
        bool            dirtyTransform = false;

        /**
         * Actor geometry type (Capsule, Box, Sphere or Convex mesh)
         */
        EGeometryType   geometryType;
        bool            isCanOverlap = true;

        OnOverlap   onOverlap;

    protected:
        PhysicsRigidActor(const EGeometryType geometry) : geometryType(geometry) { }
    };

    /**
     * Physics rigid static component.
     */
    class PhysicsRigidStatic : public PhysicsRigidActor
    {
    public:

        PhysicsRigidStatic(const PhysicsRigidStatic&) = delete;
        PhysicsRigidStatic(PhysicsRigidStatic&&) = delete;
        PhysicsRigidStatic& operator=(const PhysicsRigidStatic&) = delete;
        PhysicsRigidStatic& operator=(PhysicsRigidStatic&&) = delete;

        void    SetRigidLocation(const LibMath::Vector3& location) override;
        void    SetRigidRotation(const LibMath::Quaternion& rotation) override;
        void    SetRigidTransform(const LibMath::Vector3& location, const LibMath::Quaternion& rotation) override;
        [[nodiscard]] LibMath::Vector3      GetRigidLocation() override;
        [[nodiscard]] LibMath::Quaternion   GetRigidRotation() override;
        void    GetRigidTransform(LibMath::Vector3& location, LibMath::Quaternion& rotation) override;

    private:
        friend PhysicsInstance;

        explicit PhysicsRigidStatic(physx::PxRigidStatic* body, EGeometryType type);

        /**
         * physx internal rigid static. Should not be used in your code.
         */
        physx::PxRigidStatic*   rigidStatic;
    };

    /**
     * Physics rigid dynamic component.
     */
    class PhysicsRigidDynamic : PhysicsRigidActor
    {
    public:

        PhysicsRigidDynamic(const PhysicsRigidDynamic&) = delete;
        PhysicsRigidDynamic(PhysicsRigidDynamic&&) noexcept;
        PhysicsRigidDynamic& operator=(const PhysicsRigidDynamic&) = delete;
        PhysicsRigidDynamic& operator=(PhysicsRigidDynamic&&) = delete;

        void    SetRigidLocation(const LibMath::Vector3& location) override;
        void    SetRigidRotation(const LibMath::Quaternion& rotation) override;
        void    SetRigidTransform(const LibMath::Vector3& location, const LibMath::Quaternion& rotation) override;
        [[nodiscard]] LibMath::Vector3      GetRigidLocation() override;
        [[nodiscard]] LibMath::Quaternion   GetRigidRotation() override;
        void    GetRigidTransform(LibMath::Vector3& location, LibMath::Quaternion& rotation) override;

        /**
         * Sets mass.
         *
         * @param mass New mass.
         */
        void    SetMass(float mass) const;

        /**
         * Sets whether or not the rigid dynamic should be kinematic.
         *
         * @param isKinematic True if the rigid dynamic should be kinematic.
         */
        void    SetKinematic(bool isKinematic) const;

        /**
         * Sets linear damping.
         *
         * @param angularDamping New linear damping.
         */
        void    SetLinearDamping(float angularDamping) const;

        /**
         * Sets angular damping.
         *
         * @param angularDamping New angular damping.
         */
        void    SetAngularDamping(float angularDamping) const;

        /**
         * Sets linear velocity.
         *
         * @param velocity New linear velocity.
         */
        void    SetLinearVelocity(const LibMath::Vector3& velocity) const;

        /**
         * Sets angular velocity.
         *
         * @param velocity New angular velocity.
         */
        void    SetAngularVelocity(const LibMath::Vector3& velocity) const;

        /**
         * Sets mass space inertia tensor.
         *
         * @param tensor New tensor.
         */
        void    SetMassSpaceInertiaTensor(const LibMath::Vector3& tensor) const;

        /**
         * Returns mass.
         *
         * @return Mass.
         */
        [[nodiscard]] float GetRigidMass() const;

        /**
         * Returns whether or not the rigid dynamic is kinematic.
         *
         * @return True if the rigid dynamic is kinematic.
         */
        [[nodiscard]] bool  IsKinematic() const;

        /**
         * Returns angular damping.
         *
         * @return Angular damping.
         */
        [[nodiscard]] float GetRigidAngularDamping() const;

        /**
         * Returns linear damping.
         *
         * @return Linear damping.
         */
        [[nodiscard]] float GetRigidLinearDamping() const;

        /**
         * Returns linear velocity.
         *
         * @return Linear velocity.
         */
        [[nodiscard]] LibMath::Vector3  GetRigidLinearVelocity() const;

        /**
         * Returns angular velocity.
         *
         * @return Angular velocity.
         */
        [[nodiscard]] LibMath::Vector3  GetRigidAngularVelocity() const;

        /**
         * Add force.
         * 
         * @param force Force to apply.
         */
        void    AddForce(const LibMath::Vector3& force) const;

        /**
         * Add impulse. If you want a continuous force, you might need to use AddForce function.
         * 
         * @param impulse Impulse to apply.
         */
        void    AddImpulse(const LibMath::Vector3& impulse) const;

        /**
         * Add acceleration.
         * 
         * @param acceleration Acceleration to apply.
         */
        void    AddAcceleration(const LibMath::Vector3& acceleration) const;

        /**
         * Add force at world location.
         * 
         * @param force Force to apply.
         * @param location World location to apply force to.
         */
        void    AddForceAtLocation(const LibMath::Vector3& force, const LibMath::Vector3& location) const;

        /**
         * Add force at local location.
         * 
         * @param force Force to apply.
         * @param location Local location.
         */
        void    AddForceAtLocalLocation(const LibMath::Vector3& force, const LibMath::Vector3& location) const;

        /**
         * Add local force at world location.
         * 
         * @param force Local force to apply.
         * @param location World location.
         */
        void    AddLocalForceAtLocation(const LibMath::Vector3& force, const LibMath::Vector3& location) const;

        /**
         * Add local force at local location.
         * 
         * @param force Local force to apply.
         * @param location Local location.
         */
        void    AddLocalForceAtLocalLocation(const LibMath::Vector3& force, const LibMath::Vector3& location) const;

        /**
         * Enable or disable gravity on the current rigid dynamic.
         * 
         * @param gravity True to enable gravity, false to disable it.
         */
        void    EnableGravity(bool gravity) const;

    private:
        friend PhysicsInstance;

        explicit PhysicsRigidDynamic(physx::PxRigidDynamic* body, EGeometryType type);

        /**
         * physx internal rigid dynamic. Should not be used in your code.
         */
        physx::PxRigidDynamic*  rigidDynamic;
    };
}


