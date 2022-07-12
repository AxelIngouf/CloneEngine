#include "World.h"

#include <stack>

#include "../reflection/EnumMeta.h"
#include "../reflection/StructMeta.h"
#include "../scenegraph/SceneGraph.h"
#include "../CLog.h"

namespace Core
{
	void World::Initialize()
	{
		if (level)
		{
			Finalize();
		}

		hasDoneBeginPlay = false;
		isInPlay = false;

		level = new SceneGraph();
	}

	void World::Initialize(const char* fileName)
	{
		Initialize();
		Load(fileName);
	}

	void World::Finalize()
	{
		isInPlay = false;
		hasDoneBeginPlay = false;
		delete level;
		level = nullptr;
	}

	void World::RegisterComponentForBeginPlay(BeginPlayComponentFunctionPtr functionPtr)
	{
		beginPlayComponentFunctions.push_back(functionPtr);
	}

	void World::Start()
	{
		hasDoneBeginPlay = true;
		isInPlay = true;
		for (BeginPlayComponentFunctionPtr BeginPlayComponentFunction : beginPlayComponentFunctions)
		{
			BeginPlayComponentFunction();
		}
	}

	void World::UnPause()
	{
		isInPlay = true;
	}

	void World::Pause()
	{
		isInPlay = false;
	}

	void World::Stop()
	{
		isInPlay = false;
		hasDoneBeginPlay = false;
	}

	void World::RegisterComponentForUpdate(UpdateComponentFunctionPtr functionPtr)
	{
		updateComponentFunctions.push_back(functionPtr);
	}

	void World::UpdateAll(float elapsedTime)
	{
		UpdateAllComponent(elapsedTime);
		level->UpdateAll();
	}

	void World::UpdateAllComponent(float elapsedTime)
	{
		for (UpdateComponentFunctionPtr UpdateComponentFunction : updateComponentFunctions)
		{
			UpdateComponentFunction(elapsedTime);
		}
	}

	void World::RegisterComponentForSerialization(GetDataPtrFunctionPtr dataFunctionPtr,
	                                              GetDataQtyFunctionPtr sizeFunctionPtr,
	                                              GetMetaDataFunctionPtr metaFunctionPtr)
	{
		serializaComponentFunctions.emplace_back(dataFunctionPtr, sizeFunctionPtr, metaFunctionPtr);
	}

