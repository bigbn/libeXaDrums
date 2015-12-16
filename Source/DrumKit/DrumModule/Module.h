/*
 * Drum.h
 *
 *  Created on: 6 May 2015
 *      Author: jeremy
 */

#ifndef RASPIDRUMS_SOURCE_DRUMKIT_MODULE_H_
#define RASPIDRUMS_SOURCE_DRUMKIT_MODULE_H_

#include "../../Sound/SoundProcessor/Curves/Curve.h"
#include "../../Sound/SoundProcessor/Curves/Curves.h"
#include "../../IO/SensorType.h"
#include "../../Sound/SoundProcessor/SoundProcessor.h"

#include "../Instruments/InstrumentParameters.h"
#include "../Instruments/Instrument.h"
#include "../Instruments/Drum.h"

#include <libxml/tree.h>
#include <libxml/xmlmemory.h>

#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include <functional>



namespace DrumKit
{

	class Module
	{

	public:

		Module(std::string dir, IO::SensorType sensorType, std::shared_ptr<Sound::SoundProcessor> const& soundProc);

		void LoadKit(std::string fileLoc);

		void Start();
		void Stop();

		void GetDirectory(std::string& dir) const;

		virtual ~Module();

	private:

		void Run();

		bool GetDrumParams(xmlNode* drumName, std::vector<std::shared_ptr<Instrument>>& instrument);
		void GetDrumCurve(std::string curveName, std::vector<float>& curve);
		Sound::DrumCurve GetCurveType(std::string curveName);
		void AddSound(std::string fileLocation, std::vector<short>& data, unsigned int& duration);

		std::string drumKitName;
		std::string drumKitFolder;

		IO::SensorType sensorType;

		std::string directory;
		std::vector<std::shared_ptr<Instrument>> instruments;

		std::thread playThread;
		std::atomic<bool> isPlay;
		std::shared_ptr<Sound::SoundProcessor> soundProc;


	};

}

#endif /* RASPIDRUMS_SOURCE_DRUMKIT_MODULE_H_ */
