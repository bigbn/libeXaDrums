/*
 * Drum.cpp
 *
 *  Created on: 6 May 2015
 *      Author: jeremy
 */


#include "Module.h"

#include "../../IO/Spi.h"
#include "../../Util/Threading.h"

#include "../Triggers/TriggerManager.h"
#include "../Triggers/TriggerFactory.h"
#include "../Kits/KitManager.h"

#include "TrigSound.h"

#include <iostream>
#include <algorithm>

using namespace Sound;

namespace DrumKit
{

	Module::Module(std::string dir, const AlsaParams& alsaParams, std::shared_ptr<Mixer> mixer, std::shared_ptr<Metronome> metro)
	: directory{dir},  kitManager{dir + "Kits/"},  kitId{0},
	  isPlay{false}, soundBank{std::make_shared<SoundBank>(dir)}, mixer{mixer},
	  metronome{metro}, metronomeSoundId{-1}, isMetronomeEnabled{false},
	  recorder{soundBank.get(), alsaParams}
	{

		mixer->SetSoundBank(soundBank);

		// Load Triggers
		LoadTriggers();

		// Load all drum kits
		LoadKits();

		// Select current kit
		SelectKit(kitId);

		// Set recorder
		recorder.SetDirectory(dir);
		recorder.SetMetronomeTimeFunc([&]{ return GetLastClickTime(); });

		return;
	}

	Module::~Module()
	{
		isPlay.store(false);

		// Join all threads
		if(playThread.joinable())
		{
			playThread.join();
		}
	}

	void Module::Start()
	{

		isPlay.store(true);
		playThread = std::thread(&Module::Run, this);

		// Set high priority to the thread
//		int maxPriority = sched_get_priority_max(SCHED_FIFO);
//
//		if(maxPriority > 1)
//		{
//			maxPriority--;
//		}
//
//		sched_param sch_params;
//		sch_params.sched_priority = maxPriority;
//
//		pthread_setschedparam(playThread.native_handle(), SCHED_FIFO, &sch_params);

		Util::SetThreadPriority(playThread.native_handle(), 99);

		return;
	}


	void Module::Stop()
	{

		isPlay.store(false);
		playThread.join();

		mixer->Clear();

		return;
	}

	void Module::EnableRecording(bool record)
	{
		if(record)
		{
			recorder.Start();
		}
		else
		{
			recorder.Stop();
		}
	}

	void Module::RecorderExport(const std::string& fileName)
	{
		recorder.Export(fileName);
	}

	void Module::GetDirectory(std::string& dir) const
	{

		dir = this->directory;

		return;
	}


	void Module::SelectKit(std::size_t id)
	{

		if(id > kits.size())
		{
			std::cerr << "Error: there is no kit with the id " << id << std::endl;
			throw -1;
		}

		// Disable previous kit
		kits[kitId].Disable();

		// Clear sound bank and mixer
		mixer->Clear();
		soundBank->Clear();

		// Update kit id
		kitId = id;

		// Enable new kit
		kits[kitId].Enable();

		if(isMetronomeEnabled.load())
		{
			EnableMetronome(true);
		}


		return;
	}

	std::vector<std::string> Module::GetKitsNames() const
	{

		std::vector<std::string> names;
		std::transform(kits.cbegin(), kits.cend(), std::back_inserter(names), [](const Kit& k) { return k.GetName(); });

		return names;
	}

	std::vector<std::string> Module::GetInstrumentsNames() const
	{

		const std::vector<InstrumentPtr>& instruments = kits[kitId].GetInstruments();

		std::vector<std::string> names;
		std::transform(instruments.cbegin(), instruments.cend(), std::back_inserter(names), [](const InstrumentPtr i) { return i->GetName(); });

		return names;
	}


	bool Module::DeleteKit(const int& id)
	{

		bool isSuccess = kitManager.DeleteKit(id);

		if(isSuccess)
		{
			LoadKits();
		}

		return isSuccess;
	}

	void Module::ReloadKits()
	{

		kitManager.ReScan();
		LoadKits();

		return;
	}

	void Module::ReloadTriggers()
	{

		LoadTriggers();

		return;
	}

	void Module::EnableMetronome(bool enable)
	{

		if(enable)
		{
			metronome->GenerateClick();
			std::vector<short> data = metronome->GetData();

			metronomeSoundId = soundBank->AddSound(data, 0.5f);
			soundBank->LoopSound(metronomeSoundId, true);
			mixer->PlaySound(metronomeSoundId, 1.0f);

			isMetronomeEnabled.store(true);
		}
		else
		{

			if(metronomeSoundId != -1)
			{
				// Stop metronome sound
				mixer->StopSound(metronomeSoundId);
				soundBank->DeleteSound(metronomeSoundId);
			}

			isMetronomeEnabled.store(false);
		}


		return;
	}