	bool World::Save(const char* fileName)
	{
		std::ofstream file(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
		if (file.is_open() == false)
		{
			return false;
		}

		if (SaveNode(file, level->GetRoot()) == false)
		{
			return false;
		}

		for (const SerializeFunction& functions : serializaComponentFunctions)
		{
			if (SaveComponents(file, functions) == false)
			{
				return false;
			}
		}

		file.close();
		return true;
	}

	bool World::SaveNode(std::ofstream& file, const SceneNode* node)
	{
		char buffer[((sizeof(LibMath::Quaternion) > sizeof(LibMath::Vector3))
			             ? sizeof(LibMath::Quaternion)
			             : sizeof(LibMath::Vector3))];

		file << "<node ";

		file << "entity={";
		*(int*)buffer = node->GetEntity()->GetHandle().GetValue();
		file.write(buffer, sizeof(int));
		file << "}"; //todo: refacto

		file << "name=";
		*(int*)buffer = (int)node->GetName().size();
		file.write(buffer, sizeof(int));
		file << "{" << node->GetName() << "}";

		memcpy(buffer, &node->GetLocalTransform().position, sizeof(LibMath::Vector3));
		file << "position={";
		file.write(buffer, sizeof(LibMath::Vector3));
		file << "}";

		memcpy(buffer, &node->GetLocalTransform().rotation, sizeof(LibMath::Quaternion));
		file << "rotation={";
		file.write(buffer, sizeof(LibMath::Quaternion));
		file << "}";

		memcpy(buffer, &node->GetLocalTransform().scale, sizeof(LibMath::Vector3));
		file << "scale={";
		file.write(buffer, sizeof(LibMath::Vector3));
		file << "}";

		if (node->GetChildrenCount())
		{
			file << ">\n";

			for (const SceneNode* child : node->GetChildren())
			{
				if (SaveNode(file, child) == false)
				{
					return false;
				}
			}
			file << "</node>\n";
		}
		else
		{
			file << "/>\n";
		}

		return file.good();
	}

	bool World::SaveComponents(std::ofstream& file, const SerializeFunction& functions)
	{
		int qty = functions.GetQty();
		if (qty == 0)
		{
			return true;
		}

		const char* Data = (char*)functions.GetData();
		const Structure* MetaData = functions.GetMeta();

		for (int index = 0; index < qty; index++)
		{
			const char* current = Data + ((long long)index * (long long)MetaData->size);

			StructureField isInUse = MetaData->FindField("isInUse");
			if (*(bool*)isInUse.GetFrom(current) == false)
			{
				continue;
			}

			file << "\n";
			if (SaveStructure(file, current, MetaData) == false)
			{
				return false;
			}
		}

		return file.good();
	}

	bool World::SaveStructure(std::ofstream& file, const void* structure, const Structure* metaData)
	{
		file << metaData->name.text << "{";

		for (StructureField field : metaData->GetAllFields())
		{
			if (field.flags.include(FieldFlag::TRANSIENT))
			{
				continue;
			}

			file << field.name->text << "=";

			void* data = field.GetFrom(structure);
			if (field.flags.include(FieldFlag::POINTER)) // todo: test pointer extensivly (save & load)
			{
				std::string message("attempt to serialize a pointer : dereferencing pointer for ");
				message.append(metaData->name.text).append("::").append(field.name->text);
				LOG(LOG_WARNING, message);
				data = *(void**)data;
			}

			if (field.type->name.hash == ConstexprCustomHash("string")
				|| field.type->name.hash == ConstexprCustomHash("std::string"))
			{
				std::string& str = *(std::string*)data;

				char buffer[sizeof(int)];
				*(int*)buffer = (int)str.size();

				file.write(buffer, sizeof(int));
				file << "{" << str << "}";
			}
			else
			{
				switch (field.type->category)
				{
				case ETypeCategory::ENUMERATION:
				case ETypeCategory::PRIMITIVE:
				case ETypeCategory::WRAPPER:
					{
						file << "{";
						file.write((const char*)data, field.type->size);
						file << "}";
						break;
					}
				case ETypeCategory::STRUCTURE:
					SaveStructure(file, data, (Structure*)field.type);
					break;
				case ETypeCategory::POINTER:
				case ETypeCategory::COLLECTION:
					{
						std::string message(
							"attempt to serialize an unsupported data category : skipping serialization for ");
						message.append(metaData->name.text).append("::").append(field.name->text);
						LOG(LOG_WARNING, message);
						break;
					}
				default:
					{
						std::string message(
							"attempt to serialize an unknown data category : skipping serialization for ");
						message.append(metaData->name.text).append("::").append(field.name->text);
						LOG(LOG_WARNING, message);
						break;
					}
				}
			}
		}

		file << "}";

		return file.good();
	}

	bool World::Load(const char* fileName)
	{
		LevelFile file(fileName);

		if (file.data.IsValid())
		{
			if (LoadLevel(file) == false)
			{
				return false;
			}

			if (LoadComponents(file) == false)
			{
				return false;
			}
		}

		return true;
	}

	bool World::LoadLevel(LevelFile& file)
	{
		SceneNode* currentNode = level->GetRoot();
		std::stack<SceneNode*> currentBranch({nullptr});

		do
		{
			if (currentNode == nullptr)
			{
				currentNode = SceneNode::CreateRoot();
			}

			switch (LoadNode(file, currentNode))
			{
			case EBranchManip::POP:
				currentBranch.pop();
				break;


			case EBranchManip::PUSH:
				currentNode->ReParent(currentBranch.top());
				currentBranch.push(currentNode);
				currentNode = nullptr;
				break;


			case EBranchManip::QUO:
				currentNode->ReParent(currentBranch.top());
				currentNode = nullptr;
				break;
			}
		}
		while (currentBranch.size() > 1);

		if (currentNode)
		{
			currentNode->Destroy();
		}

		return file.ReachEOF() == false;
	}

	// return if node is completed (have no more children to load)
	EBranchManip World::LoadNode(LevelFile& file, SceneNode* node)
	{
		while (ReadWord(file))
		{
			switch (ConstexprCustomHash(file.wordBuffer.c_str()))
			{
			case ConstexprCustomHash("entity"):
				AddEntityLookup(file, node);
				break;
			case ConstexprCustomHash("position"):
				node->SetPosition(ReadVector3(file));
				break;
			case ConstexprCustomHash("rotation"):
				node->SetRotation(ReadQuaternion(file));
				break;
			case ConstexprCustomHash("scale"):
				node->SetScale(ReadVector3(file));
				break;
			case ConstexprCustomHash("name"):
				node->SetName(ReadString(file));
				break;
			default:
				break;
			}

			if (file.lastLetter == '/')
			{
				ReadWord(file);
				return file.wordBuffer.empty() ? EBranchManip::QUO : EBranchManip::POP;
			}
			if (file.lastLetter == '>')
				return EBranchManip::PUSH;
		}
		return EBranchManip::POP;
	}

	bool World::ReadWord(LevelFile& file)
	{
		file.wordBuffer.clear();

		while (file.ReachEOF() == false)
		{
			if (isspace(*file.ptr) || ispunct(*file.ptr))
			{
				file.lastLetter = *file.ptr++;
				break;
			}
			file.wordBuffer.append(file.ptr++, 1);
		}

		return file.ReachEOF() == false;
	}

	bool World::LoadComponents(LevelFile& file)
	{
		while (ReadWord(file))
		{
			if (file.wordBuffer.size())
			{
				const Structure* compMeta = (Structure*)Type::Find(file.wordBuffer.c_str());
				if (!compMeta)
				{
					continue;
				}

				const Function* createFunction = compMeta->FindFunction("CreateComponent");
				void* comp = *(void**)createFunction->Invoke(nullptr).Data;

				LoadStructure(file, comp, compMeta);

				const Function* constructorFunction = compMeta->FindFunction("Constructor");
				if (constructorFunction)
				{
					constructorFunction->Invoke(comp);
				}


				StructureField entityHandleField = compMeta->FindField("entityHandle");
				EntityHandle entity = *(EntityHandle*)entityHandleField.GetFrom(comp);

				const Function* getHandleFunction = compMeta->FindFunction("GetHandle");
				ComponentHandle component = *(ComponentHandle*)getHandleFunction->Invoke(comp).Data;

				Entity::AddDetail(entity, component);
			}
		}
		return file.ReachEOF();
	}

	bool World::LoadStructure(LevelFile& file, void* structure, const Structure* metaData)
	{
		while (ReadWord(file)
			&& file.lastLetter != '}')
		{
			if (!file.wordBuffer.size()) continue;

			StructureField field = metaData->FindField(file.wordBuffer.c_str());
			if (field.IsValid() == false) continue;


			if (field.type->name.hash == ConstexprCustomHash("EntityHandle"))
			{
				*(EntityHandle*)field.GetFrom(structure) = LookupEntity(file);
			}
			else if (field.type->name.hash == ConstexprCustomHash("ComponentHandle"))
			{
				// todo: implement component lookup
				std::string message("attempt to deserialize a stall component handle");
				LOG(LOG_WARNING, message);
			}
			else if (field.type->name.hash == ConstexprCustomHash("std::string")
				|| field.type->name.hash == ConstexprCustomHash("string"))
			{
				*(std::string*)field.GetFrom(structure) = ReadString(file);
			}
			else if (field.type->category == ETypeCategory::STRUCTURE)
			{
				LoadStructure(file, field.GetFrom(structure), (Structure*)field.type);
			}
			else
			{
				file.ptr++; // skip '{'

				memcpy(field.GetFrom(structure), file.ptr, field.type->size);
				file.ptr += field.type->size;

				file.ptr++; // skip '}'
			}
		}

		return file.ReachEOF() == false;
	}

	std::string World::ReadString(LevelFile& file)
	{
		std::string str;

		int size = *(int*)file.ptr;
		file.ptr += sizeof(int);

		file.ptr++; // skip '{'
		str.append(file.ptr, size);
		file.ptr += size;
		file.ptr++; // skip '}'

		return str;
	}

	LibMath::Vector3 World::ReadVector3(LevelFile& file)
	{
		LibMath::Vector3 vector3;

		file.ptr++; // skip '{'
		memcpy(&vector3, file.ptr, sizeof(vector3));
		file.ptr += sizeof(vector3);
		file.ptr++; // skip '}'

		return vector3;
	}

	LibMath::Quaternion World::ReadQuaternion(LevelFile& file)
	{
		LibMath::Quaternion quaternion;

		file.ptr++; // skip '{'
		memcpy(&quaternion, file.ptr, sizeof(quaternion));
		file.ptr += sizeof(quaternion);
		file.ptr++; // skip '}'

		return quaternion;
	}

	EntityHandle World::ReadEntityHandle(LevelFile& file)
	{
		int handle;

		file.ptr++; // skip '{'
		memcpy(&handle, file.ptr, sizeof(handle));
		file.ptr += sizeof(handle);
		file.ptr++; // skip '}'

		return EntityHandle(handle);
	}

	void World::AddEntityLookup(LevelFile& file, const SceneNode* node)
	{
		EntityHandle oldValue = ReadEntityHandle(file);
		EntityHandle newValue = node->GetEntity()->GetHandle();

		file.entityLookup.emplace_back(oldValue, newValue);
	}

	EntityHandle World::LookupEntity(LevelFile& file)
	{
		ReadWord(file); // skip "EntityHandle{"
		ReadWord(file); // skip "value="
		EntityHandle oldValue = ReadEntityHandle(file);
		file.ptr++; // skip '}'

		for (Lookup<EntityHandle> lookup : file.entityLookup)
		{
			if (lookup.oldValue == oldValue)
			{
				return lookup.newValue;
			}
		}

		return EntityHandle();
	}
}
