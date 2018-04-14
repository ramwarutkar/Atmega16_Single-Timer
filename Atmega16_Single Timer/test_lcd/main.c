#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
volatile uint8_t tot_overflow;
#include "nokia5110.h"

// ******************************************* TIMER DECLARATION UNO *********************************
int sec_counter = 0;

ISR(TIMER1_OVF_vect)
{
	tot_overflow++;
	TCNT1  = 64560;
}
// ***************************** Functions  ***********************************
void intro(void);
void hours_clear(void);
void hours_switch(void);
void hours_clear_arrow(void);
void min_clear(void);
void min_switch(void);
void min_clear_arrow(void);
void sec_clear(void);
void sec_switch(void);
void sec_clear_arrow(void);
void roll(void);

void timerdeclaration(void);
void reverse_timer(void);

void on_off(void); 

// ************************************** Print Variable  ***************************

static unsigned int sec_print = 0 ;
static unsigned char sec_buf[7];
static unsigned int r_sec = 0;
static unsigned char r_sec_buf[7];
static unsigned int min_print = 0 ;
static unsigned char min_buf[7];
static unsigned int  r_min = 0 ;
static unsigned char r_min_buf[7];
static unsigned int hours_print = 0 ;
static unsigned char hours_buf[7];
static unsigned int r_hours= 0 ;
static unsigned char r_hours_buf[7] ;
         
// ******************************************** Variables  *******************************

static int set_count = 1;    // For counting seconds 
static int enable_switch = 0;

int main(void)
{
    DDRD = 0b00000001;   // Output Port PIN 1(P0)
	DDRA = 0b00000000;   // Pushbutton Input Port
	PORTA= 0b00011110;  // PINA 1 = UP || PINA 2 = DOWN || PINA 3 = SET || ON/OFF Switch 
	
	timerdeclarations();  // Initialization of Timer
	sei();
	nokia_lcd_init();     // Display Initialization
    
	intro();  
   
   while (1)
   {
      
	  if(set_count == 1)                 // For Setting Seconds
	  {
	     do 
	     {
	       sec_switch();
		 } while (set_count == 1);
	  }
   else if(set_count == 2)                 // For Setting Minutes
   {
	     do 
	     {
	       nokia_lcd_set_cursor(40,33);
	       nokia_lcd_write_string("^",1);
	       nokia_lcd_render();	   
		   min_switch();
		 } while (set_count == 2);
	        
   }
   else if(set_count == 3)                         // For Setting Hours
   {
	  do 
	  {
	    nokia_lcd_set_cursor(10,33);
	    nokia_lcd_write_string("^",1);
	    nokia_lcd_render();
		hours_switch();
	  } while (set_count == 3);
	 }
   else if (set_count ==4)                             // for reverse time
   {
       r_sec = sec_print;
	   r_min = min_print;
	   r_hours = hours_print;
	   roll();
       _delay_us(100);
	   reverse_timer();
       on_off();
	   break;
	}
   }
 }
  
  
   
//  *********************************************** ON_OFF  *******************************************
void on_off()
{
	  if(bit_is_clear(PINA,4))                 //  LED ON
	  {
		  PORTD = 0b00000001;
		  r_sec = sec_print;
		  r_min = min_print;
		  r_hours = hours_print;
		  reverse_timer();
		  PORTD = 0b00000000;
	     
		 nokia_lcd_clear();
		 nokia_lcd_render();
		 
	  }
	  else
	  {
		  nokia_lcd_clear();
	  }
	  
}



void reverse_timer()
{
    if(bit_is_clear(PINA,4))
	{
		nokia_lcd_set_cursor(0,0);
		nokia_lcd_write_string("Enable Switch:ON",1);
	    nokia_lcd_render();
	}
	else
	{
        nokia_lcd_set_cursor(0,0);
        nokia_lcd_write_string("Enable Switch:OFF",1);
		nokia_lcd_render();
	}
	itoa(r_hours,r_hours_buf,10);
	nokia_lcd_set_cursor(7,25);
	nokia_lcd_write_string(r_hours_buf,1);
	nokia_lcd_set_cursor(20,25);
	nokia_lcd_write_string(":",1);
	itoa(r_min,r_min_buf,10);
	nokia_lcd_set_cursor(37,25);
    nokia_lcd_write_string(r_min_buf,1);		 
    nokia_lcd_set_cursor(54,25);
	nokia_lcd_write_string(":",1);
	itoa(r_sec,r_sec_buf,10);
	nokia_lcd_set_cursor(67,25);
    nokia_lcd_write_string(r_sec_buf,1);
    nokia_lcd_render();
	
	timerdeclarations();
	sei();
	while(1)
	{
		if(tot_overflow >= 1)
		{
			tot_overflow =0;
			sec_counter = 1;
			_delay_us(160);  // Delay for Fractional Compensation
		    }
	     
		 if(sec_counter == 1)
		 {
			if(r_sec != 0)
			{
			r_sec--;
			sec_clear();
			itoa(r_sec,r_sec_buf,10);
			nokia_lcd_set_cursor(67,25);
		    nokia_lcd_write_string(r_sec_buf,1);
			nokia_lcd_render();
		    sec_counter = 0;
			}
			else
			{
				if(r_min != 0)
				{
					r_min--;
					min_clear();
					itoa(r_min,r_min_buf,10);
					nokia_lcd_set_cursor(37,25);
					nokia_lcd_write_string(r_min_buf,1);
					nokia_lcd_render();
					r_sec = 60;
					}
			    else
				{
					  if(r_hours != 0)
					  {
						  r_hours--;
						  hours_clear();
						  itoa(r_hours,r_hours_buf,10);
						  nokia_lcd_set_cursor(7,25);
						  nokia_lcd_write_string(r_hours_buf,1);
						  nokia_lcd_render();
					      r_min = 60;
					  }
					  else
					  {
						}
					  }
			        }
		          }
                if(r_hours == 0 && r_min ==0 && r_sec == 0)
				{
					break;
				}
				}
              }




