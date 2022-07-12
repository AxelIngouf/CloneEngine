#pragma once

#include <vector>
#include <fstream>

#include "Handle.h"
#include "../filesys/MemoryMappedFile.h"
#include "Quaternion/Quaternion.h"
#include "Vector/Vector3.h"

namespace Core
{
	struct Field;
	class Structure;
	class SceneGraph;
	class SceneNode;

	typedef void (*UpdateComponentFunctionPtr)(float);
	typedef void (*BeginPlayComponentFunctionPtr)();

	typedef void* (*GetDataPtrFunctionPtr)();
	typedef int (*GetDataQtyFunctionPtr)();
	typedef const Structure* (*GetMetaDataFunctionPtr)();

	struct SerializeFunction
	{
		SerializeFunction(GetDataPtrFunctionPtr dataFunctionPtr, GetDataQtyFunctionPtr sizeFunctionPtr,
		                  GetMetaDataFunctionPtr metaData) :
			GetData(dataFunctionPtr),
			GetQty(sizeFunctionPtr),
			GetMeta(metaData)
		{
		}

		GetDataPtrFunctionPtr GetData;
		GetDataQtyFunctionPtr GetQty;
		GetMetaDataFunctionPtr GetMeta;
	};

	template <typename T>
	struct Lookup
	{
		Lookup(T oldValue, T newValue) :
			oldValue(oldValue),
			newValue(newValue)
		{
		}

		T oldValue;
		T newValue;
	};

	enum class EBranchManip { POP, PUSH, QUO };

	struct LevelFile
	{
		LevelFile(const char* filePath) :
			data(filePath),
			ptr(data.IsValid() ? data.GetData() : nullptr),
			endPtr(data.IsValid() ? ptr + data.GetSize() : nullptr)
		{
		}

		bool ReachEOF() { return ptr >= endPtr; }
		void Reset() { ptr = data.GetData(); }

		MemoryMappedFile data;
		const char* ptr;
		const char* const endPtr;

		char lastLetter;
		std::string wordBuffer;

		std::vector<Lookup<EntityHandle>> entityLookup;
		std::vector<Lookup<ComponentHandle>> componentLookup;
	};

	class World
	{
	public:
		World() = delete;
		~World() = delete;

		static void Initialize();
		static void Initialize(const char* fileName);
		static void Finalize();

		static SceneGraph* GetLevel() { return level; }

		static void RegisterComponentForBeginPlay(BeginPlayComponentFunctionPtr functionPtr);
		static void Start();
		static void UnPause();
		static void Pause();
		static void Stop();

		static void RegisterComponentForUpdate(UpdateComponentFunctionPtr functionPtr);
		static void UpdateAll(float elapsedTime);

		static void RegisterComponentForSerialization(GetDataPtrFunctionPtr dataFunctionPtr,
		                                              GetDataQtyFunctionPtr sizeFunctionPtr,
		                                              GetMetaDataFunctionPtr metaData);
		static bool Save(const char* fileName);

		static bool HasStarted() { return hasDoneBeginPlay; }
		static bool IsInPlay() { return isInPlay; }

	private:
		static void UpdateAllComponent(float elapsedTime);

		static bool SaveComponents(std::ofstream& file, const SerializeFunction& functions);
		static bool SaveNode(std::ofstream& file, const SceneNode* node);
		static bool SaveStructure(std::ofstream& file, const void* structure, const Structure* metaData);

		static bool Load(const char* fileName);
		static bool LoadLevel(LevelFile& file);
		static bool LoadComponents(LevelFile& file);
		static bool LoadStructure(LevelFile& file, void* data, const Structure* meta);
		static EBranchManip LoadNode(LevelFile& file, SceneNode* node);

		static bool ReadWord(LevelFile& file);
		static std::string ReadString(LevelFile& file);
		static LibMath::Vector3 ReadVector3(LevelFile& file);
		static LibMath::Quaternion ReadQuaternion(LevelFile& file);
		static EntityHandle ReadEntityHandle(LevelFile& file);

		static void AddEntityLookup(LevelFile& file, const SceneNode* node);
		static EntityHandle LookupEntity(LevelFile& file);

		inline static bool isInPlay = false;
		inline static bool hasDoneBeginPlay = false;
		inline static SceneGraph* level = nullptr;
		inline static std::vector<UpdateComponentFunctionPtr> updateComponentFunctions;
		inline static std::vector<BeginPlayComponentFunctionPtr> beginPlayComponentFunctions;
		inline static std::vector<SerializeFunction> serializaComponentFunctions;
	};
}
