// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 11/20/2018 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2018
   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2018
 Copyright 2018 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "DAC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "Systick.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
void MoveRoad(void);
void SysTick_Init(void);
int GetRandom(void);
void MoveObstacles (void);
void PortF_Init(void);
void taxiright(void);
void taxileft(void);
//images
const unsigned short taxi[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98,
 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98,
 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0D98, 0x0D98, 0x109D, 0x109D, 0x0D98,
 0x0D98, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0D98, 0x0D98, 0x109D, 0x109D, 0x0D98, 0x0D98, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0D98, 0x0D98, 0x109D, 0x109D, 0x0D98, 0x0D98, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0D98, 0x0D98, 0x109D, 0x109D, 0x0D98, 0x0D98, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0D98,
 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98,
 0x0D98, 0x0D98, 0x0D98, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98,
 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0D98, 0x0D98, 0xD3CA, 0xD3CA, 0xD3CA, 0xD3CA, 0xD3CA, 0xD3CA, 0xD3CA,
 0xD3CA, 0x0D98, 0x0D98, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0D98, 0x0D98, 0xD3CA, 0xD3CA, 0xD3CA, 0xD3CA, 0xD3CA, 0xD3CA, 0xD3CA, 0xD3CA, 0x0D98, 0x0D98, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98,
 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98, 0x0D98,
 0x0D98, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0xF7BE, 0xF7BE, 0xF7BE, 0xF7BE, 0xF7BE, 0xF7BE, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000

};


const unsigned short cone[] = {
 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x145A, 0x145A, 0x145A, 0x145A, 0x145A, 0x145A, 0x145A, 0xFFFF,
 0xFFFF, 0xFFFF, 0xFFFF, 0xE73D, 0xE73D, 0xE73D, 0xE73D, 0xE73D, 0xE73D, 0xE73D, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x145A,
 0x145A, 0x145A, 0x145A, 0x145A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xE73D, 0xE73D, 0xE73D, 0xE73D, 0xE73D, 0xFFFF,
 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x145A, 0x145A, 0x145A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
 0xFFFF, 0xFFFF, 0x145A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
 0xFFFF, 0xFFFF, 0xFFFF};
