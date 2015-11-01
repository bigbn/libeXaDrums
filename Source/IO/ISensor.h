/*
 * Sensor.h
 *
 *  Created on: 25 Oct 2015
 *      Author: jeremy
 */

#ifndef SOURCE_IO_ISENSOR_H_
#define SOURCE_IO_ISENSOR_H_

namespace IO
{

	class ISensor
	{

	public:

		virtual short GetData(char channel) = 0;

		virtual ~ISensor() {};

	protected:
	private:

	};

} /* namespace IO */

#endif /* SOURCE_IO_ISENSOR_H_ */