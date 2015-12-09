/**************************************************************************
* 			Programm to follow a wall								      *
*			Version 1.0 ao�t 2007				                          *
*			Michael Bonani, Jonathan Besuchet				              *
*									                                      *
**************************************************************************/

/*! \file
 * \brief Follow a wall
 * \section sect1 Introduction
 * With this program, the e-puck will follow a wall.
 * 
 * \section sect_sound2 Playing the demo
 * First of all, move the selector to the position 2 and reset the e-puck.
 * \n The e-puck will now follow the first wall he finds. You can change
 * the side on which the e-puck must follow the wall with the selector.
 *
 * \section sect_sound3 Video of the demo
 * The video of this demo: http://www.youtube.com/watch?v=xaqpoQ_XGbU
 *
 * \author Jonathan Besuchet
 */

#ifndef _WALLFOLLOW_HL
#define _WALLFOLLOW_HL


void run_wallfollow_hl();
void e_send_uart1_char_hl(const char * buff, int length);

#endif
