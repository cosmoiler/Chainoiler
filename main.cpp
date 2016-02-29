/*
 * main.cpp
 *
 *  Created on: 30 июля 2013 г.
 *      Author: Kosmonavt
 */
#include <Arduino.h>
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support

#include "Global/global.h"
#include "EEPROM/EEPROM.h"
#include "TimerOne/TimerOne.h"
#include "Wire/Wire.h"
#include <stdlib.h>
#include "hardware.h"
#include "Scottoiler/Scottoiler.h"
#include "Shell/cmd_shell.h"
#include "main.h"

//#define SCALE_TIMER 10
#define SEC 1000 // 1 мс (период таймера) * 1000 (коэффициент) = 1 сек
#define TIME_DEBUG_OUT 2
//#define CHAIN_ALL 32 // число импульсов "пробега" всей цепи
#define TIME_SP 2*SEC
#define TIME_MEASURE_VOLTAGE 1*SEC
#define TIME_READ_ADT75 1*SEC
#define TIME_WAIT_USER 30*SEC // ждем действий пользователя 30 сек после вкл зажигания
#define TIME_ZUMMER 1*SEC
#define TIME_OUT_DATA 100 // вывод данных неперерывно каждые 100 мсек


#define K_VOLT 0.14//0.18//0.14              // коэфициент делителя (для измерения напряжения сети) (0.14 R1 = 13k, R2 = )
#define V_MAX 5


//enum StateOiler {
//	AUTOMATIC, MANUAL, SETTINGS, OFF
//};
//enum StateOiler state = AUTOMATIC;

//#define VIEW_STATE_SET_ITEMS 5
//enum View {
//	SET_VALVE, SET_ON_SEC, SET_ON_CHAIN, SET_ALL_CHAIN, SET_ALLAUTO
//};
//enum View viewStateSet = SET_VALVE;

uint8_t lastCamShift;
uint8_t prevCamShift = 255;
uint8_t nCamShift;

unsigned long sp;

unsigned int tSP = TIME_SP;
unsigned long duration, durationPrev;
uint16_t nOnWheel = 0;

volatile unsigned int tOnSec;
volatile uint16_t tOnPumpSec;
unsigned int tOutDebug = TIME_DEBUG_OUT;
uint16_t tWaitUser = TIME_WAIT_USER;
uint16_t tZummerOn = TIME_ZUMMER;

uint16_t voltage = 0;

float celsius = 0;

boolean fMeasureVoltage = true;
uint16_t tMeasureVoltage = TIME_MEASURE_VOLTAGE;
boolean fReadADT75 = true;
uint16_t tReadADT75 = TIME_READ_ADT75;
boolean fValve = false; 
boolean fOutDebug = true;
boolean fCalcTime = true;
boolean fParamTrue = false;
//boolean fRedrawDisp = true;
boolean fZummerOn = false;
boolean fOnPump = false; // Флаг: клапан включен N обротов цепи
unsigned char fOutData = false; // Extern: вывод параметров (устанавливается каждые n мсек)
uint8_t tOutData = TIME_OUT_DATA;


boolean ReadADT75() {

	uint16_t regdata;
	uint8_t a,b;
	int temperature;


	if (fReadADT75) {
		tReadADT75 = TIME_READ_ADT75;
		fReadADT75 = false;

		Wire.beginTransmission(ADT75);
		Wire.write(0);
		Wire.endTransmission();

		Wire.requestFrom(ADT75, 2); // Читаем 2 байта
		if (Wire.available()){
			a = Wire.read(); // старший байт MSB
			b = Wire.read(); // младший байт LSB
			regdata = ((uint16_t)(a << 8) | b) >> 4;
			temperature = ((int)regdata/16);
			Scottoiler::instance()->SetTemperature(temperature);
			return true;
		}
	}

}

void SensorSpeed() {
	sp++; // считаем импульсы с датчика скорости
}

void SensorVolt() {
	int sensorValue;

	if (fMeasureVoltage)
	{
		sensorValue = analogRead(VIN);
		voltage = (int)(((sensorValue / 1023.0f * V_MAX) / K_VOLT) * 1000);
		//voltage = sensorValue;
		fMeasureVoltage = false;
	}
}

