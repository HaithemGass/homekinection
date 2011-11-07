/*
 * helpers.c
 *
 * Created: 10/11/2011 12:36:32 PM
 *  Author: Brian Bowman
 */ 

#include "helpers.h"

void ledPulseUpdate(){
	static uint16_t intensity = 0;
	static bool ramp = true;
	HAL_SetPwmCompareValue(&pwmChannelRed, (pulseR * intensity)/100);
     HAL_SetPwmCompareValue(&pwmChannelGreen, (pulseG * intensity)/100);
	HAL_SetPwmCompareValue(&pwmChannelBlue, (pulseB* intensity)/100);	
	(ramp) ? (intensity++) : (intensity--);
	if(intensity >= 100 || intensity <= 0 ){
		ramp = !ramp;		
	}
}

void ledBlinkOn(){	
	setLED(pulseR,pulseG,pulseB);
	ledTimer.callback = ledBlinkOff;
	ledTimer.interval = blinkOn;
	ledTimer.mode = TIMER_ONE_SHOT_MODE;
	HAL_StartAppTimer(&ledTimer);
}

void ledBlinkOff(){	
	setLED(LED_COLOR_OFF);
	ledTimer.callback = ledBlinkOn;
	ledTimer.interval = blinkOff;
	ledTimer.mode = TIMER_ONE_SHOT_MODE;
	HAL_StartAppTimer(&ledTimer);
}

void startLEDBlink(uint16_t r, uint16_t g, uint16_t b, uint32_t on, uint32_t off)
{
	pulseB = b;
	pulseG = g;
	pulseR = r;
	blinkOn = on;
	blinkOff = off;
	ledBlinkOn();		
}

void initializeLED()
{	
	//setup pwm
	HAL_OpenPwm(PWM_UNIT_1);			
	pwmChannelBlue.unit = PWM_UNIT_1	;
	pwmChannelBlue.channel  = PWM_CHANNEL_0;
	pwmChannelBlue.polarity = PWM_POLARITY_INVERTED;					
	HAL_SetPwmCompareValue(&pwmChannelBlue, 0);
	
	//setup pwm		
	pwmChannelGreen.unit = PWM_UNIT_1;
	pwmChannelGreen.channel  = PWM_CHANNEL_1;
	pwmChannelGreen.polarity = PWM_POLARITY_INVERTED;								
	HAL_SetPwmCompareValue(&pwmChannelGreen, 0);
	
	//setup pwm	
	pwmChannelRed.unit = PWM_UNIT_1;
	pwmChannelRed.channel  = PWM_CHANNEL_2;
	pwmChannelRed.polarity = PWM_POLARITY_INVERTED;			
	HAL_SetPwmFrequency(PWM_UNIT_1, LED_MAX_BRIGHTNESS , PWM_PRESCALER_8 );				
	HAL_SetPwmCompareValue(&pwmChannelRed, 0);
	
	HAL_StartPwm(&pwmChannelRed);
	HAL_StartPwm(&pwmChannelGreen);
	HAL_StartPwm(&pwmChannelBlue);
	
	setLED(LED_COLOR_OFF);
}

void startLEDPulse(uint16_t r, uint16_t g, uint16_t b, PulseSpeed s)
{	
	HAL_StopAppTimer(&ledTimer);
	pulseR = r;
	pulseG = g;
	pulseB = b;
	ledTimer.callback = ledPulseUpdate;
	ledTimer.interval = s;
	ledTimer.mode = TIMER_REPEAT_MODE;
	HAL_StartAppTimer(&ledTimer);
	
	
}

void stopLEDPulse()
{	
	setLED(LED_COLOR_OFF);
}

void stopLEDBlink()
{	
	setLED(LED_COLOR_OFF);
}

void setLED(uint16_t r, uint16_t g, uint16_t b)
{
	HAL_StopAppTimer(&ledTimer);
	HAL_SetPwmCompareValue(&pwmChannelRed, r*10);
     HAL_SetPwmCompareValue(&pwmChannelGreen, g*10);
	HAL_SetPwmCompareValue(&pwmChannelBlue, b*10);	
}
