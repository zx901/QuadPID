/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
    APM_Airspeed.cpp - airspeed (pitot) driver

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation; either version 2.1
    of the License, or (at your option) any later version.
*/

#include <FastSerial.h>
#include <AP_Common.h>
#include <AP_Airspeed.h>
#include <LowPassFilter.h>

// table of user settable parameters
const AP_Param::GroupInfo AP_Airspeed::var_info[] PROGMEM = {

    // @Param: ENABLE
    // @DisplayName: Airspeed enable
    // @Description: enable airspeed sensor
    // @Values: 0:Disable,1:Enable
    AP_GROUPINFO("ENABLE",    0, AP_Airspeed, _enable),

    // @Param: USE
    // @DisplayName: Airspeed use
    // @Description: use airspeed for flight control
    // @Values: 0:Use,1:Don't Use
    AP_GROUPINFO("USE",    1, AP_Airspeed, _use),

    // @Param: OFFSET
    // @DisplayName: Airspeed offset
    // @Description: Airspeed calibration offset
    // @Increment: 0.1
    AP_GROUPINFO("OFFSET", 2, AP_Airspeed, _offset),

    // @Param: RATIO
    // @DisplayName: Airspeed ratio
    // @Description: Airspeed calibration ratio
    // @Increment: 0.1
    AP_GROUPINFO("RATIO",  3, AP_Airspeed, _ratio),

    AP_GROUPEND
};

// calibrate the airspeed. This must be called at least once before
// the get_airspeed() interface can be used
void AP_Airspeed::calibrate(void (*callback)(unsigned long t))
{
	float sum = 0;
	uint8_t c;
	if (!_enable) {
		return;
	}
	_source->read();
	for (c = 0; c < 10; c++) {
		callback(100);
		sum += _source->read();
	}
	_airspeed_raw = sum/c;
	_offset.set_and_save(_airspeed_raw);
	_airspeed = 0;
}

// read the airspeed sensor
void AP_Airspeed::read(void)
{
	float airspeed_pressure;
	if (!_enable) {
		return;
	}
	_airspeed_raw 		= _filter.apply(_source->read());
	airspeed_pressure 	= max((_airspeed_raw - _offset), 0);
	_airspeed		= sqrt(airspeed_pressure * _ratio);	
}