void TimerInterrupt() {

	if (fValve && ((state == AUTOMATIC) || (state == TIME)))
	{
		if (!tOnSec--) {
			fValve = false;
			fCalcTime = true;
		}
	}

	if (state == TIME)
		if (tOnPumpSec) tOnPumpSec--;

	if (!tMeasureVoltage--) {
		fMeasureVoltage = true;
		tMeasureVoltage = TIME_MEASURE_VOLTAGE;
	}

	if (!tReadADT75--) {
		fReadADT75 = true;
		tReadADT75 = TIME_READ_ADT75;
	}

	//if (fOutData)
		if (!tOutData--) {
			fOutData = true;
			tOutData = TIME_OUT_DATA;
		}


	/*if (!fOutDebug)
		tOutDebug--;

	if (!tOutDebug) {
		tOutDebug = TIME_DEBUG_OUT;
		fOutDebug = true;
	}
	if (!fMainProg) // работает shell
	{
		if (!fCmdShell) //
		{
			if (!tWaitUser--)
				fMainProg = true;
		}
		else
		{
			tWaitUser = TIME_WAIT_USER;
			fCmdShell = false;
		}

	}*/
}

int PercentToInt(int per, int per100) {
	return per * per100 / 100;
}

void eScottoiler() {

	switch (state) {
	case AUTOMATIC:
		if (!fCalcTime && sp / Scottoiler::instance()->GetChainImp() >= 1) { // проверяем есть ли оборот цепи
			Scottoiler::instance()->Update(fOnPump);
			cli();
			sp = 0;
			sei();
			if (fOnPump) {
				cli();
				tOnSec = Scottoiler::instance()->GetOnSec(); //Time_ON_BLOB_OIL;
				sei();
				fValve = true;
				Scottoiler::instance()->On();
				cbi(ZUMMER_PORT, ZUMMER_PIN);
				//nOnWheel++;
			}

		}
		// ВЫКЛЮЧЕНИЕ КЛАПАНА ПО ВРЕМЕНИ
		if (fCalcTime) {
			cli();
			sp = 0;
			sei();
			Scottoiler::instance()->Off();
			//Scott.Update(temp, hum);
			fCalcTime = false;
			fValve = false;
			//digitalWrite(ZUMMER, HIGH);
			sbi(ZUMMER_PORT, ZUMMER_PIN);
		}
		// ВЫКЛЮЧЕНИЕ КЛАПАНА ПО ПРОБЕГУ
		if (fOnPump && !Scottoiler::instance()->getNOnWheel()) {//fOnPump //Scottoiler::instance()->GetState()
				//&& (nOnWheel >= Scottoiler::instance()->GetOnWheel())) {//((sp / Scottoiler::instance()->GetChainImp()) >= Scottoiler::instance()->GetOnWheel())) {
						//>= PercentToInt(Scottoiler::instance()->GetOnWheel(), Scottoiler::instance()->GetAllWheel()))) {
			Scottoiler::instance()->Off();
			fValve = false;
			fOnPump = false;
			Scottoiler::instance()->SetMode(0);
			sbi(ZUMMER_PORT, ZUMMER_PIN);
		}
		// ВКЛЮЧЕНИЕ КЛАПАНА
		if (!fOnPump && !Scottoiler::instance()->getNOffWheel()) {//!fOnPump//(!Scottoiler::instance()->GetState())
				//&& ((sp / Scottoiler::instance()->GetChainImp()) >= Scottoiler::instance()->GetOffWheel())) {
						//>= PercentToInt(Scottoiler::instance()->GetOffWheel(),
						//		Scottoiler::instance()->GetAllWheel()))) {
			cli();
			tOnSec = Scottoiler::instance()->GetOnSec(); //Time_ON_BLOB_OIL;
			sei();
			Scottoiler::instance()->On();
			fValve = true;
			fOnPump = true;
			Scottoiler::instance()->SetMode(1);
			//nOnWheel = 0;
			cbi(ZUMMER_PORT, ZUMMER_PIN);
		}
		break;
	case OFF:
		Scottoiler::instance()->Off();
		fValve = false;
		
		sbi(ZUMMER_PORT, ZUMMER_PIN);
		break;
		
	case TIME:
		if (!(Scottoiler::instance()->GetState()) && (tOnPumpSec == 0)) {
			noInterrupts();
			tOnPumpSec = Scottoiler::instance()->GetPumpPeriod() * SEC;
			tOnSec = Scottoiler::instance()->GetOnSec();
			interrupts();
			Scottoiler::instance()->On();
			
			cbi(ZUMMER_PORT, ZUMMER_PIN);
			fValve = true;
		}
		// ВЫКЛЮЧЕНИЕ КЛАПАНА ПО ВРЕМЕНИ
		if (fCalcTime) {
			Scottoiler::instance()->Off();
			//Scott.Update(temp, hum);
			fCalcTime = false;
			fValve = false;
			
			sbi(ZUMMER_PORT, ZUMMER_PIN);
		}
		break;
	}

}

