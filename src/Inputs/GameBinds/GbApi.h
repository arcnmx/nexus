///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  GbApi.h
/// Description  :  Provides functions for game InputBinds.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef GBAPI_H
#define GBAPI_H

#include <filesystem>
#include <map>
#include <mutex>

#include "EGameBinds.h"

#include "Events/EventApi.h"
#include "Inputs/InputBinds/InputBind.h"
#include "Inputs/RawInput/RawInputApi.h"
#include "Services/Logging/LogHandler.h"

constexpr const char* CH_GAMEBINDS = "GameBinds";
constexpr const char* EV_UE_KB_CH = "EV_UNOFFICIAL_EXTRAS_KEYBIND_CHANGED";

///----------------------------------------------------------------------------------------------------
/// CGameBindsApi Class
///----------------------------------------------------------------------------------------------------
class CGameBindsApi
{
	public:
	// FIXME: this needs to be moved to an evtconn
	///----------------------------------------------------------------------------------------------------
	/// OnUEInputBindChanged:
	/// 	Receives runtime InputBind updates from Unofficial Extras.
	///----------------------------------------------------------------------------------------------------
	static void OnUEInputBindChanged(void* aData);

	public:
	///----------------------------------------------------------------------------------------------------
	/// ctor
	///----------------------------------------------------------------------------------------------------
	CGameBindsApi(CRawInputApi* aRawInputApi, CLogHandler* aLogger, CEventApi* aEventApi);
	///----------------------------------------------------------------------------------------------------
	/// dtor
	///----------------------------------------------------------------------------------------------------
	~CGameBindsApi();

	///----------------------------------------------------------------------------------------------------
	/// PressAsync:
	/// 	Presses the keys of a game bind.
	///----------------------------------------------------------------------------------------------------
	void PressAsync(EGameBinds aGameBind);

	///----------------------------------------------------------------------------------------------------
	/// ReleaseAsync:
	/// 	Releases the keys of a game bind.
	///----------------------------------------------------------------------------------------------------
	void ReleaseAsync(EGameBinds aGameBind);

	///----------------------------------------------------------------------------------------------------
	/// InvokeAsync:
	/// 	Presses and releases the keys of a game bind.
	/// 	aDuration is the wait time in milliseconds between press and release.
	///----------------------------------------------------------------------------------------------------
	void InvokeAsync(EGameBinds aGameBind, int aDuration);

	///----------------------------------------------------------------------------------------------------
	/// Press:
	/// 	Presses the keys of a game bind.
	///----------------------------------------------------------------------------------------------------
	void Press(EGameBinds aGameBind);

	///----------------------------------------------------------------------------------------------------
	/// Release:
	/// 	Releases the keys of a game bind.
	///----------------------------------------------------------------------------------------------------
	void Release(EGameBinds aGameBind);

	///----------------------------------------------------------------------------------------------------
	/// IsBound:
	/// 	Returns whether a game bind has a InputBind set or not.
	///----------------------------------------------------------------------------------------------------
	bool IsBound(EGameBinds aGameBind);

	///----------------------------------------------------------------------------------------------------
	/// Import:
	/// 	Imports the game binds from an Inputs.xml.
	///----------------------------------------------------------------------------------------------------
	void Import(std::filesystem::path aPath);

	///----------------------------------------------------------------------------------------------------
	/// Get:
	/// 	Gets a game bind.
	///----------------------------------------------------------------------------------------------------
	const InputBind& Get(EGameBinds aGameBind);

	///----------------------------------------------------------------------------------------------------
	/// Set:
	/// 	Sets a game bind.
	///----------------------------------------------------------------------------------------------------
	void Set(EGameBinds aGameBind, InputBind aInputBind, bool aIsRuntimeBind = false);

	///----------------------------------------------------------------------------------------------------
	/// GetRegistry:
	/// 	Returns a copy of the registry.
	///----------------------------------------------------------------------------------------------------
	std::unordered_map<EGameBinds, InputBind> GetRegistry() const;

	private:
	CRawInputApi*                             RawInputApi = nullptr;
	CLogHandler*                              Logger      = nullptr;
	CEventApi*                                EventApi    = nullptr;

	mutable std::mutex                        Mutex;
	std::unordered_map<EGameBinds, InputBind> Registry;

	bool                                      IsReceivingRuntimeBinds;

	///----------------------------------------------------------------------------------------------------
	/// AddDefaultBinds:
	/// 	Adds the default binds, if they don't already exist.
	///----------------------------------------------------------------------------------------------------
	void AddDefaultBinds();

	///----------------------------------------------------------------------------------------------------
	/// Load:
	/// 	Loads the saved game binds from disk.
	///----------------------------------------------------------------------------------------------------
	void Load();

	///----------------------------------------------------------------------------------------------------
	/// Save:
	/// 	Saves the game binds to disk.
	///----------------------------------------------------------------------------------------------------
	void Save();
};

#endif
