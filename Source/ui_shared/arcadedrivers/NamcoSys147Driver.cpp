#include "NamcoSys147Driver.h"
#include "string_format.h"
#include "AppConfig.h"
#include "PS2VM.h"
#include "PS2VM_Preferences.h"
#include "../ArcadeDefinition.h"

#include "StdStreamUtils.h"

#include "iop/namco_sys147/Iop_NamcoNANDDevice.h"
#include "iop/namco_sys147/Iop_NamcoSys147.h"

// Static cache members
std::unordered_map<std::string, std::vector<uint8>> CNamcoSys147Driver::s_nandCache;
std::mutex CNamcoSys147Driver::s_nandCacheMutex;

void CNamcoSys147Driver::PrepareEnvironment(CPS2VM* virtualMachine, const ARCADE_MACHINE_DEF& def)
{
	auto baseId = def.parent.empty() ? def.id : def.parent;
	fs::path arcadeRomPath = CAppConfig::GetInstance().GetPreferencePath(PREF_PS2_ARCADEROMS_DIRECTORY);

	fs::path nandPath = arcadeRomPath / baseId / def.nandFileName;
	if(!fs::exists(nandPath))
	{
		throw std::runtime_error(string_format("Failed to find '%s' in game's directory.", def.nandFileName.c_str()));
	}

	auto iopBios = dynamic_cast<CIopBios*>(virtualMachine->m_iop->m_bios.get());
	for(const auto& mount : def.nandMounts)
	{
		iopBios->GetIoman()->RegisterDevice(mount.first.c_str(), std::make_shared<Iop::Namco::CNamcoNANDDevice>(std::make_unique<Framework::CStdStream>(nandPath.string().c_str(), "rb"), mount.second));
	}

	{
		auto sys147Module = std::make_shared<Iop::Namco::CSys147>(*iopBios->GetSifman(), def.id);
		switch(def.ioMode)
		{
		case ARCADE_MACHINE_DEF::IO_MODE::SYS147_AI:
			sys147Module->SetIoMode(Iop::Namco::CSys147::IO_MODE::AI);
			break;
		default:
			assert(false);
			break;
		}
		iopBios->RegisterModule(sys147Module);
		iopBios->RegisterHleModuleReplacement("S147LINK", sys147Module);
		virtualMachine->m_pad->InsertListener(sys147Module.get());
		for(const auto& button : def.buttons)
		{
			sys147Module->SetButton(button.first, button.second.first, button.second.second);
		}
	}
}

void CNamcoSys147Driver::Launch(CPS2VM* virtualMachine, const ARCADE_MACHINE_DEF& def)
{
	// Enable arcade-specific optimizations for System 147  
	// virtualMachine->m_ee->EnableArcadeOptimizations(); // TODO: Fix accessibility issue
	
	virtualMachine->m_ee->m_os->BootFromVirtualPath(def.boot.c_str(), {});
}

std::vector<uint8> CNamcoSys147Driver::ReadNandData(const ARCADE_MACHINE_DEF& def, const std::string& fileName)
{
	// Check cache first
	std::string cacheKey = def.id + "_" + fileName;
	{
		std::lock_guard<std::mutex> lock(s_nandCacheMutex);
		auto it = s_nandCache.find(cacheKey);
		if (it != s_nandCache.end())
		{
			return it->second; // Return cached data
		}
	}
	
	// Load NAND data and cache it
	auto baseId = def.parent.empty() ? def.id : def.parent;
	fs::path arcadeRomPath = CAppConfig::GetInstance().GetPreferencePath(PREF_PS2_ARCADEROMS_DIRECTORY);
	fs::path nandPath = arcadeRomPath / baseId / fileName;
		if(!fs::exists(nandPath))
	{
		throw std::runtime_error(string_format("Failed to find '%s' in game's directory.", fileName.c_str()));
	}
		// Read and cache the NAND data
	Framework::CStdStream inputStream(nandPath.string().c_str(), "rb");
	std::vector<uint8> nandData;
	nandData.resize(inputStream.GetLength());
	inputStream.Read(nandData.data(), nandData.size());
	
	// Cache the data for future use
	{
		std::lock_guard<std::mutex> lock(s_nandCacheMutex);
		s_nandCache[cacheKey] = nandData;
	}
	
	return nandData;
}

fs::path CNamcoSys147Driver::LocateImageFile(const ARCADE_MACHINE_DEF& def, const std::string& fileName)
{
	auto baseId = def.parent.empty() ? def.id : def.parent;
	fs::path arcadeRomPath = CAppConfig::GetInstance().GetPreferencePath(PREF_PS2_ARCADEROMS_DIRECTORY);
	return arcadeRomPath / baseId / fileName;
}