// ********************************************* TIMER DECLARATION *****************************************
void timerdeclarations()
{
	TCCR1B = 0b00000101;
	TCNT1  = 64560;
	TIMSK  = 0b00000100;
	tot_overflow =0;
	
}    

//   **************************************  INTRO  ********************************************************

void intro()
{
	nokia_lcd_init();
	nokia_lcd_clear();
	nokia_lcd_set_cursor(15,1);
	nokia_lcd_write_string("SAMOM",2);
	nokia_lcd_set_cursor(7, 20);
	nokia_lcd_write_string("Technologies",1);
	nokia_lcd_render();
	_delay_ms(1000);
	
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("SET TIME:-",1);
	nokia_lcd_set_cursor(7, 25);
	nokia_lcd_write_string("00 : 00 : 00",1);
	nokia_lcd_set_cursor(70,33);
	nokia_lcd_write_string("^",1);
	nokia_lcd_render();
}

void roll()
{
 _delay_ms(100);
 nokia_lcd_clear();
 nokia_lcd_set_cursor(0,0);
 nokia_lcd_write_string("STARTING TIMER",1);
 nokia_lcd_set_cursor(38,24);
 nokia_lcd_write_string("3",2);
 nokia_lcd_render();
 _delay_ms(700);
 nokia_lcd_clear();
 nokia_lcd_set_cursor(0,0);
 nokia_lcd_write_string("STARTING TIMER",1);
 nokia_lcd_set_cursor(38,24);
 nokia_lcd_write_string("2",2);
 nokia_lcd_render();
 _delay_ms(700);
 nokia_lcd_set_cursor(0,0);
 nokia_lcd_write_string("STARTING TIMER",1);
 nokia_lcd_set_cursor(38,24);
 nokia_lcd_write_string("1",2);
 nokia_lcd_render();
 _delay_ms(700);
 
 nokia_lcd_clear();
 nokia_lcd_render();
 
 	 
}
// *********************************************** HOUR SWITCH  **************************************

void hours_switch()
{
      if(bit_is_clear(PINA,1))
      {
	      if(hours_print <= 29)
	      {
		      hours_clear();
		      hours_print++;
		      itoa(hours_print,hours_buf,10);
		      nokia_lcd_set_cursor(10,25);
		      nokia_lcd_write_string(hours_buf,1);
		      nokia_lcd_render();
	      }
      }
      else if(bit_is_clear(PINA,2))
      {
	      if(min_print != 0)
	      {
		      hours_clear();
		      hours_print--;
		      itoa(hours_print,hours_buf,10);
		      nokia_lcd_set_cursor(10,25);
		      nokia_lcd_write_string(hours_buf,1);
		      nokia_lcd_render();
	      }
      }
      else if (bit_is_clear(PINA,3))
      {
	      set_count = 4;
	      nokia_lcd_clear();
	      
      }
	
} 
//  ********************************************* MINUTE SWITCH   ************************************
void min_switch()
{
 if(bit_is_clear(PINA,1))
 {
	 if(min_print <= 58)
	 {
		 min_clear();
		 min_print++;
		 itoa(min_print,min_buf,10);
		 nokia_lcd_set_cursor(35,25);
		 nokia_lcd_write_string(min_buf,1);
		 nokia_lcd_render();
	 }
 }
 else if(bit_is_clear(PINA,2))
 {
	 if(min_print != 0)
	 {
		 min_clear();
		 min_print--;
		 itoa(min_print,min_buf,10);
		 nokia_lcd_set_cursor(35,25);
		 nokia_lcd_write_string(min_buf,1);
		 nokia_lcd_render();
	 }
 }
 else if (bit_is_clear(PINA,3))
  {
	 _delay_ms(200);
	 set_count = 3;
	 min_clear_arrow();
	 }
 }

//  ******************************************** SECONDS SWITCH  *************************************

void sec_switch()
{
  
  if(bit_is_clear(PINA,1))
  {
	  if(sec_print <= 58)
	  {
	  sec_clear();
	  sec_print++;
	  itoa(sec_print,sec_buf,10);
	  nokia_lcd_set_cursor(67,25);
	  nokia_lcd_write_string(sec_buf,1);
      nokia_lcd_render();
	  }
  }
  else if(bit_is_clear(PINA,2))
  {
	   if(sec_print != 0)
	   {
	    sec_clear();
	    sec_print--;
	    itoa(sec_print,sec_buf,10);
		nokia_lcd_set_cursor(67,25);
	    nokia_lcd_write_string(sec_buf,1);
	    nokia_lcd_render();
	   }
    }
        else if (bit_is_clear(PINA,3))
          {
                   _delay_ms(200);
				   set_count = 2;
				   sec_clear_arrow();
		    }
 }

