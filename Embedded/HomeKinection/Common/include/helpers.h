/*
 * helpers.h
 *
 * Created: 10/4/2011 7:21:14 PM
 *  Author: Brian Bowman
 */ 


#ifndef HELPERS_H_
#define HELPERS_H_

HAL_PwmDescriptor_t pwmChannelRed;
HAL_PwmDescriptor_t pwmChannelGreen;
HAL_PwmDescriptor_t pwmChannelBlue;


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

void setLED(uint16_t r, uint16_t g, uint16_t b)
{
	HAL_SetPwmCompareValue(&pwmChannelRed, r);
     HAL_SetPwmCompareValue(&pwmChannelGreen, g);
	HAL_SetPwmCompareValue(&pwmChannelBlue, b);	
}


#endif /* HELPERS_H_ */