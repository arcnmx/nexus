#ifndef SHARED_H
#define SHARED_H

#include <winsock2.h>
#include <Windows.h>
#include <vector>
#include <string>

#include "Loader/AddonVersion.h"
#include "Services/Logging/LogHandler.h"
#include "Services/API/ApiClient.h"
#include "Services/Localization/Localization.h"
#include "Services/Updater/Updater.h"
#include "Services/Textures/TextureLoader.h"
#include "Services/DataLink/DlApi.h"
#include "Events/EventApi.h"
#include "Inputs/RawInput/RawInputApi.h"
#include "Inputs/InputBinds/InputBindHandler.h"
#include "Inputs/GameBinds/GbApi.h"
#include "UI/UiContext.h"

#include "imgui/imgui.h"

extern CApiClient*					RaidcoreAPI;
extern CApiClient*					GitHubAPI;

extern bool							IsGameLaunchSequence;

#endif