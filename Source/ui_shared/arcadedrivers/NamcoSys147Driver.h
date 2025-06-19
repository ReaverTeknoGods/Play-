#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>
#include <cstdint>
#include "Types.h"
#include "filesystem_def.h"
#include "ArcadeDriver.h"

class CNamcoSys147Driver : public CArcadeDriver
{
public:
	void PrepareEnvironment(CPS2VM*, const ARCADE_MACHINE_DEF&) override;
	void Launch(CPS2VM*, const ARCADE_MACHINE_DEF&) override;

private:
	std::vector<uint8> ReadNandData(const ARCADE_MACHINE_DEF&, const std::string&);
	fs::path LocateImageFile(const ARCADE_MACHINE_DEF&, const std::string&);
	
	// Cache for NAND data to avoid repeated file access
	static std::unordered_map<std::string, std::vector<uint8>> s_nandCache;
	static std::mutex s_nandCacheMutex;
};
