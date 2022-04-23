#include "HT68F0021.h"

#define	INIT_LED1()			(_pac0 = _pa0 = 0)
#define	SET_LED1(X)			(_pa0 = (X))
#define	INIT_LED2()			(_pac1 = _pa1 = 0)
#define	SET_LED2(X)			(_pa1 = (X))
#define	INIT_LED3()			(_pac7 = _pa7 = 0)
#define	SET_LED3(X)			(_pa7 = (X))
#define	INIT_LED4()			(_pac2 = _pa2 = 0)
#define	SET_LED4(X)			(_pa2 = (X))
#define	INIT_LED5()			(_pac6 = _pa6 = 0)
#define	SET_LED5(X)			(_pa6 = (X))

#define RESET_LEDS()		(_pa0 = _pa1 = _pa7 = _pa2 = _pa6 = 0)

#define	INT_ISR_ENABLE()			(_inte = 1)
#define	INT_ISR_DISABLE()			(_inte = 0)
#define	INT_SET_FLAG()				(_intf = 1)
#define	INT_CLEAR_FLAG()			(_intf = 0)
#define	EMI_ENABLE()			(_emi = 1)
#define	EMI_DISABLE()			(_emi = 0)


#define INIT_SWITCH()          (_pac5 = _papu5 = 1)

unsigned char state;
unsigned char got_intt;
volatile int timer_cnt;
volatile int prev_timer_cnt;
volatile int button_down;
volatile int sleep;

#pragma vector isr_4 @ 0x04
void isr_4(){
	//_delay(65000);
	if(_pa5 == 0) {
		prev_timer_cnt = timer_cnt = -1;
		button_down = 1;
	}/* else if(timer_cnt - prev_timer_cnt > 3 && button_down == 1){
		button_down = 0;
		prev_timer_cnt = 0;
		state = 6;
		got_intt = 1;
		timer_cnt = 0; 
	}*/ else {
		button_down = 0;
		if(sleep == 0){
			got_intt = 1;
		} else {
			sleep = 0;
		}
	    prev_timer_cnt  = timer_cnt = -1;
	}
	
} // external ISR

#pragma vector isr_c @ 0x0C
void isr_c(){
	timer_cnt ++;
	if((timer_cnt >= 300) || ((timer_cnt > 2 && timer_cnt < 10) && (_pa5 == 0))) {
		sleep = 1;
		state = 6;
		got_intt = 1;
		timer_cnt = -1;
	} else {
		button_down = 0;
	}
} // timer isr


void mydelay(unsigned int times){
	while(times--)	_delay(65000);
}

unsigned char leds[5];

void soft_pwm(unsigned int l) {
  	unsigned char i,j;  
  	for(i=0; i<l; i++){
  		for(j=0; j<255; j++){
  	  		SET_LED1(leds[0] > j);
        	SET_LED2(leds[1] > j);
        	SET_LED3(leds[2] > j);
        	SET_LED4(leds[3] > j);
        	SET_LED5(leds[4] > j);
        	if(got_intt != 0) {
        		return;
        	}
  	  		//_delay(65000);
    	}
  	}
}


