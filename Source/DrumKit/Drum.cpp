/*
 * Drum.cpp
 *
 *  Created on: 28 Oct 2015
 *      Author: jeremy
 */

#include "Drum.h"

namespace DrumKit
{

	Drum::Drum(int id, int sensorId, IO::SensorType sensorType, std::shared_ptr<Sound::Mixer> const& mixer)
	: mixer(mixer),
	  id(id),
	  sensorId(sensorId),
	  sensorType(sensorType),
	  scanTime(0),
	  threshold(0),
	  maskTime(0),
	  sensor(nullptr),
	  trigger(nullptr)
	{


		// Create sensor


		const std::string sensorLoc("/home/jeremy/Desktop/Prog/eXaDrums/eXaDrums/out.raw");

		switch(sensorType)
		{

		case IO::SensorType::Hdd:

			this->sensor = std::shared_ptr<IO::ISensor>(new IO::HddSensor(sensorLoc.c_str()));

			break;

		}

		return;
	}

	Drum::~Drum()
	{

		return;
	}


	void Drum::CreateTrigger()
	{

		trigger = std::shared_ptr<Trigger>(new Trigger(id, scanTime, threshold, maskTime, curve, mixer));

		return;
	}

	void Drum::Trig()
	{

		short value = this->sensor->GetData(0);

		this->trigger->Trig(value);

		return;
	}

	void Drum::SetDrumName(std::string drumName)
	{

		this->drumName = drumName;

		return;
	}

	void Drum::SetSoundFile(std::string soundFile)
	{

		this->soundFile = soundFile;

		return;
	}

	void Drum::SetCurve(std::vector<float> curve)
	{

		this->curve = curve;

		return;
	}

	void Drum::SetScanTime(unsigned int scanTime)
	{

		this->scanTime = scanTime;

		return;
	}

	void Drum::SetThreshold(short threshold)
	{

		this->threshold = threshold;

		return;
	}

	void Drum::SetMaskTime(int maskTime)
	{

		this->maskTime = maskTime;

		return;
	}

}

