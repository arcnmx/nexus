///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  EventApi.cpp
/// Description  :  Provides functions raise and receive events.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "EventApi.h"

#include "Loader/ArcDPS.h"
#include "Loader/Loader.h"

void CEventApi::Raise(const char* aIdentifier, void* aEventData)
{
	if (aIdentifier == nullptr) { return; }

	const std::lock_guard<std::mutex> lock(this->Mutex);

	auto it = this->Registry.find(aIdentifier);
	
	if (it == this->Registry.end())
	{
		return;
	}

	it->second.AmountRaises++;

	for (EventSubscriber& sub : it->second.Subscribers)
	{
		sub.Callback(aEventData);
	}
}

void CEventApi::Raise(signed int aSignature, const char* aIdentifier, void* aEventData)
{
	if (aIdentifier == nullptr) { return; }

	const std::lock_guard<std::mutex> lock(this->Mutex);

	auto it = this->Registry.find(aIdentifier);

	if (it == this->Registry.end())
	{
		return;
	}

	it->second.AmountRaises++;

	for (EventSubscriber& sub : it->second.Subscribers)
	{
		if (sub.Signature == aSignature)
		{
			sub.Callback(aEventData);
			break;
		}
	}
}

void CEventApi::Subscribe(const char* aIdentifier, EVENT_CONSUME aConsumeEventCallback)
{
	if (aIdentifier == nullptr)           { return; }
	if (aConsumeEventCallback == nullptr) { return; }

	const std::lock_guard<std::mutex> lock(this->Mutex);
	
	EventSubscriber sub{};
	sub.Callback = aConsumeEventCallback;

	/* Resolve addon signature. */
	for (Addon* addon : Loader::Addons)
	{
		if (addon->Module == nullptr ||
			addon->ModuleSize == 0 ||
			addon->Definitions == nullptr ||
			addon->Definitions->Signature == 0)
		{
			continue;
		}

		void* startAddress = addon->Module;
		void* endAddress = ((PBYTE)addon->Module) + addon->ModuleSize;

		if ((void*)aConsumeEventCallback >= startAddress && (void*)aConsumeEventCallback <= endAddress)
		{
			sub.Signature = addon->Definitions->Signature;
			break;
		}
	}

	auto it = this->Registry.find(aIdentifier);

	/* Emplace new event or add subscriber to existing. */
	if (it == this->Registry.end())
	{
		EventData ev{};
		ev.Subscribers.push_back(sub);

		this->Registry.emplace(aIdentifier, ev);
	}
	else
	{
		it->second.Subscribers.push_back(sub);
	}

	// TODO / FIXME
	// Dirty hack to detect ArcDPS below. Do this cleaner later.
	if (ArcDPS::IsLoaded)
	{
		return;
	}

	if (!(strcmp("EV_ARCDPS_COMBATEVENT_LOCAL_RAW", aIdentifier) == 0 || strcmp("EV_ARCDPS_COMBATEVENT_SQUAD_RAW", aIdentifier) == 0))
	{
		return;
	}

	ArcDPS::Detect();
}

void CEventApi::Unsubscribe(const char* aIdentifier, EVENT_CONSUME aConsumeEventCallback)
{
	if (aIdentifier == nullptr) { return; }

	const std::lock_guard<std::mutex> lock(this->Mutex);
	
	auto it = this->Registry.find(aIdentifier);

	if (it == this->Registry.end())
	{
		return;
	}

	it->second.Subscribers.erase(
		std::remove_if(
			it->second.Subscribers.begin(),
			it->second.Subscribers.end(),
			[aConsumeEventCallback](EventSubscriber& sub)
			{
				return aConsumeEventCallback == sub.Callback;
			}
		),
		it->second.Subscribers.end()
	);
}

int CEventApi::Verify(void* aStartAddress, void* aEndAddress)
{
	int refCounter = 0;

	const std::lock_guard<std::mutex> lock(this->Mutex);
	
	for (auto& [identifier, ev] : this->Registry)
	{
		ev.Subscribers.erase(
			std::remove_if(
				ev.Subscribers.begin(),
				ev.Subscribers.end(),
				[&refCounter, aStartAddress, aEndAddress](EventSubscriber& sub)
				{
					if ((void*)sub.Callback >= aStartAddress && (void*)sub.Callback <= aEndAddress)
					{
						refCounter++;
						return true;
					}
					return false;
				}
			),
			ev.Subscribers.end()
		);
	}

	return refCounter;
}

std::unordered_map<std::string, EventData> CEventApi::GetRegistry() const
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	return this->Registry;
}