//  **************************************************  Clear Hour Function ********************************************
void hours_clear()
{
	nokia_lcd_set_pixel(1,23,0);
	nokia_lcd_set_pixel(1,24,0);
	nokia_lcd_set_pixel(1,25,0);
	nokia_lcd_set_pixel(1,26,0);
	nokia_lcd_set_pixel(1,27,0);
	nokia_lcd_set_pixel(1,28,0);
	nokia_lcd_set_pixel(1,29,0);
	nokia_lcd_set_pixel(1,30,0);
	nokia_lcd_set_pixel(1,31,0);
	nokia_lcd_set_pixel(2,23,0);
	nokia_lcd_set_pixel(3,23,0);
	nokia_lcd_set_pixel(4,23,0);
	nokia_lcd_set_pixel(5,23,0);
	nokia_lcd_set_pixel(6,23,0);
	nokia_lcd_set_pixel(7,23,0);
	nokia_lcd_set_pixel(8,23,0);
	nokia_lcd_set_pixel(9,23,0);
	nokia_lcd_set_pixel(10,23,0);
	nokia_lcd_set_pixel(11,23,0);
	nokia_lcd_set_pixel(12,23,0);
	nokia_lcd_set_pixel(13,23,0);
	nokia_lcd_set_pixel(14,23,0);
	nokia_lcd_set_pixel(15,23,0);
	nokia_lcd_set_pixel(16,23,0);
	nokia_lcd_set_pixel(17,23,0);
	nokia_lcd_set_pixel(18,23,0);
	nokia_lcd_set_pixel(19,23,0);
	nokia_lcd_set_pixel(2,24,0);
	nokia_lcd_set_pixel(3,24,0);
	nokia_lcd_set_pixel(4,24,0);
	nokia_lcd_set_pixel(5,24,0);
	nokia_lcd_set_pixel(6,24,0);
	nokia_lcd_set_pixel(7,24,0);
	nokia_lcd_set_pixel(8,24,0);
	nokia_lcd_set_pixel(9,24,0);
	nokia_lcd_set_pixel(10,24,0);
	nokia_lcd_set_pixel(11,24,0);
	nokia_lcd_set_pixel(12,24,0);
	nokia_lcd_set_pixel(13,24,0);
	nokia_lcd_set_pixel(14,24,0);
	nokia_lcd_set_pixel(15,24,0);
	nokia_lcd_set_pixel(16,24,0);
	nokia_lcd_set_pixel(17,24,0);
	nokia_lcd_set_pixel(18,24,0);
	nokia_lcd_set_pixel(19,24,0);
	nokia_lcd_set_pixel(2,25,0);
	nokia_lcd_set_pixel(3,25,0);
	nokia_lcd_set_pixel(4,25,0);
	nokia_lcd_set_pixel(5,25,0);
	nokia_lcd_set_pixel(6,25,0);
	nokia_lcd_set_pixel(7,25,0);
	nokia_lcd_set_pixel(8,25,0);
	nokia_lcd_set_pixel(9,25,0);
	nokia_lcd_set_pixel(10,25,0);
	nokia_lcd_set_pixel(11,25,0);
	nokia_lcd_set_pixel(12,25,0);
	nokia_lcd_set_pixel(13,25,0);
	nokia_lcd_set_pixel(14,25,0);
	nokia_lcd_set_pixel(15,25,0);
	nokia_lcd_set_pixel(16,25,0);
	nokia_lcd_set_pixel(17,25,0);
	nokia_lcd_set_pixel(18,25,0);
	nokia_lcd_set_pixel(19,25,0);
	nokia_lcd_set_pixel(2,26,0);
	nokia_lcd_set_pixel(3,26,0);
	nokia_lcd_set_pixel(4,26,0);
	nokia_lcd_set_pixel(5,26,0);
	nokia_lcd_set_pixel(6,26,0);
	nokia_lcd_set_pixel(7,26,0);
	nokia_lcd_set_pixel(8,26,0);
	nokia_lcd_set_pixel(9,26,0);
	nokia_lcd_set_pixel(10,26,0);
	nokia_lcd_set_pixel(11,26,0);
	nokia_lcd_set_pixel(12,26,0);
	nokia_lcd_set_pixel(13,26,0);
	nokia_lcd_set_pixel(14,26,0);
	nokia_lcd_set_pixel(15,26,0);
	nokia_lcd_set_pixel(16,26,0);
	nokia_lcd_set_pixel(17,26,0);
	nokia_lcd_set_pixel(18,26,0);
	nokia_lcd_set_pixel(19,26,0);
	nokia_lcd_set_pixel(2,27,0);
	nokia_lcd_set_pixel(3,27,0);
	nokia_lcd_set_pixel(4,27,0);
	nokia_lcd_set_pixel(5,27,0);
	nokia_lcd_set_pixel(6,27,0);
	nokia_lcd_set_pixel(7,27,0);
	nokia_lcd_set_pixel(8,27,0);
	nokia_lcd_set_pixel(9,27,0);
	nokia_lcd_set_pixel(10,27,0);
	nokia_lcd_set_pixel(11,27,0);
	nokia_lcd_set_pixel(12,27,0);
	nokia_lcd_set_pixel(13,27,0);
	nokia_lcd_set_pixel(14,27,0);
	nokia_lcd_set_pixel(15,27,0);
	nokia_lcd_set_pixel(16,27,0);
	nokia_lcd_set_pixel(17,27,0);
	nokia_lcd_set_pixel(18,27,0);
	nokia_lcd_set_pixel(19,27,0);
	nokia_lcd_set_pixel(2,28,0);
	nokia_lcd_set_pixel(3,28,0);
	nokia_lcd_set_pixel(4,28,0);
	nokia_lcd_set_pixel(5,28,0);
	nokia_lcd_set_pixel(6,28,0);
	nokia_lcd_set_pixel(7,28,0);
	nokia_lcd_set_pixel(8,28,0);
	nokia_lcd_set_pixel(9,28,0);
	nokia_lcd_set_pixel(10,28,0);
	nokia_lcd_set_pixel(11,28,0);
	nokia_lcd_set_pixel(12,28,0);
	nokia_lcd_set_pixel(13,28,0);
	nokia_lcd_set_pixel(14,28,0);
	nokia_lcd_set_pixel(15,28,0);
	nokia_lcd_set_pixel(16,28,0);
	nokia_lcd_set_pixel(17,28,0);
	nokia_lcd_set_pixel(18,28,0);
	nokia_lcd_set_pixel(19,28,0);
	nokia_lcd_set_pixel(2,29,0);
	nokia_lcd_set_pixel(3,29,0);
	nokia_lcd_set_pixel(4,29,0);
	nokia_lcd_set_pixel(5,29,0);
	nokia_lcd_set_pixel(6,29,0);
	nokia_lcd_set_pixel(7,29,0);
	nokia_lcd_set_pixel(8,29,0);
	nokia_lcd_set_pixel(9,29,0);
	nokia_lcd_set_pixel(10,29,0);
	nokia_lcd_set_pixel(11,29,0);
	nokia_lcd_set_pixel(12,29,0);
	nokia_lcd_set_pixel(13,29,0);
	nokia_lcd_set_pixel(14,29,0);
	nokia_lcd_set_pixel(15,29,0);
	nokia_lcd_set_pixel(16,29,0);
	nokia_lcd_set_pixel(17,29,0);
	nokia_lcd_set_pixel(18,29,0);
	nokia_lcd_set_pixel(19,29,0);
	nokia_lcd_set_pixel(2,30,0);
	nokia_lcd_set_pixel(3,30,0);
	nokia_lcd_set_pixel(4,30,0);
	nokia_lcd_set_pixel(5,30,0);
	nokia_lcd_set_pixel(6,30,0);
	nokia_lcd_set_pixel(7,30,0);
	nokia_lcd_set_pixel(8,30,0);
	nokia_lcd_set_pixel(9,30,0);
	nokia_lcd_set_pixel(10,30,0);
	nokia_lcd_set_pixel(11,30,0);
	nokia_lcd_set_pixel(12,30,0);
	nokia_lcd_set_pixel(13,30,0);
	nokia_lcd_set_pixel(14,30,0);
	nokia_lcd_set_pixel(15,30,0);
	nokia_lcd_set_pixel(16,30,0);
	nokia_lcd_set_pixel(17,30,0);
	nokia_lcd_set_pixel(18,30,0);
	nokia_lcd_set_pixel(19,30,0);
	nokia_lcd_set_pixel(2,31,0);
	nokia_lcd_set_pixel(3,31,0);
	nokia_lcd_set_pixel(4,31,0);
	nokia_lcd_set_pixel(5,31,0);
	nokia_lcd_set_pixel(6,31,0);
	nokia_lcd_set_pixel(7,31,0);
	nokia_lcd_set_pixel(8,31,0);
	nokia_lcd_set_pixel(9,31,0);
	nokia_lcd_set_pixel(10,31,0);
	nokia_lcd_set_pixel(11,31,0);
	nokia_lcd_set_pixel(12,31,0);
	nokia_lcd_set_pixel(13,31,0);
	nokia_lcd_set_pixel(14,31,0);
	nokia_lcd_set_pixel(15,31,0);
	nokia_lcd_set_pixel(16,31,0);
	nokia_lcd_set_pixel(17,31,0);
	nokia_lcd_set_pixel(18,31,0);
	nokia_lcd_set_pixel(19,31,0);
    nokia_lcd_render();
}

