#ifndef timers_h
#define timers_h
/* MACRO: prepare_sleep
 * Sets the sleep mode for the board without actually putting it to sleep
 */
#define prepare_sleep()  SMCR = 0b0000;// Set idle mode in sleep mode control register


/* MACRO: goto_sleep
 * Puts the arduino in sleep mode so that a clock or external interrupt will
 * wake it up. 
 */
#define goto_sleep()  SMCR = SMCR | 1; // Enable sleep in sleep mode control register

/* MACRO: wake_up
 * To be the first instruction when an interrupt wakes the device
 */
#define wake_up()   SMCR = SMCR & ~1; // Disable sleep in sleep mode control register
extern void enter_sleep(void);
extern int periodical_interrupt(void);
#endif
