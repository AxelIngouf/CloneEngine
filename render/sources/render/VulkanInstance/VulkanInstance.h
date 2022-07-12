#pragma once
#include <vector>

#include "../VulkanMacros.h"

namespace vk {
    struct LayerProperties;
    struct ExtensionProperties;
    class Device;
	class Instance;
}

namespace Render
{
	struct ExtensionList
	{
		ExtensionList() = default;

		explicit ExtensionList(std::vector<const char*> extensionList) :
			extensions(std::move(extensionList))
		{
		}

		void AddExtension(const std::string_view extension)
		{
			extensions.emplace_back(extension.data());
		}

		void RemoveExtensionAt(const size_t idx)
		{
			// Swap with last then erase
			std::iter_swap(extensions.begin() + idx, extensions.end() - 1);
			extensions.pop_back();
		}

		[[nodiscard]] const char* const* List() const
		{
			return extensions.data();
		}

		[[nodiscard]] size_t Count() const
		{
			return extensions.size();
		}

	private:
		std::vector<const char*> extensions;
	};

	struct CreationParams
	{
		ExtensionList requiredExtensions;
		std::string appName = "Application Name";
		std::string engineName = "Engine Name";
	};

	class VulkanInstance
	{
	public:
        [[nodiscard]] const vk::Instance& Instance() const;

		VulkanInstance() = default;

		bool Initialize(CreationParams& instanceParams);

	private:

		void InitDynamicDispatcherFirstStep();
		void InitDynamicDispatcherSecondStep();

	public:
		void InitDynamicDispatcherThirdStep(vk::Device device);

	private:

		bool EnableValidationLayersSupport();
		bool CheckValidationLayersSupport();

		bool Create();
		bool CheckRequiredExtensionAvailability(const ExtensionList& requiredExtensions);
		void RetrieveExtensionProperties();

		vk::UniqueInstance instance;

		std::vector<vk::ExtensionProperties> extensionProperties;
		std::vector<vk::LayerProperties> layerProperties;

		CreationParams creationParams;
	};
}
