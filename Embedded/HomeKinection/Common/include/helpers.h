/*
 * helpers.h
 *
 * Created: 10/4/2011 7:21:14 PM
 *  Author: Brian Bowman
 */ 


#ifndef HELPERS_H_
#define HELPERS_H_

#include <pwm.h>
#include <appTimer.h>

#define LED_COLOR_OFF 0,0,0

#define LED_COLOR_RED 255,0,0
#define LED_COLOR_GREEN 0,255,0
#define LED_COLOR_BLUE 0,0,255
#define LED_COLOR_WHITE 255,255,200
#define LED_COLOR_YELLOW 235,105,0
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

#define LED_MAX_BRIGHTNESS 2550

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

void ledPulseUpdate();
void ledBlinkOn();
void ledBlinkOff();
void startLEDBlink(uint16_t r, uint16_t g, uint16_t b, uint32_t on, uint32_t off);
void initializeLED();
void startLEDPulse(uint16_t r, uint16_t g, uint16_t b, PulseSpeed s);
void stopLEDPulse();
void stopLEDBlink();
void setLED(uint16_t r, uint16_t g, uint16_t b);


#endif /* HELPERS_H_ */