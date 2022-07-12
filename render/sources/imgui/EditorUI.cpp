#include "EditorUI.h"

#include <direct.h>
#include <ShlObj.h>

#include <imgui/GameViewport.h>
#include "ConsoleLogs.h"
#include "ImguiImpl.h"
#include "UIDebugWindow.h"
#include "../../../sound/sources/sound/SoundManager.h"
#include "core/GameLoop.h"
#include "core/PoolAllocator.h"
#include "core/ResourceManager.h"
#include "core/ThreadPool.h"
#include "core/scenegraph/SceneGraph.h"
#include "core/ECS/World.h"

#include "imgui/imgui.h"
#include "render/VulkanConstants.h"
#include "render/Light/LightComponent.h"
#include <core/File.h>


#include "UITheme.h"
#include "../../../physic/sources/physic/PhysicsManager.h"
#include "render/Image/Image.h"
#include "render/Material/Material.h"
#include "render/VulkanRenderer/VulkanRenderer.h"

using namespace ImGui;

static bool isFileDragged = false;
static EFileType draggedFileType = EFileType::MODEL;

EditorUI::EditorUI()
{
	std::filesystem::path projectPath = std::filesystem::current_path();

	paths.push_back(projectPath.append("assets/"));

	uiTheme = Core::MemoryPool::Alloc<UITheme>();
}

EditorUI::~EditorUI()
{
	Core::MemoryPool::Free(uiTheme);
}

void EditorUI::Init()
{
	const std::string folderIconPath = "assets_editor/folder_image.png";
	const std::string fileIconPath = "assets_editor/file_image.png";
	const std::string soundFileIconPath = "assets_editor/sound_file_image.png";
	const std::string soundPlayIconPath = "assets_editor/sound_play.png";
	const std::string soundPauseIconPath = "assets_editor/sound_pause.png";
	const std::string levelIconPath = "assets_editor/level_icon.png";

	Render::Image::InitializeImage(folderIconPath);
	Render::Image::InitializeImage(fileIconPath);
	Render::Image::InitializeImage(soundFileIconPath);
	Render::Image::InitializeImage(soundPlayIconPath);
	Render::Image::InitializeImage(soundPauseIconPath);
	Render::Image::InitializeImage(levelIconPath);

	folderIcon = ResourceManager::GetResource<Render::Image>(folderIconPath)->imguiImage;
	fileIcon = ResourceManager::GetResource<Render::Image>(fileIconPath)->imguiImage;
	soundFileIcon = ResourceManager::GetResource<Render::Image>(soundFileIconPath)->imguiImage;
	soundPlayIcon = ResourceManager::GetResource<Render::Image>(soundPlayIconPath)->imguiImage;
	soundPauseIcon = ResourceManager::GetResource<Render::Image>(soundPauseIconPath)->imguiImage;
	levelIcon = ResourceManager::GetResource<Render::Image>(levelIconPath)->imguiImage;

	uiTheme->InitThemeValues();

	for (const Core::Type* type : Core::Type::GetAll())
	{
		if (type->category == Core::ETypeCategory::STRUCTURE
			&& type->name.hash != Core::ConstexprCustomHash("Comp")
			&& type->name.hash != Core::ConstexprCustomHash("Component"))
		{
			const Core::Structure* comp = (Core::Structure*)type;
			if (comp->Inherit("Comp"))
			{
				components.emplace_back(comp);
			}
		}
	}

	const Core::StaticField* initParamsType = components[0]->FindStaticField("initParamsType");
	if (initParamsType)
	{
		const Core::Type* initParams = Core::Type::Find(initParamsType->GetValue<long long>());

		InitializeParams((const Core::Structure*)initParams);
	}
}

void EditorUI::Draw(const int idx)
{
	DockSpaceOverViewport(GetMainViewport());

	//ShowDemoWindow();

	DrawMainMenuBar();

	DrawDebugWindow();

	GameViewport::gameViewport.DrawViewport(idx);
	GameViewport::gameViewport.DrawDebugViewport(idx);

	DrawToolsWindow();

	DrawSceneGraphWindow();

	DrawInspectorWindow();
}