void SaveParams()
{
	uint16_t AllWheel;
	uint16_t tOnSec;
	uint8_t nWrite;

	if (fSaveParams)
	{
		nWrite = EEPROM.read(0);
		nWrite++;
		EEPROM.write(0, nWrite);
		tOnSec = Scottoiler::instance()->GetOnSec();  // время включенного состояния насоса в мс
		EEPROM.write(2, tOnSec>>8);
		EEPROM.write(1, tOnSec);
		EEPROM.write(3, Scottoiler::instance()->getOnWheel()); // число оборотов когда насос работает

		AllWheel = Scottoiler::instance()->getOffWheel(); // период включения насоса
		//uint8_t *x = (uint8_t *)&AllWheel;
		EEPROM.write(5, AllWheel>>8);
		EEPROM.write(4, AllWheel);
		EEPROM.write(6, state); // сохраняем состояние системы
		EEPROM.write(7, Scottoiler::instance()->GetChainImp());
		EEPROM.write(8, Scottoiler::instance()->GetPumpPeriod());

		fSaveParams = false;

		Beep(100);
	}
}

void ReadParams()
{
	// ЧИТАЕМ НАСТРОЙКИ
	uint16_t xx[] = {EEPROM.read(1), EEPROM.read(2)};
	Scottoiler::instance()->SetOnSec((xx[1]<<8)|xx[0]);

	Scottoiler::instance()->SetOnWheel(EEPROM.read(3));

	uint16_t yy[] = {EEPROM.read(4), EEPROM.read(5)};
	//uint16_t *y = (uint16_t *)&xx;
	Scottoiler::instance()->SetOffWheel((yy[1]<<8)|yy[0]);
	state = (StateOiler) EEPROM.read(6);
	Scottoiler::instance()->SetChainImp(EEPROM.read(7));
	Scottoiler::instance()->SetPumpPeriod(EEPROM.read(8));
}

void Beep(unsigned int ms)
{
	digitalWrite(ZUMMER, LOW);
	delay(ms);
	digitalWrite(ZUMMER, HIGH);
	delay(ms);
}

void setup()
{
	cli();
	cbi(DDR(&SPEEDIN_PORT), SPEEDIN_PIN);
	sbi(DDR(&SCOTTOILER_PORT), SCOTTOILER_PIN);
	sbi(DDR(&ZUMMER_PORT), ZUMMER_PIN);

	cbi(SCOTTOILER_PORT, SCOTTOILER_PIN);
	sbi(ZUMMER_PORT, ZUMMER_PIN);

	state = AUTOMATIC;

	Scottoiler::init();
	Scottoiler::instance()->Setup(SCOTTOILER, 20, 10, 200);

	ReadParams();

	tOnSec = Scottoiler::instance()->GetOnSec();
	tOnPumpSec = Scottoiler::instance()->GetPumpPeriod()*SEC;

	analogReference (DEFAULT);
	voltage = analogRead(VIN);

	attachInterrupt(0, SensorSpeed, FALLING);

	Timer1.initialize(1000); // T = 1 mSec
	Timer1.attachInterrupt(TimerInterrupt);

	sei();

	Wire.begin();

	cmd_process_setup();
	fMainProg = false; // командный процессор
	fCmdShell = false; // юзер не активен
	delay(500);
	if (state != OFF) {
		Beep(100);
		Beep(200);
	}
	sp = 0;
	fOnPump = false;
}

//#define DEBUG

void loop()
{
	ReadADT75();

	SensorVolt();

	cmd_process_loop();

	eScottoiler();

	SaveParams();


#ifdef DEBUG
	Scottoiler::instance()->On();
			delay(50);
			Scottoiler::instance()->Off();
			delay(500);
#endif

}

int main(void)
{
	init();
	setup();
	for (;;) {
		loop();
	}
	return 0;
}

