/*
 * @file i2.c
 * @author xpress_embedo
 * @date 1 Feb, 2020
 * 
 * @brief Main File of the Project
 *
 */

#include "main.h"
#include "OLED.h"
#include "ds1307.h"

/* Local Variables */
static uint32_t millisecond = 0;

/* Local Function */
void timer0_init( void );
void system_init( void );


/* Start From Here*/
void main(void) 
{
  uint32_t timestamp = 0;
  char lcd_msg[16u] = {0};
  system_init();
  OLED_ClearDisplay();
  OLED_Update();
  
  while(1)
  {
    // Task-1 (500ms)
    if( millis() - timestamp > 500ul )
    {
      uint8_t year, month, date, hour, minute, second;
      static uint8_t sec_blink;
      timestamp = millis();
      second = DS1307_Read (DS1307_SEC);
      minute = DS1307_Read (DS1307_MIN);
      hour   = DS1307_Read (DS1307_HOUR);
      date   = DS1307_Read (DS1307_DATE);
      month  = DS1307_Read (DS1307_MONTH);
      year   = DS1307_Read (DS1307_YEAR);
      // The value Read are in BCD Format, which needs to be converted into char
      if( sec_blink )
      {
        sec_blink = FALSE;
        sprintf (lcd_msg, "TIME: %c%c:%c%c:%c%c", \
                 BCD2UpperCh (hour), BCD2LowerCh (hour), \
                 BCD2UpperCh (minute), BCD2LowerCh (minute), \
                 BCD2UpperCh (second), BCD2LowerCh (second) );
      }
      else
      {
        sec_blink = TRUE;
        sprintf (lcd_msg, "TIME: %c%c:%c%c %c%c", \
                 BCD2UpperCh (hour), BCD2LowerCh (hour), \
                 BCD2UpperCh (minute), BCD2LowerCh (minute), \
                 BCD2UpperCh (second), BCD2LowerCh (second) );
      }
      OLED_Write_Text (0, 0, lcd_msg);
      sprintf (lcd_msg, "DATE: %c%c/%c%c/%c%c", \
               BCD2UpperCh (date), BCD2LowerCh (date), \
               BCD2UpperCh (month), BCD2LowerCh (month), \
               BCD2UpperCh (year), BCD2LowerCh (year) );
      OLED_Write_Text (0, 8, lcd_msg);
      OLED_Update ();
    }
  }
  return;
}

/* Function Definitions */

/**
 * @breif Initialize the system
 */
void system_init( void )
{
  // Initialize 1ms Timer
  timer0_init ();
  // Initialize DS1307 and Start Time
  DS1307_Init ();
  // Initialize OLED
  OLED_Init ();
  Delay_ms(1000);
}

/**
 * @brief Counts milliseconds till startup
 * @return This function returns the number of milliseconds elapsed till the 
 * system is powered up.
 */
uint32_t millis( void )
{
  uint32_t time = 0;
  GIE = 0;
  time = millisecond;
  GIE = 1;
  return time;
}

/**
 * @brief Initialize Timer0 Module to generate 1ms interrupt
 */
void timer0_init( void )
{
  // Code Generated Using mikroC Timer Calculator
  T0CON	= 0x88;
  TMR0H	= 0xD1;
  TMR0L = 0x20;
  ENABLE_INT();
  TMR0IE = 1;
}

/**
 * @brief PIC18F Interrupt Service Routine
 */
void __interrupt() ISR_ROUTINE( void )
{
  if( TMR0IF )
  {
    TMR0IF = 0;
    TMR0H	= 0xD1;
    TMR0L = 0x20;
    millisecond++;
  }
}