//  *****************************************************  Clear Minute Function   ****************************************

void min_clear()
{
   nokia_lcd_set_pixel(32,23,0);
   nokia_lcd_set_pixel(32,24,0);
   nokia_lcd_set_pixel(32,25,0);
   nokia_lcd_set_pixel(32,26,0);
   nokia_lcd_set_pixel(32,27,0);
   nokia_lcd_set_pixel(32,28,0);
   nokia_lcd_set_pixel(32,29,0);
   nokia_lcd_set_pixel(32,30,0);
   nokia_lcd_set_pixel(32,31,0);
   nokia_lcd_set_pixel(33,23,0);
   nokia_lcd_set_pixel(33,24,0);
   nokia_lcd_set_pixel(33,25,0);
   nokia_lcd_set_pixel(33,26,0);
   nokia_lcd_set_pixel(33,27,0);
   nokia_lcd_set_pixel(33,28,0);
   nokia_lcd_set_pixel(33,29,0);
   nokia_lcd_set_pixel(33,30,0);
   nokia_lcd_set_pixel(33,31,0);
   nokia_lcd_set_pixel(34,23,0);
   nokia_lcd_set_pixel(34,24,0);
   nokia_lcd_set_pixel(34,25,0);
   nokia_lcd_set_pixel(34,26,0);
   nokia_lcd_set_pixel(34,27,0);
   nokia_lcd_set_pixel(34,28,0);
   nokia_lcd_set_pixel(34,29,0);
   nokia_lcd_set_pixel(34,30,0);
   nokia_lcd_set_pixel(34,31,0);
   nokia_lcd_set_pixel(35,23,0);
   nokia_lcd_set_pixel(35,24,0);
   nokia_lcd_set_pixel(35,25,0);
   nokia_lcd_set_pixel(35,26,0);
   nokia_lcd_set_pixel(35,27,0);
   nokia_lcd_set_pixel(35,28,0);
   nokia_lcd_set_pixel(35,29,0);
   nokia_lcd_set_pixel(35,30,0);
   nokia_lcd_set_pixel(35,31,0);
   nokia_lcd_set_pixel(36,23,0);
   nokia_lcd_set_pixel(36,24,0);
   nokia_lcd_set_pixel(36,25,0);
   nokia_lcd_set_pixel(36,26,0);
   nokia_lcd_set_pixel(36,27,0);
   nokia_lcd_set_pixel(36,28,0);
   nokia_lcd_set_pixel(36,29,0);
   nokia_lcd_set_pixel(36,30,0);
   nokia_lcd_set_pixel(36,31,0);
   nokia_lcd_set_pixel(37,23,0);
   nokia_lcd_set_pixel(37,24,0);
   nokia_lcd_set_pixel(37,25,0);
   nokia_lcd_set_pixel(37,26,0);
   nokia_lcd_set_pixel(37,27,0);
   nokia_lcd_set_pixel(37,28,0);
   nokia_lcd_set_pixel(37,29,0);
   nokia_lcd_set_pixel(37,30,0);
   nokia_lcd_set_pixel(37,31,0);
   nokia_lcd_set_pixel(38,23,0);
   nokia_lcd_set_pixel(38,24,0);
   nokia_lcd_set_pixel(38,25,0);
   nokia_lcd_set_pixel(38,26,0);
   nokia_lcd_set_pixel(38,27,0);
   nokia_lcd_set_pixel(38,28,0);
   nokia_lcd_set_pixel(38,29,0);
   nokia_lcd_set_pixel(38,30,0);
   nokia_lcd_set_pixel(38,31,0);
   nokia_lcd_set_pixel(39,23,0);
   nokia_lcd_set_pixel(39,24,0);
   nokia_lcd_set_pixel(39,25,0);
   nokia_lcd_set_pixel(39,26,0);
   nokia_lcd_set_pixel(39,27,0);
   nokia_lcd_set_pixel(39,28,0);
   nokia_lcd_set_pixel(39,29,0);
   nokia_lcd_set_pixel(39,30,0);
   nokia_lcd_set_pixel(39,31,0);
   nokia_lcd_set_pixel(40,23,0);
   nokia_lcd_set_pixel(40,24,0);
   nokia_lcd_set_pixel(40,25,0);
   nokia_lcd_set_pixel(40,26,0);
   nokia_lcd_set_pixel(40,27,0);
   nokia_lcd_set_pixel(40,28,0);
   nokia_lcd_set_pixel(40,29,0);
   nokia_lcd_set_pixel(40,30,0);
   nokia_lcd_set_pixel(40,31,0);
   nokia_lcd_set_pixel(41,23,0);
   nokia_lcd_set_pixel(41,24,0);
   nokia_lcd_set_pixel(41,25,0);
   nokia_lcd_set_pixel(41,26,0);
   nokia_lcd_set_pixel(41,27,0);
   nokia_lcd_set_pixel(41,28,0);
   nokia_lcd_set_pixel(41,29,0);
   nokia_lcd_set_pixel(41,30,0);
   nokia_lcd_set_pixel(41,31,0);
   nokia_lcd_set_pixel(42,23,0);
   nokia_lcd_set_pixel(42,24,0);
   nokia_lcd_set_pixel(42,25,0);
   nokia_lcd_set_pixel(42,26,0);
   nokia_lcd_set_pixel(42,27,0);
   nokia_lcd_set_pixel(42,28,0);
   nokia_lcd_set_pixel(42,29,0);
   nokia_lcd_set_pixel(42,30,0);
   nokia_lcd_set_pixel(42,31,0);
   nokia_lcd_set_pixel(43,23,0);
   nokia_lcd_set_pixel(43,24,0);
   nokia_lcd_set_pixel(43,25,0);
   nokia_lcd_set_pixel(43,26,0);
   nokia_lcd_set_pixel(43,27,0);
   nokia_lcd_set_pixel(43,28,0);
   nokia_lcd_set_pixel(43,29,0);
   nokia_lcd_set_pixel(43,30,0);
   nokia_lcd_set_pixel(43,31,0);
   nokia_lcd_set_pixel(44,23,0);
   nokia_lcd_set_pixel(44,24,0);
   nokia_lcd_set_pixel(44,25,0);
   nokia_lcd_set_pixel(44,26,0);
   nokia_lcd_set_pixel(44,27,0);
   nokia_lcd_set_pixel(44,28,0);
   nokia_lcd_set_pixel(44,29,0);
   nokia_lcd_set_pixel(44,30,0);
   nokia_lcd_set_pixel(44,31,0);
   nokia_lcd_set_pixel(45,23,0);
   nokia_lcd_set_pixel(45,24,0);
   nokia_lcd_set_pixel(45,25,0);
   nokia_lcd_set_pixel(45,26,0);
   nokia_lcd_set_pixel(45,27,0);
   nokia_lcd_set_pixel(45,28,0);
   nokia_lcd_set_pixel(45,29,0);
   nokia_lcd_set_pixel(45,30,0);
   nokia_lcd_set_pixel(45,31,0);
   nokia_lcd_set_pixel(46,23,0);
   nokia_lcd_set_pixel(46,24,0);
   nokia_lcd_set_pixel(46,25,0);
   nokia_lcd_set_pixel(46,26,0);
   nokia_lcd_set_pixel(46,27,0);
   nokia_lcd_set_pixel(46,28,0);
   nokia_lcd_set_pixel(46,29,0);
   nokia_lcd_set_pixel(46,30,0);
   nokia_lcd_set_pixel(46,31,0);
   nokia_lcd_set_pixel(47,23,0);
   nokia_lcd_set_pixel(47,24,0);
   nokia_lcd_set_pixel(47,25,0);
   nokia_lcd_set_pixel(47,26,0);
   nokia_lcd_set_pixel(47,27,0);
   nokia_lcd_set_pixel(47,28,0);
   nokia_lcd_set_pixel(47,29,0);
   nokia_lcd_set_pixel(47,30,0);
   nokia_lcd_set_pixel(47,31,0);
   nokia_lcd_set_pixel(48,23,0);
   nokia_lcd_set_pixel(48,24,0);
   nokia_lcd_set_pixel(48,25,0);
   nokia_lcd_set_pixel(48,26,0);
   nokia_lcd_set_pixel(48,27,0);
   nokia_lcd_set_pixel(48,28,0);
   nokia_lcd_set_pixel(48,29,0);
   nokia_lcd_set_pixel(48,30,0);
   nokia_lcd_set_pixel(48,31,0);
   nokia_lcd_set_pixel(49,23,0);
   nokia_lcd_set_pixel(49,24,0);
   nokia_lcd_set_pixel(49,25,0);
   nokia_lcd_set_pixel(49,26,0);
   nokia_lcd_set_pixel(49,27,0);
   nokia_lcd_set_pixel(49,28,0);
   nokia_lcd_set_pixel(49,29,0);
   nokia_lcd_set_pixel(49,30,0);
   nokia_lcd_set_pixel(49,31,0);
   nokia_lcd_set_pixel(50,23,0);
   nokia_lcd_set_pixel(50,24,0);
   nokia_lcd_set_pixel(50,25,0);
   nokia_lcd_set_pixel(50,26,0);
   nokia_lcd_set_pixel(50,27,0);
   nokia_lcd_set_pixel(50,28,0);
   nokia_lcd_set_pixel(50,29,0);
   nokia_lcd_set_pixel(50,30,0);
   nokia_lcd_set_pixel(50,31,0);
   nokia_lcd_set_pixel(51,23,0);
   nokia_lcd_set_pixel(51,24,0);
   nokia_lcd_set_pixel(51,25,0);
   nokia_lcd_set_pixel(51,26,0);
   nokia_lcd_set_pixel(51,27,0);
   nokia_lcd_set_pixel(51,28,0);
   nokia_lcd_set_pixel(51,29,0);
   nokia_lcd_set_pixel(51,30,0);
   nokia_lcd_set_pixel(51,31,0);
   nokia_lcd_render();
}

