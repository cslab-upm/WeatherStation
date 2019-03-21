#include "timers.h"
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>
#define F_CLK_IO 16000000 // Arduino clock frequency 16MHz
  
/*
 * Timers 1 to 5 are 16-bit timers. Will operate in ctc mode
 * since it won't be used to generate a waveform.
 * CTC mode generates an interrupt with a frequency that follows
 * this formula:
 *  
 *   f_OCnA = (F_CLK_IO)/2*N*(1+OCRnA)
 *
 * Where N is the prescaler factor (1,8,64,256,1024) and
 * OCRnA is the output compare register for timer n.
 * 
 * Since we want to count seconds, we will use 1024 prescaler to
 * reach minimun frequency and maximize the time between interrupts.
 * 
 * Therefore, the time between interupts is defined as
 * 
 *   t = 2048 * (1 + OCRnA) / F_CLK_IO
 *
 * And the value for OCRnA is
 *
 *   OCRnA = t * (F_CLK_IO/2048) - 1
 *
 * In this case, whith F_CLK_IO = 16000000 as default frequency,
 *
 *   OCRnA = t * 7812.5 - 1
 *
 * So, since OCRnA is an integer value, an even number of seconds will
 * work accurately, and since it is a 16-bit value, it won't be possible
 * to have more than 8 seconds between interrupts.
 */

/* PROC: interrupt_every
 * Prepares timer1 to raise an interrupt every 4.09 seconds
 * PARAMS: 
 *        INT seconds: frequency of the interrupt
 * RETURN: INT status: 0 if OK
 *                     1 if parameters are invalid
 *                      
 */
extern int periodical_interrupt(void){
 // Normal timer operation.
  TCCR1A = 0x00; 
  
  // Clear the timer counter register.
  // You can pre-load this register with a value in order to 
  // reduce the timeout period, say if you wanted to wake up
  // ever 4.0 seconds exactly.
  //
  TCNT1=0x0000; 
  
  // Configure the prescaler for 1:1024, giving us a 
  // timeout of 4.09 seconds.
  
  TCCR1B = 0x05;
  
  // Enable the timer overlow interrupt. 
  TIMSK1=0x01; 

}

extern void enter_sleep(void) {
  set_sleep_mode(SLEEP_MODE_IDLE);
  
  sleep_enable();


  /* Disable all of the unused peripherals. This will reduce power
   * consumption further and, more importantly, some of these
   * peripherals may generate interrupts that will wake our Arduino from
   * sleep!
   */
  power_adc_disable();
 // power_spi_disable();
  power_timer0_disable();
 // power_timer2_disable();
  //power_twi_disable();  

  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the timer timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}