void main()
{	
	unsigned char leds2[5] = {0,0,0,0,0};
	unsigned char step = 0;
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned char k = 0;

	unsigned char stretch = 0;
	
	unsigned char step5_seq[6] = {36, 42, 49, 17, 10, 4};

	INIT_LED1();
	INIT_LED2();
	INIT_LED3();
	INIT_LED4();
	INIT_LED5();

    //_ints1 = 0; _ints0 = 1;	//rising edge trigger external interrupt
    _ints1 = 1; _ints0 = 1;	//rising edge and falling edge triggers external interrupt

    _intps	= 0;
	INT_CLEAR_FLAG();
  	INT_ISR_ENABLE();
  	EMI_ENABLE();

  	_intf = 0;
  	_inte = 1;
  	_emi = 1;
  	
  	
  	//////////////////////// Timer registers ///////////////////////////////////////////
  	_tmr = 0;
	
	// interrupt registers
	_emi = 1;
	//_tbe = 1;
	_te = 1;
	
	// TMRC register
	_tm1 = 1;
	_tm0 = 0;
	_ton = 0; // disable timer
	_tpsc0 = 1;
	_tpsc1 = 1;
	_tpsc2 = 1;
	

	_ton = 1;
  	//////////////////////// Timer registers ///////////////////////////////////////////
	

	INIT_SWITCH();
	_wdtc = 0xA8;
	//_pas04 = 0;
	//_pawu5 = 0;
	//_inte = 0;
	
	state = 0;
	got_intt = 0;
	prev_timer_cnt = -1;
	button_down = 0;
	timer_cnt = -1;
	sleep = 0;
	RESET_LEDS();
	for(step = 0; step < 5; step ++){
		leds[step] = 0;
	}
	while(1) {
		if(got_intt != 0){
	    	mydelay(20);
	    	state += 1;
	    	if(state > 5) {
	    		state = 0;
	    	}
	    	RESET_LEDS();
	    	for(step = 0; step < 5; step ++){
	    		leds[step] = 0;
	    	}
	    	i = j = 0;
	    	stretch = 1;
	    	got_intt = 0;
	    	// button_down = 0;
	    }
	    switch(state) {
	    	case 0:
	    		RESET_LEDS(); 
	    		break;
	    	case 1:
	    		//soft_pwm(&leds);
	    		//mydelay(2);
	    		SET_LED1(1);
        		SET_LED2(1);
        		SET_LED3(1);
        		SET_LED4(1);
        		SET_LED5(1);
	    		break;
	    	case 2:
	    		stretch = 3;
	    	case 3:
	    	    if(state == 3) {
	    	    	stretch = 1;
	    	    }
	    	    for(k = 0; k < 248; k += 4){
	    			for(i = 0; i < 5; i++) {
	    				leds[i] = k;
	    			}
	    			soft_pwm(stretch);
	    			if(got_intt != 0){
	    				RESET_LEDS();
	    				break;
	    			}
	    		}
	    		for(i = 0; i < 5; i++) {
	    			leds[i] = k;
	    		}
	    		soft_pwm(stretch);
	    		for(k = 255; k > 5; k -= 4){
	    			for(i = 0; i < 5; i++) {
	    				leds[i] = k;
	    			}
	    			soft_pwm(stretch);
	    			if(got_intt != 0){
	    				RESET_LEDS();
	    				break;
	    			}
	    		}
	    		break;
	    	case 4:
	    		for(k = 0; k < 240; k += 10){
	    			leds[i] = k;
	    			if(j != i) {
	    				leds[j] = 255 - k;
	    			}
	    			soft_pwm(1);
	    			if(got_intt != 0){
	    				break;
	    			}
	    		}
	    		if(got_intt != 0){
	    			break;
	    		}
	    			
	    		if(j != i) {
	    			leds[j] = 0;
	    		}
	    		leds[i] = 255;
	    		soft_pwm(100);
	    		j = i++;
	    		if(i >= 5) {
	    			i = 0;
	    		}
	    		break;
	    	case 5:
	    		for(k=0; k < 6; k++) {
	    			j = (step5_seq[k] & 32) >> 5;
	    			for(i = 0; i <= 253; i += 2) {
	    				for(step = 0; step < 5; step ++){
	    					if((step5_seq[k] & (1 << step)) > 0) {
	    						leds[step] = (j == 1) ? i : (255 - i);
	    					}
	    				}
	    				soft_pwm(1);
	    				if(got_intt != 0){
	    					break;
	    				}
	    			}
	    			for(step = 0; step < 5; step ++){
	    				if((step5_seq[k] & (1 << step)) > 0) {
	    					leds[step] = (j == 1) ? 255 : 0;
	    				}
	    			}
	    			soft_pwm(5);
	    			if(got_intt != 0){
	    				break;
	    			}
	    		}
	    		break;
	    }
	}