const unsigned short whiteline[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xF7BE, 0xF7BE, 0x0000, 0x0000, 0xF7BE, 0xF7BE, 0x0000, 0x0000, 0xF7BE, 0xF7BE, 0x0000,
 0x0000, 0xF7BE, 0xF7BE, 0x0000, 0x0000, 0xF7BE, 0xF7BE, 0x0000, 0x0000, 0xF7BE, 0xF7BE, 0x0000, 0x0000, 0xF7BE, 0xF7BE, 0x0000,
 0x0000, 0xF7BE, 0xF7BE, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


const unsigned short blackline[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};


const unsigned short trafficcone[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C,
 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0B1C, 0x0B1C,
 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C,
 0x0B1C, 0x0B1C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C,
 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D,
 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C,
 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D,
 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x6B6D, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C,
 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0B1C,
 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0B1C, 0x0000, 0x0000, 0x0B1C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0B1C, 0x0B1C, 0x0B1C, 0x0B1C, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};


const unsigned short bear[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2188, 0x2188, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x2188, 0x2188, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2188, 0x2188,
 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x2188, 0x2188, 0x2188, 0x463B, 0x463B, 0x463B, 0x463B, 0x2188, 0x2188, 0x2188, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2188, 0x2188, 0x463B, 0x463B, 0x463B, 0x463B,
 0x463B, 0x463B, 0x2188, 0x2188, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2188,
 0x2188, 0x2188, 0x463B, 0x463B, 0x463B, 0x463B, 0x463B, 0x463B, 0x2188, 0x2188, 0x2188, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x463B, 0x463B, 0x463B, 0x463B, 0x2188, 0x2188, 0x2188,
 0x2188, 0x2188, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188,
 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x2188, 0x2188, 0x2188, 0x463B, 0x463B, 0x463B, 0x463B, 0x2188, 0x2188, 0x2188, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2188, 0x2188, 0x2188, 0x463B, 0x463B, 0x463B, 0x463B, 0x463B, 0x463B,
 0x2188, 0x2188, 0x2188, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2188, 0x2188, 0x2188, 0x2188,
 0x463B, 0x463B, 0x0041, 0x0041, 0x463B, 0x463B, 0x2188, 0x2188, 0x2188, 0x2188, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x463B, 0x463B, 0x463B, 0x463B, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2188, 0x2188, 0x0041, 0x0041, 0x2188, 0x2188, 0x2188, 0x2188,
 0x2188, 0x2188, 0x0041, 0x0041, 0x2188, 0x2188, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2188, 0x2188,
 0x0041, 0xDF3D, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0xDF3D, 0x0041, 0x2188, 0x1189, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0557, 0x0557, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188,
 0x2188, 0x1189, 0x0557, 0x0557, 0x0000, 0x0000, 0x0000, 0x0000, 0x0557, 0x0557, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188,
 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x1189, 0x0557, 0x0557, 0x0000, 0x0000, 0x0000, 0x0000, 0x0557, 0x2188,
 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x2188, 0x1189, 0x0557,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2188, 0x2188, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x2188, 0x2188, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000};


const unsigned short branch[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0E4A, 0x0E4A, 0x0E4A, 0x0E4A, 0x0000, 0x0000, 0x0000, 0x0000, 0x2948,
 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x0000, 0x0000, 0x0E4A, 0x0E4A, 0x0E4A, 0x0E4A, 0x0000, 0x0000,
 0x0000, 0x0000, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x0000, 0x0000, 0x0E4A, 0x0E4A, 0x0E4A,
 0x0E4A, 0x0000, 0x0000, 0x0000, 0x0000, 0x2948, 0x2948, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0E4A, 0x0E4A, 0x0E4A, 0x0E4A, 0x0000, 0x0000, 0x0000, 0x0000, 0x2948, 0x2948, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948,
 0x2948, 0x2948, 0x2948, 0x2948, 0x0000, 0x0000, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948,
 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x0000, 0x0000, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948,
 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x0000, 0x0000, 0x2948, 0x2948, 0x2948, 0x2948,
 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x2948, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0E4A, 0x0E4A, 0x0E4A, 0x0E4A, 0x0E4A,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0E4A, 0x0E4A,
 0x0E4A, 0x0E4A, 0x0E4A, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};


uint8_t  w1=0, w2=20, w3=40, w4=60, w5=80, w6=100, w7=120, w8=140;
uint8_t  m1=0, m2=20, m3=40, m4=60, m5=80, m6=100, m7=120, m8=140;
uint16_t obstacle, lane;
int y=0, x0=10, x1=53, x2=95;
int tx=53, ty=159;
int gameover=0;
int flag=0;
char count=0;
uint32_t ADCMail;
uint32_t ADCStatus=0;
void GameBoard (void){
uint8_t  w1=0, w2=20, w3=40, w4=60, w5=80, w6=100, w7=120, w8=140;
//ST7735_FillScreen(0x0000);            // set screen to black
ST7735_DrawBitmap(tx, ty, taxi, 26,15); // player ship middle bottom
ST7735_DrawBitmap(38,w1, whiteline, 4,11);
ST7735_DrawBitmap(38,w2, whiteline, 4,11);
ST7735_DrawBitmap(38,w3, whiteline, 4,11);
ST7735_DrawBitmap(38,w4, whiteline, 4,11);
ST7735_DrawBitmap(38,w5, whiteline, 4,11);
ST7735_DrawBitmap(38,w6, whiteline, 4,11);
ST7735_DrawBitmap(38,w7, whiteline, 4,11);
ST7735_DrawBitmap(38,w8, whiteline, 4,11);
ST7735_DrawBitmap(80,w1, whiteline, 4,11);
ST7735_DrawBitmap(80,w2, whiteline, 4,11);
ST7735_DrawBitmap(80,w3, whiteline, 4,11);
ST7735_DrawBitmap(80,w4, whiteline, 4,11);
ST7735_DrawBitmap(80,w5, whiteline, 4,11);
ST7735_DrawBitmap(80, w6, whiteline, 4,11);
ST7735_DrawBitmap(80,w7, whiteline, 4,11);
ST7735_DrawBitmap(80,w8, whiteline, 4,11);
//ST7735_DrawBitmap(10, 17,trafficcone,  18,16);
//ST7735_DrawBitmap(53, 17,branch,  17,10);
//ST7735_DrawBitmap(95, 17,bear,  19,19);
}


int main(void){
	DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
	Random_Init(1);
  Output_Init();
	PortF_Init();
	ST7735_SetCursor(1, 1);
  ST7735_OutString("Press Button");
	ST7735_SetCursor(1, 2);
  ST7735_OutString("to Start");
	while ( (GPIO_PORTE_DATA_R &= 0x01)!=0x01)  {}
	Sound_Init();
	ADC_Init();
	SysTick_Init();
Timer0_Init (&MoveRoad, 2000000 );
Timer1_Init(&MoveObstacles, 1000000);
		EnableInterrupts();
	ST7735_FillScreen(0x0000);    	// set screen to black
	GameBoard();
  while(1){
	if(ADCStatus==1){
			if (ADCMail >3000 && ADCMail > 2000){
				taxileft();}
	    if (ADCMail < 1500){
				taxiright();}		
				ADCStatus = 0;}	
	if (gameover ==1){
		DisableInterrupts();	
		ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString("GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString("Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString("You Lose!");
	break;
  }
	if (count==11){
	DisableInterrupts();
	ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString("CONGRATS");
  ST7735_SetCursor(1, 2);
  ST7735_OutString("You Made It");
  ST7735_SetCursor(1, 3);
  ST7735_OutString("To The Airport!!");
	break;
}
}
}

void MoveRoad(void){
w1= w1+20;
ST7735_DrawBitmap(38,w1, whiteline, 4,11);
ST7735_DrawBitmap(80,w1, whiteline, 4,11);
ST7735_DrawBitmap(tx, ty, taxi, 26,15); // player ship middle bottom
	ST7735_SetCursor(1, 1);
	if (count>9){
	ST7735_OutChar(0x31);
	ST7735_SetCursor(2, 1);	
	ST7735_OutChar(0x30); }
	ST7735_OutChar(count+0x30);
}	
	

void MoveObstacles(void){
if (ty==y){	
	if ((lane==0 && tx>-5 && tx <38) || (lane==1 && tx >30 && tx< 80) || (lane==2 && tx> 80 && tx < 120)) {
	if (((((GPIO_PORTE_DATA_R &= 0x02) !=0x02)) || flag==1)){
		gameover=1; } 
	else { 
		flag=1;
	}
	} }
if (y>180 || y==0){
	y=0;
	count++;
obstacle=(Random32()>>24)%60;
lane=(Random32()>>24)%3;}
if (obstacle >=0 && obstacle <=19 && lane==0){
	ST7735_DrawBitmap(x0, y,trafficcone,  20,19);
}
if (obstacle >=0 && obstacle <=19 && lane==1){
	ST7735_DrawBitmap(x1, y,trafficcone,  20,19);
}
if (obstacle >=0 && obstacle <=19 && lane==2){
	ST7735_DrawBitmap(x2, y,trafficcone,  20,19);
}
if (obstacle >19 && obstacle <=39 && lane==0){
	ST7735_DrawBitmap(x0, y,bear,  22,22);}
if (obstacle >19 && obstacle <=39 && lane==1){
	ST7735_DrawBitmap(x1, y,bear,  22,22);}
if (obstacle >19 && obstacle <=39 && lane==2){
	ST7735_DrawBitmap(x2, y,bear,  22,22);
} 
if (obstacle >39 && obstacle <=59 && lane==0){
	ST7735_DrawBitmap(x0, y,branch,  19,14);
}
if (obstacle >39 && obstacle <=59 && lane==1){
	ST7735_DrawBitmap(x1, y,branch,  19,14);
}
if (obstacle >39 && obstacle <=59 && lane==2){
	ST7735_DrawBitmap(x2, y,branch,  19,14);
}
y +=1;
ST7735_DrawBitmap(tx, ty, taxi, 26,15); // player ship middle bottom
}
void PortF_Init(void){
volatile uint32_t delayy;
	SYSCTL_RCGCGPIO_R |= 0x30;				// turn on clock Port F
	delayy = SYSCTL_RCGCGPIO_R;
	delayy = SYSCTL_RCGCGPIO_R;
	delayy = SYSCTL_RCGCGPIO_R;
	delayy = SYSCTL_RCGCGPIO_R;
	delayy = SYSCTL_RCGCGPIO_R;
	delayy = SYSCTL_RCGCGPIO_R;
	GPIO_PORTF_DIR_R |= 0x0E;			 												
	GPIO_PORTF_AFSEL_R = 0;				
	GPIO_PORTF_DEN_R |= 0x0E;	
	GPIO_PORTE_DIR_R &=~0x03;			 												
	GPIO_PORTE_AFSEL_R = 0;				
	GPIO_PORTE_DEN_R |= 0x03;	
}
void SysTick_Init(void){void DisableInterrupts(void);
	NVIC_ST_CTRL_R = 0;		// diable SysTick during intialization
	NVIC_ST_RELOAD_R = 1333333;		// 0.01667 for 60Mhz
	NVIC_ST_CURRENT_R = 0;		// clear current
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000;		// enable interupt	
	NVIC_ST_CTRL_R = 0x07;		
void EnableInterrupts(void);
}


void SysTick_Handler(void){
	
	GPIO_PORTF_DATA_R ^= 0x02;
	GPIO_PORTF_DATA_R ^= 0x04;
	ADCMail = ADC_In();
	ADCStatus = 1;
	GPIO_PORTF_DATA_R ^= 0x08;
	}

void taxiright(void){
	if (tx <100 && tx > -5){
		tx +=1;
	ST7735_DrawBitmap(tx, ty, taxi, 26,15);}
}
void taxileft(void){
	if (tx <120 && tx > 0) {
			tx-=1 ;
	ST7735_DrawBitmap(tx, ty, taxi, 26,15);}
}
	