//  ********************************************* Clear Seconds Function   ***********************************************

void sec_clear()
{nokia_lcd_set_pixel(60,23,0);
	nokia_lcd_set_pixel(60,24,0);
	nokia_lcd_set_pixel(60,25,0);
	nokia_lcd_set_pixel(60,26,0);
	nokia_lcd_set_pixel(60,27,0);
	nokia_lcd_set_pixel(60,28,0);
	nokia_lcd_set_pixel(60,29,0);
	nokia_lcd_set_pixel(60,30,0);
	nokia_lcd_set_pixel(60,31,0);
	nokia_lcd_set_pixel(61,23,0);
	nokia_lcd_set_pixel(61,24,0);
	nokia_lcd_set_pixel(61,25,0);
	nokia_lcd_set_pixel(61,26,0);
	nokia_lcd_set_pixel(61,27,0);
	nokia_lcd_set_pixel(61,28,0);
	nokia_lcd_set_pixel(61,29,0);
	nokia_lcd_set_pixel(61,30,0);
	nokia_lcd_set_pixel(61,31,0);
	nokia_lcd_set_pixel(62,23,0);
	nokia_lcd_set_pixel(62,24,0);
	nokia_lcd_set_pixel(62,25,0);
	nokia_lcd_set_pixel(62,26,0);
	nokia_lcd_set_pixel(62,27,0);
	nokia_lcd_set_pixel(62,28,0);
	nokia_lcd_set_pixel(62,29,0);
	nokia_lcd_set_pixel(62,30,0);
	nokia_lcd_set_pixel(62,31,0);
	nokia_lcd_set_pixel(63,23,0);
	nokia_lcd_set_pixel(63,24,0);
	nokia_lcd_set_pixel(63,25,0);
	nokia_lcd_set_pixel(63,26,0);
	nokia_lcd_set_pixel(63,27,0);
	nokia_lcd_set_pixel(63,28,0);
	nokia_lcd_set_pixel(63,29,0);
	nokia_lcd_set_pixel(63,30,0);
	nokia_lcd_set_pixel(63,31,0);
	nokia_lcd_set_pixel(64,23,0);
	nokia_lcd_set_pixel(64,24,0);
	nokia_lcd_set_pixel(64,25,0);
	nokia_lcd_set_pixel(64,26,0);
	nokia_lcd_set_pixel(64,27,0);
	nokia_lcd_set_pixel(64,28,0);
	nokia_lcd_set_pixel(64,29,0);
	nokia_lcd_set_pixel(64,30,0);
	nokia_lcd_set_pixel(64,31,0);
	nokia_lcd_set_pixel(65,23,0);
	nokia_lcd_set_pixel(65,24,0);
	nokia_lcd_set_pixel(65,25,0);
	nokia_lcd_set_pixel(65,26,0);
	nokia_lcd_set_pixel(65,27,0);
	nokia_lcd_set_pixel(65,28,0);
	nokia_lcd_set_pixel(65,29,0);
	nokia_lcd_set_pixel(65,30,0);
	nokia_lcd_set_pixel(65,31,0);
	nokia_lcd_set_pixel(66,23,0);
	nokia_lcd_set_pixel(66,24,0);
	nokia_lcd_set_pixel(66,25,0);
	nokia_lcd_set_pixel(66,26,0);
	nokia_lcd_set_pixel(66,27,0);
	nokia_lcd_set_pixel(66,28,0);
	nokia_lcd_set_pixel(66,29,0);
	nokia_lcd_set_pixel(66,30,0);
	nokia_lcd_set_pixel(66,31,0);
	nokia_lcd_set_pixel(67,23,0);
	nokia_lcd_set_pixel(67,24,0);
	nokia_lcd_set_pixel(67,25,0);
	nokia_lcd_set_pixel(67,26,0);
	nokia_lcd_set_pixel(67,27,0);
	nokia_lcd_set_pixel(67,28,0);
	nokia_lcd_set_pixel(67,29,0);
	nokia_lcd_set_pixel(67,30,0);
	nokia_lcd_set_pixel(67,31,0);
	nokia_lcd_set_pixel(68,23,0);
	nokia_lcd_set_pixel(68,24,0);
	nokia_lcd_set_pixel(68,25,0);
	nokia_lcd_set_pixel(68,26,0);
	nokia_lcd_set_pixel(68,27,0);
	nokia_lcd_set_pixel(68,28,0);
	nokia_lcd_set_pixel(68,29,0);
	nokia_lcd_set_pixel(68,30,0);
	nokia_lcd_set_pixel(68,31,0);
	nokia_lcd_set_pixel(69,23,0);
	nokia_lcd_set_pixel(69,24,0);
	nokia_lcd_set_pixel(69,25,0);
	nokia_lcd_set_pixel(69,26,0);
	nokia_lcd_set_pixel(69,27,0);
	nokia_lcd_set_pixel(69,28,0);
	nokia_lcd_set_pixel(69,29,0);
	nokia_lcd_set_pixel(69,30,0);
	nokia_lcd_set_pixel(69,31,0);
	nokia_lcd_set_pixel(70,23,0);
	nokia_lcd_set_pixel(70,24,0);
	nokia_lcd_set_pixel(70,25,0);
	nokia_lcd_set_pixel(70,26,0);
	nokia_lcd_set_pixel(70,27,0);
	nokia_lcd_set_pixel(70,28,0);
	nokia_lcd_set_pixel(70,29,0);
	nokia_lcd_set_pixel(70,30,0);
	nokia_lcd_set_pixel(70,31,0);
	nokia_lcd_set_pixel(71,23,0);
	nokia_lcd_set_pixel(71,24,0);
	nokia_lcd_set_pixel(71,25,0);
	nokia_lcd_set_pixel(71,26,0);
	nokia_lcd_set_pixel(71,27,0);
	nokia_lcd_set_pixel(71,28,0);
	nokia_lcd_set_pixel(71,29,0);
	nokia_lcd_set_pixel(71,30,0);
	nokia_lcd_set_pixel(71,31,0);
	nokia_lcd_set_pixel(72,23,0);
	nokia_lcd_set_pixel(72,24,0);
	nokia_lcd_set_pixel(72,25,0);
	nokia_lcd_set_pixel(72,26,0);
	nokia_lcd_set_pixel(72,27,0);
	nokia_lcd_set_pixel(72,28,0);
	nokia_lcd_set_pixel(72,29,0);
	nokia_lcd_set_pixel(72,30,0);
	nokia_lcd_set_pixel(72,31,0);
	nokia_lcd_set_pixel(73,23,0);
	nokia_lcd_set_pixel(73,24,0);
	nokia_lcd_set_pixel(73,25,0);
	nokia_lcd_set_pixel(73,26,0);
	nokia_lcd_set_pixel(73,27,0);
	nokia_lcd_set_pixel(73,28,0);
	nokia_lcd_set_pixel(73,29,0);
	nokia_lcd_set_pixel(73,30,0);
	nokia_lcd_set_pixel(73,31,0);
	nokia_lcd_set_pixel(74,23,0);
	nokia_lcd_set_pixel(74,24,0);
	nokia_lcd_set_pixel(74,25,0);
	nokia_lcd_set_pixel(74,26,0);
	nokia_lcd_set_pixel(74,27,0);
	nokia_lcd_set_pixel(74,28,0);
	nokia_lcd_set_pixel(74,29,0);
	nokia_lcd_set_pixel(74,30,0);
	nokia_lcd_set_pixel(74,31,0);
	nokia_lcd_set_pixel(75,23,0);
	nokia_lcd_set_pixel(75,24,0);
	nokia_lcd_set_pixel(75,25,0);
	nokia_lcd_set_pixel(75,26,0);
	nokia_lcd_set_pixel(75,27,0);
	nokia_lcd_set_pixel(75,28,0);
	nokia_lcd_set_pixel(75,29,0);
	nokia_lcd_set_pixel(75,30,0);
	nokia_lcd_set_pixel(75,31,0);
	nokia_lcd_set_pixel(76,23,0);
	nokia_lcd_set_pixel(76,24,0);
	nokia_lcd_set_pixel(76,25,0);
	nokia_lcd_set_pixel(76,26,0);
	nokia_lcd_set_pixel(76,27,0);
	nokia_lcd_set_pixel(76,28,0);
	nokia_lcd_set_pixel(76,29,0);
	nokia_lcd_set_pixel(76,30,0);
	nokia_lcd_set_pixel(76,31,0);
	nokia_lcd_set_pixel(77,23,0);
	nokia_lcd_set_pixel(77,24,0);
	nokia_lcd_set_pixel(77,25,0);
	nokia_lcd_set_pixel(77,26,0);
	nokia_lcd_set_pixel(77,27,0);
	nokia_lcd_set_pixel(77,28,0);
	nokia_lcd_set_pixel(77,29,0);
	nokia_lcd_set_pixel(77,30,0);
	nokia_lcd_set_pixel(77,31,0);
	nokia_lcd_set_pixel(78,23,0);
	nokia_lcd_set_pixel(78,24,0);
	nokia_lcd_set_pixel(78,25,0);
	nokia_lcd_set_pixel(78,26,0);
	nokia_lcd_set_pixel(78,27,0);
	nokia_lcd_set_pixel(78,28,0);
	nokia_lcd_set_pixel(78,29,0);
	nokia_lcd_set_pixel(78,30,0);
	nokia_lcd_set_pixel(78,31,0); 	
	nokia_lcd_render();
}

