#include "PhysicsMeshComponent.h"
#include "core/ECS/Entity.h"
#include "model/Model.h"
#include "model/Vertex.h"
#include "physic/PhysicsManager.h"

namespace Physics
{
	StaticConvexMeshParams::StaticConvexMeshParams() :
		vertices(nullptr),
		file(File(EFileType::MODEL)),
		staticFriction(0.5f),
		dynamicFriction(0.5f),
		restitution(0.6f),
		isTrigger(false)
	{
	}

	DynamicConvexMeshParams::DynamicConvexMeshParams() :
		vertices(nullptr),
		file(File(EFileType::MODEL)),
		staticFriction(0.5f),
		dynamicFriction(0.5f),
		restitution(0.6f),
		angularDamping(0.5f),
		isTrigger(false)
	{
	}


	PhysicsDynamicConvexMeshComponent::PhysicsDynamicConvexMeshComponent(PhysicsDynamicConvexMeshComponent&& other) noexcept :
		Component<PhysicsDynamicConvexMeshComponent>(other),
		PhysicsRigidDynamic(std::move(other)),
		vertices(other.vertices),
		path(other.path),
		staticFriction(other.staticFriction),
		dynamicFriction(other.dynamicFriction),
		restitution(other.restitution),
		velocity(other.velocity),
		angularDamping(other.angularDamping),
		isTrigger(other.isTrigger)
	{
	}

	void PhysicsDynamicConvexMeshComponent::Initialize(const void* params)
	{
		if (params != nullptr)
		{
			const DynamicConvexMeshParams* initParams = static_cast<const DynamicConvexMeshParams*>(params);

			vertices = initParams->vertices;
			path = initParams->file.path;
			staticFriction = initParams->staticFriction;
			dynamicFriction = initParams->dynamicFriction;
			restitution = initParams->restitution;
			velocity = initParams->velocity;
			angularDamping = initParams->angularDamping;
			isTrigger = initParams->isTrigger;
		}
		else
		{
			LOG(LOG_ERROR, "Convex mesh component initializer was not set in AddComponent().", Core::ELogChannel::CLOG_PHYSICS);
		}
	}

	void PhysicsDynamicConvexMeshComponent::Constructor()
	{
		SetPhysicsEntityHandle(GetEntityHandle());

		auto* vert = static_cast<std::vector<Model::Vertex>*>(vertices);

		if (vert == nullptr)
		{
			Model::Model(path.c_str());
			CreateConvexMeshRigidDynamicFromModel(this, Core::Entity::GetEntity(GetEntityHandle()), path, staticFriction, dynamicFriction, restitution, velocity, angularDamping);
		}
		else
		{
			CreateConvexMeshRigidDynamic(this, Core::Entity::GetEntity(GetEntityHandle()), *vert, staticFriction, dynamicFriction, restitution, velocity, angularDamping);
		}

		if (isTrigger)
			SetIsTriggerShape(true);
	}

    void PhysicsDynamicConvexMeshComponent::Finalize()
    {
		Release();
    }

	PhysicsStaticConvexMeshComponent::PhysicsStaticConvexMeshComponent(
		PhysicsStaticConvexMeshComponent&& other) noexcept :
		Component<PhysicsStaticConvexMeshComponent>(other),
		PhysicsRigidStatic(std::move(other)),
		vertices(other.vertices),
		path(other.path),
		staticFriction(other.staticFriction),
		dynamicFriction(other.dynamicFriction),
		restitution(other.restitution),
		isTrigger(other.isTrigger)
	{
	}

	void PhysicsStaticConvexMeshComponent::Initialize(const void* params)
	{
		if (params != nullptr)
		{
			const StaticConvexMeshParams* initParams = static_cast<const StaticConvexMeshParams*>(params);

			vertices = initParams->vertices;
			path = initParams->file.path;
			staticFriction = initParams->staticFriction;
			dynamicFriction = initParams->dynamicFriction;
			restitution = initParams->restitution;
			isTrigger = initParams->isTrigger;
		}
		else
		{
			LOG(LOG_ERROR, "Convex mesh component initializer was not set in AddComponent().", Core::ELogChannel::CLOG_PHYSICS);
		}
	}

	void PhysicsStaticConvexMeshComponent::Constructor()
	{
		SetPhysicsEntityHandle(GetEntityHandle());

		auto* vert = static_cast<std::vector<Model::Vertex>*>(vertices);

		if (vert == nullptr)
		{
			Model::Model(path.c_str());
			CreateConvexMeshRigidStaticFromModel(this, Core::Entity::GetEntity(GetEntityHandle()), path, staticFriction, dynamicFriction, restitution);
		}
		else
		{
			CreateConvexMeshRigidStatic(this, Core::Entity::GetEntity(GetEntityHandle()), *vert);
		}

		if (isTrigger)
			SetIsTriggerShape(true);
	}

    void PhysicsStaticConvexMeshComponent::Finalize()
    {
		Release();
    }

	TriangleMeshInitParams::TriangleMeshInitParams() :
		vertices(nullptr),
		indices(nullptr),
		file(File(EFileType::MODEL)),
		staticFriction(0.5f),
		dynamicFriction(0.5f),
		restitution(0.6f),
		isTrigger(false)
	{
	}

    PhysicsStaticTriangleMeshComponent::PhysicsStaticTriangleMeshComponent(
		PhysicsStaticTriangleMeshComponent&& other) noexcept :
		Component<PhysicsStaticTriangleMeshComponent>(other),
		PhysicsRigidStatic(std::move(other)),
		vertices(other.vertices),
		indices(other.indices),
		path(other.path),
		staticFriction(other.staticFriction),
		dynamicFriction(other.dynamicFriction),
		restitution(other.restitution),
		isTrigger(other.isTrigger)
	{
	}

	void PhysicsStaticTriangleMeshComponent::Initialize(const void* params)
	{
		if (params != nullptr)
		{
			const TriangleMeshInitParams* initParams = static_cast<const TriangleMeshInitParams*>(params);

			vertices = initParams->vertices;
			indices = initParams->indices;
			path = initParams->file.path;
			staticFriction = initParams->staticFriction;
			dynamicFriction = initParams->dynamicFriction;
			restitution = initParams->restitution;
			isTrigger = initParams->isTrigger;
		}
		else
		{
			LOG(LOG_ERROR, "Triangle mesh component initializer was not set in AddComponent().", Core::ELogChannel::CLOG_PHYSICS);
		}
	}

	void PhysicsStaticTriangleMeshComponent::Constructor()
	{
		SetPhysicsEntityHandle(GetEntityHandle());

		auto* vert = static_cast<std::vector<Model::Vertex>*>(vertices);
		auto* ind = static_cast<std::vector<unsigned>*>(indices);

		if (vert == nullptr || ind == nullptr)
		{
			Model::Model(path.c_str());
			CreateTriangleMeshRigidStaticFromModel(this, Core::Entity::GetEntity(GetEntityHandle()), path, staticFriction, dynamicFriction, restitution);
		}
		else
		{
			CreateTriangleMeshRigidStatic(this, Core::Entity::GetEntity(GetEntityHandle()), *vert, *ind, staticFriction, dynamicFriction, restitution);
		}

		if (isTrigger)
			SetIsTriggerShape(true);
	}

    void PhysicsStaticTriangleMeshComponent::Finalize()
    {
		Release();
    }
}
