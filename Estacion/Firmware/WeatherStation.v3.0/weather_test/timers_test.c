//#include "timers_test.h"


#define F_CLK_IO 16000000 // Arduino clock frequency 16MHz
// Function declarations
  
int interrupt_every(int seconds);
void goto_sleep();
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
 * Prepares timer1 to raise an interrupt
 * at a fixed frequency
 * PARAMS: 
 *        INT seconds: frequency of the interrupt
 * RETURN: INT status: 0 if OK
 *                     1 if parameters are invalid
 *                      
 */
int interrupt_every(int seconds){
  //Test precondition
  if (seconds < 0 || seconds > 8) return 1;

  // Set timer 1 to CTC mode

  TCCR1B |= (1<<WGM12);

  // Set OCR1A to have an interrupt at desired frequency

  OCR1A = t * (F_CLK_IO/2048) - 1;

  // Enable and unmask TIMER1_COMPA
  TIMSK1 |= (1 <<OCIE1A); //unmask
  SREG |= (1 << 7); // Globally enable interrupts

  // Set prescaler to 1024 and start timer

  TCCR1B |= (1 << CS12) | (1 << CS10);
  
}
/* PROC: goto_sleep
 * Puts the arduino in sleep mode so that an external interrupt will
 * wake it up. 
 */
void goto_sleep(){

}