// ************************************************* ARROW CLEAR   *************************************

void sec_clear_arrow()
{
	nokia_lcd_set_pixel(70,33,0);
	nokia_lcd_set_pixel(70,34,0);
	nokia_lcd_set_pixel(70,35,0);
	nokia_lcd_set_pixel(70,36,0);
	nokia_lcd_set_pixel(71,33,0);
	nokia_lcd_set_pixel(71,34,0);
	nokia_lcd_set_pixel(71,35,0);
	nokia_lcd_set_pixel(71,36,0);
	nokia_lcd_set_pixel(72,33,0);
	nokia_lcd_set_pixel(72,34,0);
	nokia_lcd_set_pixel(72,35,0);
	nokia_lcd_set_pixel(72,36,0);
	nokia_lcd_set_pixel(73,33,0);
	nokia_lcd_set_pixel(73,34,0);
	nokia_lcd_set_pixel(73,35,0);
	nokia_lcd_set_pixel(73,36,0);
	nokia_lcd_set_pixel(74,33,0);
	nokia_lcd_set_pixel(74,34,0);
	nokia_lcd_set_pixel(74,35,0);
	nokia_lcd_set_pixel(74,36,0);
	nokia_lcd_set_pixel(75,33,0);
	nokia_lcd_set_pixel(75,34,0);
	nokia_lcd_set_pixel(75,35,0);
	nokia_lcd_set_pixel(75,36,0);	
	nokia_lcd_render();
}

