/*
 * helpers.h
 *
 * Created: 10/4/2011 7:21:14 PM
 *  Author: Brian Bowman
 */ 


#ifndef HELPERS_H_
#define HELPERS_H_


#define LED_COLOR_OFF 0,0,0

#define LED_COLOR_RED 255,0,0
#define LED_COLOR_GREEN 0,255,0
#define LED_COLOR_BLUE 0,0,255
#define LED_COLOR_WHITE 255,255,200
#define LED_COLOR_YELLOW 255,125,0
#define LED_COLOR_PURPLE 110,0,100
#define LED_COLOR_ORANGE 255,30,0
#define LED_COLOR_PINK 255,100,100
#define LED_COLOR_TURQUOISE 0,125,50

#define LED_BLINK_FAST_HEARTBEAT 100, 750
#define LED_BLINK_MEDIUM_HEARTBEAT 100, 1500
#define LED_BLINK_SLOW_HEARTBEAT 100, 5000

#define LED_BLINK_FAST_BLIP 50, 750
#define LED_BLINK_MEDIUM_BLIP 50, 1500
#define LED_BLINK_SLOW_BLIP  50, 5000

#define LED_BLINK_FAST 250, 250
#define LED_BLINK_MEDIUM 500, 500
#define LED_BLINK_SLOW 750, 750

#define LED_BLINK_FAST_LONG_SHORT 500, 250
#define LED_BLINK_MEDIUM_LONG_SHORT 1000, 500
#define LED_BLINK_SLOW_LONG_SHORT 2000, 1000

#define LED_BLINK_FAST_SHORT_LONG 250, 500
#define LED_BLINK_MEDIUM_SHORT_LONG 500, 1000
#define LED_BLINK_SLOW_SHORT_LONG 1000, 2000

HAL_PwmDescriptor_t pwmChannelRed;
HAL_PwmDescriptor_t pwmChannelGreen;
HAL_PwmDescriptor_t pwmChannelBlue;

HAL_AppTimer_t ledTimer;

uint16_t pulseR, pulseG, pulseB;
uint32_t blinkOn, blinkOff;

typedef enum{
	PULSE_FAST = 10,
	PULSE_MEDIUM = 20,
	PULSE_SLOW = 50
}PulseSpeed;

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

void ledBlinkOn();
void ledBlinkOff();

void ledBlinkOn(){
	HAL_StopAppTimer(&ledTimer);
	setLED(pulseR,pulseG,pulseB);
	ledTimer.callback = ledBlinkOff;
	ledTimer.interval = blinkOn;
	ledTimer.mode = TIMER_ONE_SHOT_MODE;
	HAL_StartAppTimer(&ledTimer);
}

void ledBlinkOff(){
	HAL_StopAppTimer(&ledTimer);
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
	HAL_StartPwm(&pwmChannelBlue);
	HAL_SetPwmCompareValue(&pwmChannelBlue, 100);
	
	//setup pwm		
	pwmChannelGreen.unit = PWM_UNIT_1	;
	pwmChannelGreen.channel  = PWM_CHANNEL_1;
	pwmChannelGreen.polarity = PWM_POLARITY_INVERTED;							
	HAL_StartPwm(&pwmChannelGreen);
	HAL_SetPwmCompareValue(&pwmChannelGreen, 100);
	
	//setup pwm	
	pwmChannelRed.unit = PWM_UNIT_1	;
	pwmChannelRed.channel  = PWM_CHANNEL_2;
	pwmChannelRed.polarity = PWM_POLARITY_INVERTED;			
	HAL_SetPwmFrequency(PWM_UNIT_1, LED_MAX_BRIGHTNESS , PWM_PRESCALER_64 );			
	HAL_StartPwm(&pwmChannelRed);
	HAL_SetPwmCompareValue(&pwmChannelRed, 0);
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
	HAL_StopAppTimer(&ledTimer);
}

void setLED(uint16_t r, uint16_t g, uint16_t b)
{
	HAL_SetPwmCompareValue(&pwmChannelRed, r);
     HAL_SetPwmCompareValue(&pwmChannelGreen, g);
	HAL_SetPwmCompareValue(&pwmChannelBlue, b);	
}


#endif /* HELPERS_H_ */