#include <algorithm>
#include <cstring>
#include "BootableUtils.h"
#include "DiskUtils.h"
#include "ArcadeDiskCache.h"
#include "OpticalMedia.h"

bool BootableUtils::IsBootableExecutablePath(const fs::path& filePath)
{
	auto extension = filePath.extension().string();
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	return (extension == ".elf");
}

bool BootableUtils::IsBootableDiscImagePath(const fs::path& filePath)
{
	const auto& supportedExtensions = DiskUtils::GetSupportedExtensions();
	auto extension = filePath.extension().string();
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	auto extensionIterator = supportedExtensions.find(extension);
	return extensionIterator != std::end(supportedExtensions);
}

bool BootableUtils::IsBootableArcadeDefPath(const fs::path& filePath)
{
	auto extension = filePath.extension().string();
	return (extension == ".arcadedef");
}

BootableUtils::BOOTABLE_TYPE BootableUtils::GetBootableType(const fs::path& filePath)
{
	auto extension = filePath.extension().string();
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	if(extension == ".elf")
		return BootableUtils::BOOTABLE_TYPE::PS2_ELF;
	if(extension == ".arcadedef")
		return BootableUtils::BOOTABLE_TYPE::PS2_ARCADE;

	if(BootableUtils::IsBootableDiscImagePath(filePath))
	{
		try
		{
			// Optimization: Use cached disk validation for better performance
			std::string cachedDiskId;
			if(CArcadeDiskCache::GetInstance().GetCachedDiskId(filePath, &cachedDiskId))
			{
				return BootableUtils::BOOTABLE_TYPE::PS2_DISC;
			}
			
			// Fallback to full validation if not cached
			auto opticalMedia = DiskUtils::CreateOpticalMediaFromPath(filePath, COpticalMedia::CREATE_AUTO_DISABLE_DL_DETECT);
			auto fileSystem = opticalMedia->GetFileSystem();
			auto systemConfigFile = std::unique_ptr<Framework::CStream>(fileSystem->Open("SYSTEM.CNF;1"));
			if(!systemConfigFile) return BootableUtils::BOOTABLE_TYPE::UNKNOWN;

			auto systemConfig = DiskUtils::ParseSystemConfigFile(systemConfigFile.get());			if(auto bootItemIterator = systemConfig.find("BOOT2"); bootItemIterator != std::end(systemConfig))
			{
				// Cache the result for future use
				std::string diskId;
				if(DiskUtils::TryGetDiskId(filePath, &diskId))
				{
					CArcadeDiskCache::GetInstance().CacheDiskId(filePath, diskId);
				}
				return BootableUtils::BOOTABLE_TYPE::PS2_DISC;
			}
		}
		catch(const std::exception&)
		{
		}
	}
	return BootableUtils::BOOTABLE_TYPE::UNKNOWN;
}
