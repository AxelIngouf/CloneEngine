#pragma once

#include <vector>

#include "SceneNode.h"

namespace Core
{
	class SceneGraph
	{
	public:
		SceneGraph() = default;
		SceneGraph(const SceneGraph&) = delete;
		SceneGraph(SceneGraph&&) = delete;
		~SceneGraph();

		SceneGraph& operator=(const SceneGraph&) = delete;
		SceneGraph& operator=(SceneGraph&&) = delete;

		void UpdateAll();
		//void UpdateVisible(class Camera* view); //todo: add bounding sphere to sceneNode

		SceneNode* GetRoot() { return root; }

	private:

		SceneNode* root = SceneNode::CreateRoot();

	};

	/*
	class OptimizeSceneGraph
	{
	public:
		OptimizeSceneGraph() = default;
		OptimizeSceneGraph(const OptimizeSceneGraph&) = delete;
		OptimizeSceneGraph(OptimizeSceneGraph&&) = delete;
		virtual ~OptimizeSceneGraph();

		OptimizeSceneGraph& operator=(const OptimizeSceneGraph&) = delete;
		OptimizeSceneGraph& operator=(OptimizeSceneGraph&&) = delete;

		bool Save(const char* filepath);
		bool Load(const char* filepath);

		void Update(class Camera* view = nullptr);

		bool DestroyNode(SceneNode* node);
		SceneNode* CreateNode(SceneNode* parent);
		bool ChangeMobility(SceneNode* node, ENodeMobility mobility);
		bool ChangeMobility(SceneNode* node, ENodeMobility mobility, bool changeChildren);

	private:

		std::vector<SceneNode> immovable;
		std::vector<SceneNode> movable;
		std::vector<SceneNode> moving;
		std::vector<SceneNode*> temporary;

		std::vector<GraphicInfo> graphic;

		SceneNode* firstMoving;
		SceneNode* firstDirty;
	};
	*/
}
