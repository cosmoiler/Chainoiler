#if defined(ARDUINO) && ARDUINO >= 100
//#include <Arduino.h>
#include <avr/io.h>
#else
#include "WProgram.h"
#endif
#include "../Global/global.h"
#include "Scottoiler.h"
#include "../hardware.h"
#include "../main.h"

//extern Scottoiler Scott;//(uint8_t pin, uint8_t userOnSec, uint8_t userOnWheel, uint8_t userAllWheel);
Scottoiler *Scottoiler::_instance = 0;

Scottoiler::Scottoiler() //uint8_t pin, uint8_t userOnSec, uint8_t userOnWheel, uint8_t userAllWheel
{
   /* _OnSec = userOnSec;
    _OnWheel = userOnWheel;

    _AllWheel = userAllWheel;

 /*   _mode = 0;

	_On_N = 2000;
	_Off_M = 50;
	_Sec_K = userK;
	_blobs = userBlobs;*/

  //  digitalWrite(pin, LOW);
  //  this->pin = pin;
}

void Scottoiler::Setup(uint8_t pin, uint8_t userOnSec, uint8_t userOnWheel, uint16_t userOffWheel)
{
	   _OnSec = userOnSec;
	   _OnWheel = userOnWheel;

	   _OffWheel = userOffWheel;
	   _ChainImp = 32;
	   this->_mode = 0;

	 /*   _mode = 0;

		_On_N = 2000;
		_Off_M = 50;
		_Sec_K = userK;
		_blobs = userBlobs;*/

	//pinMode(pin, OUTPUT);
	//digitalWrite(pin, LOW);

	_Non = 0;

	this->pin = pin;
}

void Scottoiler::Update(bool userOnPump)
{
	if (!_nOn) _nOn = _OnWheel;
	if (!_nOff) _nOff = _OffWheel;

	if (userOnPump) _nOn--;
	else
		_nOff--;
}

uint16_t Scottoiler::GetOnSec()
{
    //
    return _OnSec;
}

// Число оборотов колеса когда клапан включен (в об.)
uint8_t Scottoiler::getOnWheel()
{
    return _OnWheel; //_OnWheel * _AllWheel / 100;
}

uint8_t Scottoiler::getNOnWheel()
{
    return _nOn; //_OnWheel * _AllWheel / 100;
}

// Число оборотов колеса когда клапан выключен (в %)
uint16_t Scottoiler::getOffWheel()
{
    return _OffWheel; //_AllWheel - _OnWheel;//100 - GetOnWheel();
}

uint16_t Scottoiler::getNOffWheel()
{
    return _nOff; //_AllWheel - _OnWheel;//100 - GetOnWheel();
}

// период работы клапана (в оборотах колеса)
uint16_t Scottoiler::GetAllWheel()
{
    //
    return 0;//_AllWheel;
}

// состояние клапана (вкл./выкл.)
bool Scottoiler::GetState()
{
    return _fStateValve;
}

uint8_t Scottoiler::GetMode()
{
    return _mode;
}

uint16_t Scottoiler::GetOn_N()
{
	return _On_N;
}

uint16_t Scottoiler::GetOff_M()
{
	return _Off_M;
}

uint8_t Scottoiler::GetPumpPeriod()
{
	return _Sec_Period;
}

uint8_t Scottoiler::GetBlobs()
{
    return _blobs;
}

long int Scottoiler::GetNon()
{
	return _Non;
}

int Scottoiler::GetTemperature()
{
	return _celsius;
}

uint8_t Scottoiler::GetChainImp()
{
	return _ChainImp;
}

void Scottoiler::SetOnSec(uint16_t userOnSec)
{
    _OnSec = userOnSec;
}

void Scottoiler::SetOnWheel(uint8_t userOnWheel)
{
    _OnWheel = userOnWheel;
    _nOn = userOnWheel;
}

void Scottoiler::SetOffWheel(uint16_t userOffWheel)
{
    _OffWheel = userOffWheel;
    _nOff = userOffWheel;
}

void Scottoiler::SetAllWheel(uint16_t userAllWheel)
{
    //_AllWheel = 0;//userAllWheel;
}

void Scottoiler::SetMode(uint8_t userMode)
{
    _mode = userMode;
}

void Scottoiler::SetOn_N(uint16_t userN)
{
	_On_N = userN;
}

void Scottoiler::SetOff_M(uint16_t userM)
{
	_Off_M = userM;
}

void Scottoiler::SetPumpPeriod(uint8_t userK)
{
	_Sec_Period = userK;
}

void Scottoiler::SetBlobs(uint8_t userBlobs)
{
    _blobs = userBlobs;
}

void Scottoiler::SetTemperature(int userCelsius)
{
	_celsius = userCelsius;
}

void Scottoiler::SetChainImp(uint8_t userChainImp)
{
	_ChainImp = userChainImp;
}

void Scottoiler::On()
{
	//digitalWrite(pin, HIGH);
	sbi(SCOTTOILER_PORT, SCOTTOILER_PIN);
	//Beep(800);
    _fStateValve = true;
    _Non++;
}

void Scottoiler::Off()
{
	//digitalWrite(pin, LOW);
	cbi(SCOTTOILER_PORT, SCOTTOILER_PIN);
    _fStateValve = false;
}