void EditorUI::DrawMainMenuBar()
{
	if (BeginMainMenuBar())
	{
		if (BeginMenu("File"))
		{
			if (MenuItem("New"))
			{
				Core::World::Initialize();
				currentLevelFile = "";
			}
			if (MenuItem("Open", "Ctrl+O"))
			{
				if (Core::World::HasStarted())
					Core::GameLoop::StopWorld();

				LoadProject();
			}
			if (MenuItem("Save", "Ctrl+S"))
			{
				if (currentLevelFile.empty())
				{
					SaveProject();
				}
				else
				{
					Core::World::Save(currentLevelFile.c_str());
				}
			}
			if (MenuItem("Save As.."))
			{
				SaveProject();
			}
			if (MenuItem("Package Project"))
			{
				PackageProject();
			}

			ImGui::EndMenu();
		}

		if (BeginMenu("Edit"))
		{
			if (MenuItem("Refresh Shaders"))
			{
				Render::VulkanRenderer::RefreshShaders();
			}
			if (Checkbox("PhysX debug", &physXDebug))
			{
				Render::VulkanRenderer::TogglePhysXDebug(physXDebug);
			}

			GameViewport::gameViewport.DrawViewportSelectorMenu();

			ImGui::EndMenu();
		}

		if (BeginMenu("UI"))
		{
			const ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs;

			if (ColorEdit4("Text Color", &uiTheme->currentBaseColors.textColor.x, flags))
			{
				uiTheme->UpdateUIColor();
				needStyleUpdate = true;
			}
			if (ColorEdit4("Background Color", &uiTheme->currentBaseColors.backgroundColor.x, flags))
			{
				uiTheme->UpdateUIColor();
				needStyleUpdate = true;
			}
			if (ColorEdit4("Highlight Color 1", &uiTheme->currentBaseColors.highlightColor1.x, flags))
			{
				uiTheme->UpdateUIColor();
				needStyleUpdate = true;
			}
			if (ColorEdit4("Highlight Color 2", &uiTheme->currentBaseColors.highlightColor2.x, flags))
			{
				uiTheme->UpdateUIColor();
				needStyleUpdate = true;
			}
			Separator();
			Text("Presets: ");
			if (Combo("Theme", (int*)(&uiTheme->currentTheme), uiTheme->themeNames, UITheme::NUM_UI_THEMES))
			{
				uiTheme->ChangeUITheme();
				needStyleUpdate = true;
			}
			if (Combo("Brightness", (int*)(&uiTheme->currentBrightness), uiTheme->brightnessNames,
			          UITheme::NUM_BRIGHTNESS_LEVELS))
			{
				uiTheme->ChangeUITheme();
				needStyleUpdate = true;
			}


			ImGui::EndMenu();
		}


		const std::string pausePlayText = !Core::GameLoop::IsPaused() ? "Pause" : "Play";

		if (MenuItem(pausePlayText.c_str(), "F5"))
		{
			//if (!Core::World::HasStarted())
			//{
			//	if (currentLevelFile.empty())
			//	{
			//		SaveProject();
			//	}
			//	else
			//	{
			//		Core::World::Save(currentLevelFile.c_str());
			//	}
			//}

			Core::GameLoop::TogglePause();
		}
		if (Core::World::HasStarted())
		{
			//if (MenuItem("Stop"))
			//{
			//	Core::GameLoop::StopWorld();
			//	Core::World::Initialize(currentLevelFile.c_str());
			//}
		}

		Separator();

		if (MenuItem("Quit", "Alt+F4"))
		{
			exit(EXIT_SUCCESS);
		}

		EndMainMenuBar();
	}
}

void EditorUI::DrawToolsWindow()
{
	Begin("Tools");

	if (BeginTabBar("MyTabBar"))
	{
		if (BeginTabItem("Files Explorer"))
		{
			ExploreFiles();
			EndTabItem();
		}
		if (BeginTabItem("Console log"))
		{
			EndTabItem();

			ConsoleLogs::consoleLogs.Draw("Console log");
		}
		EndTabBar();
	}

	End();
}

void EditorUI::DrawSceneGraphWindow()
{
	Begin("SceneGraph");

	DrawSceneNode(Core::World::GetLevel()->GetRoot());

	End();
}

std::string EditorUI::GetID(Core::SceneNode* node)
{
	return std::to_string(node->GetEntity()->GetHandle().GetValue()).append("_entity");
}

