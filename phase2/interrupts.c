/* interrupts.c */

/* determine the line the interrupt is on */
/* don't worry about line 0 
   line 1-2 are clocks
   line 3 = disk device
   line4 = tape device
   line 5 = network device
   line 6 = printer device
   line 7 = terminal device 
*/

/* lower line number = higher priority 
given line 3-7, where is the interrupt?
will have the line number and the device number, and should be able to figure it out
*/