	void Module::ChangeTempo(int tempo)
	{

		metronome->SetTempo(tempo);
		RestartMetronome();

		return;
	}

	void Module::ChangeVolume(int volume)
	{

		if(isMetronomeEnabled.load())
		{
			soundBank->SetSoundVolume(metronomeSoundId, float(volume / 100.0f));
		}

		return;
	}

	float Module::GetClickVolume() const
	{
		return soundBank->GetSound(metronomeSoundId).GetVolume();
	}


	double Module::GetClickPosition() const
	{

		if(isMetronomeEnabled.load())
		{
			unsigned long pos = soundBank->GetSound(metronomeSoundId).LoadIndex();
			double relPos = double(pos) / double(soundBank->GetSound(metronomeSoundId).GetLength());
			//fraction -= int(fraction);

			return relPos;
		}
		else
		{
			return 0.0f;
		}
	}

	int64_t Module::GetLastClickTime() const
	{

		if(isMetronomeEnabled.load())
		{
			return soundBank->GetSound(metronomeSoundId).GetLastStartTime();
		}
		else
		{
			return 0;
		}
	}

	void Module::RestartMetronome()
	{

		if(isMetronomeEnabled.load())
		{
			EnableMetronome(false);
			EnableMetronome(true);
		}

		return;
	}


	// PRIVATE METHODS

	void Module::LoadKits()
	{

		// Clear kits list
		kits.clear();

		// Get all kits locations
		std::vector<std::string> kitsPaths = kitManager.GetKitsLocations();

		std::transform(kitsPaths.cbegin(), kitsPaths.cend(), std::back_inserter(kits), [this](std::string const& kitPath)
		{
			KitParameters kitParams;
			KitManager::LoadKit(kitPath, kitParams);

			return Kit(kitParams, this->triggers, this->soundBank);
		});

		return;
	}

	void Module::LoadTriggers()
	{

		// Update sensors configuration
		TriggerManager::LoadSensorsConfig(this->directory, this->sensorsConfig);

		if(sensorsConfig.sensorType == IO::SensorType::Spi)
		{
			IO::Spi::get().Close();
			IO::Spi::get().Open(sensorsConfig.samplingRate, 0);
		}


		// Load triggers
		TriggerManager::LoadTriggersConfig(this->directory, sensorsConfig, this->triggersParameters);
		//TriggerManager::SaveTriggersConfig(this->directory, this->triggersParameters);

		// Create Triggers
		this->CreateTriggers(this->triggersParameters);

		return;
	}


	void Module::Run()
	{

		if(isMetronomeEnabled.load())
		{
			mixer->PlaySound(metronomeSoundId, 1.0f);
		}

		lastTrigTime.store(0, std::memory_order_relaxed);
		lastTrigValue.store(0, std::memory_order_relaxed);

		while(isPlay.load())
		{

			// Refresh triggers
			std::for_each(triggers.begin(), triggers.end(), [](TriggerPtr& triggerPtr) { triggerPtr->Refresh(); });

			// Get most recent hit
			auto it = std::max_element(triggers.cbegin(), triggers.cend(),
					[](const TriggerPtr& t1, const TriggerPtr& t2) { return t1->GetTriggerState().trigTime < t2->GetTriggerState().trigTime; });


			int64_t trigTime = (*it)->GetTriggerState().trigTime;
			int64_t prevTrigTime = lastTrigTime.exchange(trigTime, std::memory_order_release);

			if(prevTrigTime != trigTime)
			{
				lastTrigValue.store(int((*it)->GetTriggerState().value * 100.0f), std::memory_order_release);

				//std::cout << double(trigTime - soundBank->GetSound(metronomeSoundId).GetLastStartTime()) / 1000. << std::endl;

			}

			// Check instruments triggers and send sounds to mixer if necessary
			const std::vector<InstrumentPtr>& instruments = kits[kitId].GetInstruments();
			for(const InstrumentPtr& instrumentPtr : instruments)
			{

				bool isTriggerEvent = instrumentPtr->IsTriggerEvent();

				if(isTriggerEvent)
				{
					int soundId = 0;
					float volume = 1.0f;
					instrumentPtr->GetSoundProps(soundId, volume);

					if(recorder.IsRecording(std::memory_order_relaxed))
					{
						recorder.Push(TrigSound{instrumentPtr->GetId(), soundId, trigTime, volume});
					}

					mixer->PlaySound(soundId, volume);
				}
			}
		}

		return;
	}


	void Module::CreateTriggers(const std::vector<TriggerParameters>& triggersParameters)
	{

		triggers.clear();

		for(const TriggerParameters& triggerParameters : triggersParameters)
		{
			TriggerPtr triggerPtr = TriggerFactory::CreateTrigger(triggerParameters);
			triggers.push_back(triggerPtr);
		}

		return;
	}

}

