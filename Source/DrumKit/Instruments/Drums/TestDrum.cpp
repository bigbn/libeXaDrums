/*
 * Drum.cpp
 *
 *  Created on: 28 Oct 2015
 *      Author: jeremy
 */

#include "TestDrum.h"

namespace DrumKit
{

	TestDrum::TestDrum(InstrumentParameters parameters, std::shared_ptr<Sound::SoundBank> soundBank)
	: Instrument(parameters, soundBank),
	  drumHeadTrigger(nullptr), drumRimTrigger(nullptr),
	  drumHeadSoundId(0), drumRimSoundId(0)
	{


		return;
	}

	TestDrum::~TestDrum()
	{

		return;
	}


	void TestDrum::SetTriggers(std::vector<TriggerPtr>& triggers)
	{

		if(parameters.triggersIdsAndLocations.size() != numTriggers)
		{
			throw -1;
		}

		for(const TriggerPtr triggerPtr : triggers)
		{

			auto triggerIdAndLocation = std::find_if(parameters.triggersIdsAndLocations.cbegin(),
					parameters.triggersIdsAndLocations.cend(),
					[triggerPtr](std::pair<int, TriggerLocation> const& idAndLocation)
					{
						return (idAndLocation.first == triggerPtr->GetId());
					});

			if(triggerIdAndLocation != std::end(parameters.triggersIdsAndLocations))
			{

				TriggerLocation triggerLocation =  (*triggerIdAndLocation).second;

				switch (triggerLocation)
				{
					case TriggerLocation::DrumHead: this->drumHeadTrigger = triggerPtr; break;
					case TriggerLocation::Rim: this->drumRimTrigger = triggerPtr; break;

					default: break;
				}
			}
		}


		return;
	}

	void TestDrum::SetSound(InstrumentSoundInfo const& soundInfo, std::shared_ptr<Sound::SoundProcessor> const& soundProcessor)
	{

		Sound::InstrumentSoundType soundType = soundInfo.type;
		std::string soundLocation = soundInfo.soundLocation;

		switch (soundType)
		{
			case Sound::InstrumentSoundType::RimShot: drumRimSoundId = soundBank->LoadSound(soundLocation); break;
			case Sound::InstrumentSoundType::Default: drumHeadSoundId = soundBank->LoadSound(soundLocation); break;

			default: throw -1; break;
		}


		return;
	}

	void TestDrum::SetVolume(std::shared_ptr<Sound::SoundBank>& soundBank, float volume)
	{

		soundBank->SetSoundVolume(drumRimSoundId, volume);
		soundBank->SetSoundVolume(drumHeadSoundId, volume);

		return;
	}

	bool TestDrum::isTriggerEvent()
	{

		TriggerState headTriggerState = drumHeadTrigger->GetTriggerState();
		TriggerState rimTriggerState = drumRimTrigger->GetTriggerState();


		if(headTriggerState.isTrig || rimTriggerState.isTrig)
		{

			return true;
		}
		else
		{
			return false;
		}

	}

	void TestDrum::GetSoundProps(int& id, float& volume)
	{

		TriggerState headTriggerState = drumHeadTrigger->GetTriggerState();
		TriggerState rimTriggerState = drumRimTrigger->GetTriggerState();


		if(headTriggerState.isTrig)
		{
			if(drumHeadTrigger->GetTriggerState().strength > 0.25f)
			{
				id = drumHeadSoundId;
				volume = headTriggerState.strength;
			}
			else
			{
				id = drumRimSoundId;
				volume = rimTriggerState.strength;
			}
		}

		return;
	}
	// PRIVATE

	/*void Drum::GenerateSounds()
	{

		std::function<void(InstrumentSoundInfo)> genSounds = [this](InstrumentSoundInfo soundInfo)
		{

			std::vector<short> soundData;
			unsigned int soundDuration;

			// Load sound
			//Sound::SoundBank::LoadSound(soundInfo.soundLocation, soundData, soundDuration);

			switch (soundInfo.type)
			{
				case Sound::InstrumentSoundType::Default:

						int id;
						//soundProcessor->AddSound(id, soundData);

						// Internal Id = 0 for default sound
						soundIds.insert(std::pair<int, int>(0, id));

					break;
				default:
					break;
			}




		};


		std::for_each(parameters.soundsInfo.cbegin(), parameters.soundsInfo.cend(), genSounds);

		return;
	}
	*/
}