void min_clear_arrow()
{
	nokia_lcd_set_pixel(40,33,0);
	nokia_lcd_set_pixel(40,34,0);
	nokia_lcd_set_pixel(40,35,0);
	nokia_lcd_set_pixel(40,36,0);
	nokia_lcd_set_pixel(41,33,0);
	nokia_lcd_set_pixel(41,34,0);
	nokia_lcd_set_pixel(41,35,0);
	nokia_lcd_set_pixel(41,36,0);
	nokia_lcd_set_pixel(42,33,0);
	nokia_lcd_set_pixel(42,34,0);
	nokia_lcd_set_pixel(42,35,0);
	nokia_lcd_set_pixel(42,36,0);
	nokia_lcd_set_pixel(43,33,0);
	nokia_lcd_set_pixel(43,34,0);
	nokia_lcd_set_pixel(43,35,0);
	nokia_lcd_set_pixel(43,36,0);
	nokia_lcd_set_pixel(44,33,0);
	nokia_lcd_set_pixel(44,34,0);
	nokia_lcd_set_pixel(44,35,0);
	nokia_lcd_set_pixel(44,36,0);
	nokia_lcd_set_pixel(45,33,0);
	nokia_lcd_set_pixel(45,34,0);
	nokia_lcd_set_pixel(45,35,0);
	nokia_lcd_set_pixel(45,36,0);
	nokia_lcd_render();
}