void EditorUI::DrawInspectorWindow()
{
	Begin("Inspector");

	if (currentSelectedNode == nullptr)
	{
		TextWrapped("No asset selected.");
	}
	else if (currentSelectedNode->GetEntity() == nullptr)
	{
		currentSelectedNode = nullptr;
	}
	else
	{
		TextWrapped(currentSelectedNode->GetName().c_str());
		Text("Rename :");
		SameLine();
		char name[255] = "";
		PushID(GetID(currentSelectedNode).c_str());
		if (InputText("", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			currentSelectedNode->SetName(name);
		}
		PopID();

		Separator();

		const auto& localLocation = currentSelectedNode->GetLocalTransform().position;
		const auto& worldLocation = currentSelectedNode->GetWorldTransformNoCheck().position;

		const auto& localScale = currentSelectedNode->GetLocalTransform().scale;

		const auto& localRotation = currentSelectedNode->GetLocalTransform().rotation.EulerAngles();
		const auto& worldRotation = currentSelectedNode->GetWorldTransformNoCheck().rotation.EulerAngles();

		float local[3]{localLocation.x, localLocation.y, localLocation.z};
		float world[3]{worldLocation.x, worldLocation.y, worldLocation.z};

		float scale[3]{localScale.x, localScale.y, localScale.z};

		float localRot[3]{localRotation.x, localRotation.y, localRotation.z};
		float worldRot[3]{worldRotation.x, worldRotation.y, worldRotation.z};

		TextWrapped("Local transform");
		PushID("local location");
		DrawFieldName("Location");
		if (DragFloat3("", local, INPUT_STEP))
		{
			currentSelectedNode->SetPosition({local[0], local[1], local[2]});
		}
		PopID();
		PushID("local rotation");
		SetNextItemWidth(150);
		DrawFieldName("Rotation");
		if (DragFloat3("", localRot, INPUT_STEP))
		{
			currentSelectedNode->SetRotation({
				LibMath::Degree(localRot[0]), LibMath::Degree(localRot[1]), LibMath::Degree(localRot[2])
			});
		}
		PopID();
		PushID("local scale");
		SetNextItemWidth(150);
		DrawFieldName("Scale");
		if (DragFloat3("", scale, INPUT_STEP))
		{
			currentSelectedNode->SetScale({scale[0], scale[1], scale[2]});
		}
		PopID();
		Separator();

		TextWrapped("World transform");
		PushID("World location");
		DrawFieldName("Location");
		if (DragFloat3("", world, INPUT_STEP))
		{
			currentSelectedNode->SetWorldPosition({world[0], world[1], world[2]});
		}
		PopID();
		PushID("World rotation");
		DrawFieldName("Rotation");
		if (DragFloat3("", worldRot, INPUT_STEP))
		{
			currentSelectedNode->SetWorldRotation({
				LibMath::Degree(worldRot[0]), LibMath::Degree(worldRot[1]), LibMath::Degree(worldRot[2])
			});
		}
		PopID();

		{
			/* Components */
			DrawComponents();

			Separator();

			CreateComponentSection();
		}
	}

	End();
}

void EditorUI::DrawComponents() const
{
	if (currentSelectedNode != nullptr)
	{
		int temp = 0;
		for (auto& detail : currentSelectedNode->GetEntity()->GetAllComponents())
		{
			void* comp = detail->GetComponent();

			Core::Structure* metaData = detail->GetComponentMeta();

			if (CollapsingHeader((metaData->GetName() + "##" + std::to_string(temp)).c_str(),
			                     ImGuiTreeNodeFlags_CollapsingHeader))
			{
				DrawFields(metaData, comp);
				ComponentDestroyButton(detail->GetComponentHandle());
			}

			temp++;
		}
	}
}

void EditorUI::DrawFields(Core::Structure* structure, void* instance) const
{
	for (const auto& field : structure->GetAllFields())
	{
		ImGuiInputTextFlags inputFlags = 0;
		if (field.flags.include(Core::FieldFlag::EDITOR_HIDDEN))
		{
			continue;
		}

		if (field.flags.include(Core::FieldFlag::READONLY))
		{
			inputFlags |= ImGuiInputTextFlags_ReadOnly;
		}
		PushID(field.name->text.c_str());
		switch (field.type->name.hash)
		{
		case Core::ConstexprCustomHash("float"):
			{
				DrawFieldName(field.name->text.c_str());

				float tempFloat = field.GetValue<float>(instance);

				if (DragFloat("", &tempFloat, INPUT_STEP, 0.f, 0.f, "%.3f", inputFlags))
				{
					field.SetValue(instance, tempFloat);
				}
				break;
			}
		case Core::ConstexprCustomHash("int"):
			{
				DrawFieldName(field.name->text.c_str());

				int tempInt = field.GetValue<int>(instance);
				if (DragInt("", &tempInt, 1, 0, 0, "%d", inputFlags))
				{
					field.SetValue(instance, tempInt);
				}
				break;
			}
		case Core::ConstexprCustomHash("bool"):
			{
				DrawFieldName(field.name->text.c_str());

				if (Checkbox("", &field.GetValue<bool>(instance)) && !field.flags.include(
					Core::FieldFlag::READONLY))
				{
				}
				break;
			}
		case Core::ConstexprCustomHash("File"):
		case Core::ConstexprCustomHash("Core::File"):
			{
				DragDropFile(static_cast<File*>(field.GetFrom(instance)));

				break;
			}
		case Core::ConstexprCustomHash("LibMath::Vector2"):
			{
				DrawFieldName(field.name->text.c_str());

				const LibMath::Vector2 vector = field.GetValue<LibMath::Vector2>(instance);
				float tempFloat2[2] = {vector.x, vector.y};
				if (DragFloat2("", tempFloat2, INPUT_STEP, 0, 0, "%.3f", inputFlags))
				{
					field.SetValue(instance, LibMath::Vector2(tempFloat2[0], tempFloat2[1]));
				}
				break;
			}
		case Core::ConstexprCustomHash("LibMath::Vector3"):
			{
				DrawFieldName(field.name->text.c_str());

				LibMath::Vector3& vector = field.GetValue<LibMath::Vector3>(instance);
				if (DragFloat3("", &vector[0], INPUT_STEP, 0, 0, "%.3f", inputFlags))
				{
				}
				break;
			}
		case Core::ConstexprCustomHash("LibMath::Vector4"):
			{
				DrawFieldName(field.name->text.c_str());

				LibMath::Vector4& vector = field.GetValue<LibMath::Vector4>(instance);
				if (ColorEdit4("", &vector[0], ImGuiColorEditFlags_NoInputs))
				{
				}
				break;
			}
		case Core::ConstexprCustomHash("LibMath::Quaternion"):
			{
				DrawFieldName(field.name->text.c_str());

				const LibMath::Vector3 vector = field.GetValue<LibMath::Quaternion>(instance).EulerAngles();
				float tempFloat3[3] = {vector.x, vector.y, vector.z};
				if (DragFloat3("", tempFloat3, INPUT_STEP, 0, 0, "%.3f", inputFlags))
				{
					field.SetValue(instance, LibMath::Quaternion(LibMath::Degree(tempFloat3[0]),
					                                             LibMath::Degree(tempFloat3[1]),
					                                             LibMath::Degree(tempFloat3[2])));
				}
				break;
			}
		case Core::ConstexprCustomHash("std::string"):
			{
				DrawFieldName(field.name->text.c_str());

				std::string string = field.GetValue<std::string>(instance);

				char* text = _strdup(string.c_str());
				if (InputText("", text, IM_ARRAYSIZE(text), inputFlags | ImGuiInputTextFlags_EnterReturnsTrue))
				{
					field.SetValue(instance, std::string(text));
				}
				break;
			}
		case Core::ConstexprCustomHash("UITextPointer"):
			{
				const UITextPointer uiTextPointer = field.GetValue<UITextPointer>(instance);

				char* text = _strdup(uiTextPointer.text->text.c_str());
				if (InputText(field.name->text.c_str(), text, IM_ARRAYSIZE(text),
				              inputFlags | ImGuiInputTextFlags_EnterReturnsTrue))
				{
					uiTextPointer.text->text = text;
				}

				break;
			}
		case Core::ConstexprCustomHash("UIImagePointer"):
			{
				const UIImagePointer uiImagePointer = field.GetValue<UIImagePointer>(instance);

				TextWrapped(uiImagePointer.image->path.c_str());

				auto* iconImage = ResourceManager::GetResource<Render::Image>(uiImagePointer.image->path);
				void* icon;
				if (iconImage == nullptr)
				{
					Core::ThreadPool::defaultThreadPool.AddTask([uiImagePointer]
					{
						ImGuiImpl::LoadImGuiTexture(uiImagePointer.image->path);
					});
					icon = fileIcon;
				}
				else
				{
					icon = iconImage->imguiImage;
				}

				Image(icon, {imageWidth, imageHeight});

				break;
			}
		case Core::ConstexprCustomHash("MaterialPointer"):
			{
				const Render::MaterialPointer materialPointer = field.GetValue<Render::MaterialPointer>(instance);

				if (TreeNode("Materials"))
				{
					int i = 0;
					for (auto* mat : materialPointer.materials)
					{
						std::string materialName = "Name" + i;
						if (!mat)
						{
							PushID(materialName.c_str());
							if (TreeNode("Name: %s", "ERROR_MATERIAL_IS_NULL"))
							{
								TreePop();
								Separator();
							}
							PopID();
						}
						else
						{
							PushID(materialName.c_str());
							if (TreeNode("Name: %s", mat->name.c_str()))
							{
								std::string ambientString = "Ambient Color" + i;
								std::string diffuseString = "Diffuse Color" + i;
								std::string specularString = "Specular Color" + i;
								std::string specularExponentString = "Specular Exponent" + i;
								std::string alphaTreeString = "AlphaTree" + i;
								std::string alphaString = "Alpha" + i;

								PushID(ambientString.c_str());
								if (TreeNode("Ambient"))
								{
									/* AMBIENT */
									LibMath::Vector3 aCol = mat->ambientColor;
									if (ColorEdit3("", &aCol[0]))
										mat->SetAmbientColor(aCol);

									DragDropTexture(mat, Model::MaterialTextureLocation::AMBIENT, i);
									TreePop();
								}
								PopID();

								PushID(diffuseString.c_str());
								if (TreeNode("Diffuse"))
								{
									/* DIFFUSE */
									LibMath::Vector3 dCol = mat->diffuseColor;
									if (ColorEdit3("", &dCol[0]))
										mat->SetDiffuseColor(dCol);

									DragDropTexture(mat, Model::MaterialTextureLocation::DIFFUSE, i);
									TreePop();
								}
								PopID();

								PushID(specularString.c_str());
								if (TreeNode("Specular"))
								{
									/* SPECULAR */
									LibMath::Vector3 sCol = mat->specularColor;
									if (ColorEdit3("", &sCol[0]))
										mat->SetSpecularColor(sCol);

									DragDropTexture(mat, Model::MaterialTextureLocation::SPECULAR, i);

									float sExp = mat->specularExponent;
									PushID(specularExponentString.c_str());
									DrawFieldName("Specular Exponent");
									if (DragFloat("", &sExp, 0.5f, 0.f, 100.f))
										mat->SetSpecularExponent(sExp);
									PopID();

									TreePop();
								}
								PopID();

								PushID(alphaTreeString.c_str());
								if (TreeNode("Alpha"))
								{
									/* ALPHA */
									float a = mat->alpha;
									PushID(alphaString.c_str());
									if (DragFloat("", &a, 0.01f, 0.f, 1.f))
										mat->SetAlpha(a);
									PopID();

									DragDropTexture(mat, Model::MaterialTextureLocation::ALPHA, i);
									TreePop();
								}
								PopID();

								TreePop();
								Separator();
							}
							PopID();
						}
						i++;
					}
					TreePop();
				}

				break;
			}
		default:
			if (field.type->category == Core::ETypeCategory::STRUCTURE)
			{
				DrawFields((Core::Structure*)field.type, field.GetFrom(instance));
			}
			break;
		}
		PopID();
	}
}

void EditorUI::DrawFieldName(const std::string& fieldName)
{
	TextWrapped(fieldName.c_str());
	SameLine(GetContentRegionAvailWidth() / 3);
	SetNextItemWidth(GetContentRegionAvailWidth());
}

void EditorUI::CreateComponentSection()
{
	PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	if (BeginChild("Components Creation", ImVec2(0, 260), true))
	{
		TextWrapped("Components Creation");


		static int compIndex = 0;

		if (BeginCombo("Choose...", components[compIndex]->name.text.c_str(), 0))
		{
			for (auto n = 0; n < components.size(); n++)
			{
				const bool isSelected = compIndex == n;
				if (Selectable(components[n]->name.text.c_str(), isSelected))
				{
					compIndex = n;

					const Core::StaticField* initParamsType = components[compIndex]->FindStaticField("initParamsType");

					if (initParamsType)
					{
						const Core::Type* initParams = Core::Type::Find(initParamsType->GetValue<long long>());

						InitializeParams((const Core::Structure*)initParams);
					}
					else
					{
						delete initParamsBuffer;
						initParamsBuffer = nullptr;
					}
				}

				if (isSelected)
					SetItemDefaultFocus();
			}

			EndCombo();
		}

		if (initParamsBuffer != nullptr)
		{
			const Core::StaticField* initParamsType = components[compIndex]->FindStaticField("initParamsType");
			const Core::Type* initParams = Core::Type::Find(initParamsType->GetValue<long long>());

			DrawFields((Core::Structure*)initParams, initParamsBuffer);
		}

		ComponentCreation(components[compIndex]);
	}
	EndChild();
	PopStyleVar();
}

void EditorUI::ComponentCreation(const Core::Structure* selectedComponent) const
{
	if (Button("Create Component"))
	{
		auto* entity = currentSelectedNode->GetEntity();
		auto* comp = entity->AddComponent(selectedComponent, initParamsBuffer);
		(void)comp;
		(void)entity;
	}
}

void EditorUI::ComponentDestroyButton(const Core::ComponentHandle componentHandle) const
{
	Spacing();

	const std::string id = "button_destruction" + std::to_string(componentHandle.GetValue()) + std::to_string(
		componentHandle.GetType());
	PushID(id.c_str());
	if (Button("Destroy Component"))
	{
		currentSelectedNode->GetEntity()->DestroyComponent(componentHandle);
	}
	PopID();
}

void EditorUI::InitializeParams(const Core::Structure* initParamsType)
{
	initParamsBuffer = initParamsType->New();
}

void EditorUI::DragDropFile(File* instance) const
{
	Render::Image* iconImage;
	void* icon = fileIcon;

	if (instance->fileType == EFileType::IMAGE)
	{
		iconImage = ResourceManager::GetResource<Render::Image>(instance->path);
		if (iconImage == nullptr)
		{
			Core::ThreadPool::defaultThreadPool.AddTask([instance]
			{
				ImGuiImpl::LoadImGuiTexture(instance->path);
			});
			icon = fileIcon;
		}
		else
		{
			icon = iconImage->imguiImage;
		}
	}

	TextWrapped(instance->path.c_str());
	if (!isFileDragged || draggedFileType != instance->fileType)
	{
		Image(icon, {imageWidth, imageHeight});
	}
	else
	{
		Image(icon, {imageWidth, imageHeight}, {0, 0}, {1, 1}, {0, 1, 0, 1});
	}
	if (BeginDragDropTarget())
	{
		const auto* payload = AcceptDragDropPayload("DRAG_DROP_FILE");

		if (payload != nullptr)
		{
			const auto* filePath = static_cast<const char*>(payload->Data);
			std::string formattedFilePath = filePath;
			formattedFilePath = formattedFilePath.substr(0, formattedFilePath.find_last_of(".") + 4);
			if (instance->IsValid(formattedFilePath))
			{
				instance->path = formattedFilePath;
			}
		}
		EndDragDropTarget();
	}
}


void EditorUI::DragDropTexture(Render::Material* mat,
                               Model::MaterialTextureLocation materialTextureLocation, int id) const
{
	void* icon = fileIcon;

	if (mat->HasTexture(materialTextureLocation))
	{
		TextWrapped(mat->GetTextureImage(materialTextureLocation)->imagePath.c_str());
		icon = mat->GetTextureImage(materialTextureLocation)->imguiImage;
	}
	else
	{
		TextWrapped("Unassigned");
	}

	if (!isFileDragged || draggedFileType != EFileType::IMAGE)
	{
		Image(icon, {imageWidth, imageHeight});
	}
	else
	{
		Image(icon, {imageWidth, imageHeight}, {0, 0}, {1, 1}, {0, 1, 0, 1});
	}


	if (BeginDragDropTarget())
	{
		const auto* payload = AcceptDragDropPayload("DRAG_DROP_FILE");

		if (payload != nullptr)
		{
			const auto* filePath = static_cast<const char*>(payload->Data);
			std::string formattedFilePath = filePath;
			formattedFilePath = formattedFilePath.substr(0, formattedFilePath.find_last_of(".") + 4);

			File tempFile(EFileType::IMAGE);

			if (tempFile.IsValid(formattedFilePath))
			{
				auto* image = ResourceManager::GetResource<Render::Image>(formattedFilePath);
				if (image)
				{
					mat->SetTextureImage(image, materialTextureLocation);
				}
				else
				{
					LOG(LOG_ERROR,
					    Core::CLog::FormatString("new texture '%s' not found", formattedFilePath.c_str()).c_str(),
					    Core::ELogChannel::CLOG_RENDER);
				}
			}
		}
		EndDragDropTarget();
	}

	std::string buttonName = "Remove ";

	switch (materialTextureLocation)
	{
	case Model::MaterialTextureLocation::AMBIENT:
		buttonName += "Ambient";
		break;
	case Model::MaterialTextureLocation::DIFFUSE:
		buttonName += "Diffuse";
		break;
	case Model::MaterialTextureLocation::SPECULAR:
		buttonName += "Specular";
		break;
	case Model::MaterialTextureLocation::ALPHA:
		buttonName += "Alpha";
		break;
	default:
		buttonName += "ERROR";
		break;
	}

	buttonName += " Texture##" + id;

	if (Button(buttonName.c_str()))
		mat->SetTextureImage(nullptr, materialTextureLocation);
}

void EditorUI::DrawDebugWindow() const
{
	Begin("Debug");

	FPSCounter();
	Separator();


	if (TreeNode("Benchmark"))
	{
		UIDebugWindow::DrawBenchmarkValues();
		TreePop();
		Separator();
	}


	UIDebugWindow::DrawDebugWindow();

	End();
}

void EditorUI::PackageProject()
{
	Core::ThreadPool::defaultThreadPool.AddTask([]
	{
		TCHAR szDir[MAX_PATH];
		BROWSEINFO bInfo;
		bInfo.hwndOwner = nullptr;
		bInfo.pidlRoot = nullptr;
		bInfo.pszDisplayName = szDir;
		bInfo.lpszTitle = "Please, select a folder";
		bInfo.ulFlags = 0;
		bInfo.lpfn = nullptr;
		bInfo.lParam = 0;
		bInfo.iImage = -1;

		auto* const lpItem = SHBrowseForFolder(&bInfo);
		if (lpItem != nullptr)
		{
			SHGetPathFromIDList(lpItem, szDir);

			std::string finalProjectDir(szDir);
			finalProjectDir.append("\\windows_noeditor\\");

			_mkdir(finalProjectDir.c_str());
			ShellExecute(nullptr, "open", finalProjectDir.c_str(), nullptr, nullptr, SW_SHOWDEFAULT);

			std::string message = "Shipping in ";
			message.append(szDir);
			message.append(" folder.");

			LOG(LOG_INFO, message, Core::ELogChannel::CLOG_EDITOR);

			std::string batCommand = "build.bat ";
			batCommand.append(finalProjectDir);
			std::system(batCommand.c_str());
		}
	});
}

void EditorUI::LoadProject()
{
	auto path = std::filesystem::current_path(); //getting path

	const int BUFSIZE = 1024;
	char buffer[BUFSIZE] = {0};
	OPENFILENAME ofns = {0};
	ofns.lStructSize = sizeof(ofns);
	ofns.lpstrFile = buffer;
	ofns.nMaxFile = BUFSIZE;
	ofns.lpstrTitle = "Open Level";
	ofns.Flags = OFN_FILEMUSTEXIST | OFN_NOREADONLYRETURN;
	GetOpenFileName(&ofns);

	current_path(path); //setting path

	currentLevelFile = std::string(buffer);
	Core::World::Initialize(buffer);
}

void EditorUI::SaveProject()
{
	auto path = std::filesystem::current_path(); //getting path

	const int BUFSIZE = 1024;
	char buffer[BUFSIZE] = {0};
	OPENFILENAME ofns = {0};
	ofns.lStructSize = sizeof(ofns);
	ofns.lpstrFile = buffer;
	ofns.nMaxFile = BUFSIZE;
	ofns.lpstrTitle = "Save Level";
	ofns.Flags = OFN_NOREADONLYRETURN;
	GetOpenFileName(&ofns);

	current_path(path); //setting path

	currentLevelFile = std::string(buffer);
	Core::World::Save(buffer);
}

void EditorUI::ExploreFiles()
{
	DrawExploredDirectories();

	Separator();

	std::vector<std::filesystem::directory_entry> files;

	// Sort out directories from regular files
	for (const auto& file : std::filesystem::directory_iterator(paths[paths.size() - 1]))
	{
		if (is_regular_file(file.path()))
		{
			files.emplace_back(file);
		}
		else
		{
			files.insert(files.begin(), file);
		}
	}

	maxColumns = static_cast<int>(GetWindowSize().x / (imagePadding + imageWidth));
	if (maxColumns == 0)
		maxColumns += 1;

	int imageCount = 0;
	float offset = 10.f;
	for (const auto& file : files)
	{
		const std::string& fileName = file.path().filename().string();
		BeginGroup();

		if (!is_regular_file(file.path())) // directory
		{
			PushID(file.path().string().c_str());
			if (ImageButton(folderIcon, {imageWidth, imageHeight}, {0, 0}, {1, 1}, 0))
			{
				paths.push_back(file.path());
				FreeCurrentSound();
			}
			PopID();
		}
		else // file
		{
			std::string projectRelativePath = file.path().string();
			{
				// get path in project
				std::replace(projectRelativePath.begin(), projectRelativePath.end(), '\\', '/');
				const auto assetStringPos = projectRelativePath.find("assets");
				projectRelativePath = projectRelativePath.substr(assetStringPos);
			}

			auto fileExtension = file.path().extension().generic_string();
			if (fileExtension.find("png") != std::string::npos || fileExtension.find("jpg") != std::string::npos ||
				fileExtension.find("tga") != std::string::npos)
			{
				auto* texture = ResourceManager::GetResource<Render::Image>(projectRelativePath);

				if (texture != nullptr)
				{
					PushID(file.path().string().c_str());
					Image(texture->imguiImage, {imageWidth, imageHeight});
					PopID();
					if (BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						SetDragDropPayload("DRAG_DROP_FILE", projectRelativePath.c_str(), projectRelativePath.size());

						Image(texture->imguiImage, {imageWidth, imageHeight});

						EndDragDropSource();

						isFileDragged = true;
						draggedFileType = EFileType::IMAGE;
					}
				}
				else
				{
					Image(fileIcon, {imageWidth, imageHeight});
					Core::ThreadPool::defaultThreadPool.AddTask([projectRelativePath]
					{
						ImGuiImpl::LoadImGuiTexture(projectRelativePath);
					});
				}
			}
			else if (fileExtension.find("mp3") != std::string::npos || fileExtension.find("wav") != std::string::npos)
			{
				Sound::Sound2D* sound = nullptr;

				PushID(file.path().string().c_str());
				ImVec2 imageLocation = GetCursorPos();
				if (ImageButton(soundFileIcon, {imageWidth, imageHeight}, {0, 0}, {1, 1}, 0))
				{
					Sound::SoundManager::Play2DSound(projectRelativePath, false, true);

					sound = ResourceManager::GetResource<Sound::Sound2D>(projectRelativePath + "noloop");

					if (currentSound != nullptr && sound->GetPath() != currentSound->GetPath())
					{
						FreeCurrentSound();
					}
					currentSound = sound;
				}
				else
				{
					sound = ResourceManager::GetResource<Sound::Sound2D>(projectRelativePath + "noloop");
				}

				if (BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					SetDragDropPayload("DRAG_DROP_FILE", projectRelativePath.c_str(), sizeof(projectRelativePath));
					EndDragDropSource();

					isFileDragged = true;
					draggedFileType = EFileType::SOUND;
				}

				SetCursorPos(imageLocation);
				if (sound && sound->IsPlaying() && !sound->IsPaused())
				{
					Image(soundPauseIcon, {imageWidth, imageHeight});
				}
				else
				{
					Image(soundPlayIcon, {imageWidth, imageHeight});
				}

				PopID();
			}
			else if (fileExtension.find("obj") != std::string::npos || fileExtension.find("fbx") != std::string::npos)
			{
				Image(fileIcon, {imageWidth, imageHeight});
				if (BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					SetDragDropPayload("DRAG_DROP_FILE", projectRelativePath.c_str(), projectRelativePath.size());
					Image(fileIcon, {imageWidth, imageHeight});
					Text(fileName.c_str());
					EndDragDropSource();

					isFileDragged = true;
					draggedFileType = EFileType::MODEL;
				}
			}
			else if (fileExtension.find("lvl") != std::string::npos)
			{
				PushID(file.path().string().c_str());
				if (ImageButton(levelIcon, {imageWidth, imageHeight}))
				{
					Core::World::Initialize(file.path().string().c_str());
				}
				PopID();
			}
			else
			{
				Image(fileIcon, {imageWidth, imageHeight});
			}
		}

		std::string formattedLongMessage = fileName;
		if (fileName.size() > maxCharacters)
		{
			formattedLongMessage = fileName.substr(0, maxCharacters / 2 - 1).append("...").append(
				fileName.substr(fileName.size() - maxCharacters / 2 + 1, fileName.size() - 1));
		}


		Text(formattedLongMessage.c_str());
		EndGroup();

		imageCount++;
		const int mod = imageCount % maxColumns;
		if (mod != 0)
		{
			offset += imageWidth + imagePadding;
			SameLine(offset);
		}
		else
		{
			offset = 10.f;
		}
	}

	if (GetDragDropPayload() == nullptr)
	{
		isFileDragged = false;
	}
	else
	{
		isFileDragged = true;
	}
}

void EditorUI::DrawExploredDirectories()
{
	const float dirPadding = 20.f;
	const float dirHeight = 20.f;

	float offset = 7.f;

	const float windowWidth = GetWindowSize().x;

	for (size_t i = 0; i < paths.size(); i++)
	{
		const std::string dirName = i == 0 ? "Assets" : paths[i].filename().string();

		const float nameLength = static_cast<float>(dirName.size()) * 10;

		if (Button(dirName.c_str(), {nameLength, dirHeight}))
		{
			const size_t nbrToPop = paths.size() - i - 1;

			for (size_t j = 0; j < nbrToPop; j++)
			{
				paths.pop_back();
			}

			FreeCurrentSound();
		}

		if (i + 1 < paths.size())
		{
			offset += dirPadding + nameLength;

			SameLine(offset - dirPadding / 1.66f);
			Text(">");

			if (windowWidth < offset + static_cast<float>(paths[i + 1].filename().string().length() * 10))
			{
				offset = 7.f;
			}
			else
			{
				SameLine(offset);
			}
		}
	}
}

void EditorUI::DrawSceneNode(Core::SceneNode* sceneNode)
{
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth;

	if (currentSelectedNode == sceneNode)
	{
		base_flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (sceneNode->GetChildrenCount() == 0)
	{
		base_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool nodeOpen;

	PushID(GetID(sceneNode).c_str());
	if (sceneNode->GetParent() == nullptr)
	{
		base_flags |= ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf;
		nodeOpen = TreeNodeEx(sceneNode, base_flags, "Root");
	}
	else
	{
		nodeOpen = TreeNodeEx(sceneNode, base_flags, sceneNode->GetName().c_str());

		if (IsItemClicked(ImGuiMouseButton_Left))
		{
			currentSelectedNode = sceneNode;
		}

		if (BeginDragDropSource())
		{
			SetDragDropPayload("_TREENODE", nullptr, 0);
			EndDragDropSource();
		}
	}
	PopID();


	if (IsItemClicked(ImGuiMouseButton_Right))
	{
		OpenPopup(GetID(sceneNode).append("_popup").c_str());
	}

	bool isCreateEntitySelected = false;
	bool isDestroyEntitySelected = false;
	if (BeginPopup(GetID(sceneNode).append("_popup").c_str()))
	{
		if (Selectable("Create Child Entity"))
		{
			isCreateEntitySelected = true;
		}
		if (sceneNode->GetParent() != nullptr && Selectable("Destroy Entity"))
		{
			isDestroyEntitySelected = true;
		}
		EndPopup();
	}

	if (nodeOpen)
	{
		if (sceneNode->GetChildrenCount() > 0)
		{
			for (size_t iNode = 0; iNode < sceneNode->GetChildrenCount(); iNode++)
			{
				DrawSceneNode(sceneNode->GetChildren()[iNode]);
			}

			TreePop();
		}
	}

	if (isCreateEntitySelected)
	{
		sceneNode->CreateChild();
	}
	else if (isDestroyEntitySelected)
	{
		if (sceneNode == currentSelectedNode)
			currentSelectedNode = nullptr;
		if (currentSelectedNode != nullptr && currentSelectedNode->IsDescendantOf(sceneNode))
		{
			currentSelectedNode = nullptr;
		}

		sceneNode->Destroy();
	}
}

void EditorUI::FPSCounter() const
{
	const float fps = GetIO().Framerate;

	std::vector<float>& frames = GameViewport::gameViewport.GetFrames();

	for (size_t i = 1; i < Render::VulkanConstants::fpsGraphSize; i++)
		frames[i - 1] = frames[i];

	frames[Render::VulkanConstants::fpsGraphSize - 1] = fps;

	Text("FPS: %.2f", fps);

	float maxVal = 0;
	const float minVal = 0;

	for (float frame : frames)
	{
		if (maxVal < frame)
			maxVal = frame;
	}


	PlotLines("", &frames[0], static_cast<int>(frames.size()), 0, nullptr, minVal, maxVal,
	          ImVec2(GetWindowContentRegionWidth(), 50));
}

void EditorUI::FreeCurrentSound()
{
	if (currentSound != nullptr)
	{
		Core::MemoryPool::Free(currentSound);
		currentSound = nullptr;
	}
}

void EditorUI::UpdateStyle()
{
	if (!needStyleUpdate)
		return;

	uiTheme->UpdateUITheme();


	needStyleUpdate = false;
}
