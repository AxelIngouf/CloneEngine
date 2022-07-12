#include "SceneGraph.h"

namespace Core
{
    SceneGraph::~SceneGraph()
    {
        root->Destroy();
    }

    void SceneGraph::UpdateAll()
    {
        root->DeepCleanWorldTransform();
    }
}