void hours_clear_arrow()
{
	nokia_lcd_set_pixel(10,33,20);
	nokia_lcd_set_pixel(10,34,20);
	nokia_lcd_set_pixel(10,35,20);
	nokia_lcd_set_pixel(10,36,20);
	nokia_lcd_set_pixel(11,33,20);
	nokia_lcd_set_pixel(11,34,20);
	nokia_lcd_set_pixel(11,35,20);
	nokia_lcd_set_pixel(11,36,20);
	nokia_lcd_set_pixel(12,33,20);
	nokia_lcd_set_pixel(12,34,20);
	nokia_lcd_set_pixel(12,35,20);
	nokia_lcd_set_pixel(12,36,20);
	nokia_lcd_set_pixel(13,33,20);
	nokia_lcd_set_pixel(13,34,20);
	nokia_lcd_set_pixel(13,35,20);
	nokia_lcd_set_pixel(13,36,20);
	nokia_lcd_set_pixel(14,33,20);
	nokia_lcd_set_pixel(14,34,20);
	nokia_lcd_set_pixel(14,35,20);
	nokia_lcd_set_pixel(14,36,20);
	nokia_lcd_set_pixel(15,33,20);
	nokia_lcd_set_pixel(15,34,20);
	nokia_lcd_set_pixel(15,35,20);
	nokia_lcd_set_pixel(15,36,20);
	nokia_lcd_render();
}