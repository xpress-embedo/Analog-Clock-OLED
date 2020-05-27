/*
 * @file main.c
 * @author xpress_embedo
 * @date 24th May 2020
 * 
 * @brief Main File of the Project
 *
 */

#include "main.h"
#include "OLED.h"
#include "ds1307.h"

/* Project Related Macros */
#define CLOCK_CENTER_X      ( 3u*OLED_Width()/4u )
#define CLOCK_CENTER_Y      ( OLED_Height()/2u )

/* Local Variables */
static uint32_t millisecond = 0;

/* Local Function */
void timer0_init( void );
void system_init( void );
void display_clock_face( void );
void display_time( uint8_t, uint8_t, uint8_t);


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
      OLED_ClearDisplay();
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
        sprintf (lcd_msg, "%c%c:%c%c:%c%c", \
                 BCD2UpperCh (hour), BCD2LowerCh (hour), \
                 BCD2UpperCh (minute), BCD2LowerCh (minute), \
                 BCD2UpperCh (second), BCD2LowerCh (second) );
      }
      else
      {
        sec_blink = TRUE;
        sprintf (lcd_msg, "%c%c:%c%c %c%c", \
                 BCD2UpperCh (hour), BCD2LowerCh (hour), \
                 BCD2UpperCh (minute), BCD2LowerCh (minute), \
                 BCD2UpperCh (second), BCD2LowerCh (second) );
      }
      OLED_Write_Text (0, 0, "  CLOCK");
      OLED_Write_Text (4, 10, lcd_msg);
      sprintf (lcd_msg, "%c%c/%c%c/%c%c", \
               BCD2UpperCh (date), BCD2LowerCh (date), \
               BCD2UpperCh (month), BCD2LowerCh (month), \
               BCD2UpperCh (year), BCD2LowerCh (year) );
      OLED_Write_Text (4, 18, lcd_msg);
      
      // Analog Clock
      display_clock_face();
      display_time( BCD2Decimal(hour), \
                   BCD2Decimal(minute),\
                   BCD2Decimal(second));
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

/**
 * @brief Draw the Clock Face
 */
void display_clock_face( void )
{
  uint16_t idx = 0;
  uint16_t x1, x2, y1, y2;
  // Draw Clock Face
//  for( idx = 0; idx<2u; idx++ )
//  {
//    OLED_Circle( CLOCK_CENTER_X, CLOCK_CENTER_Y, 31-idx, 1);  // Big Circle
//  }
//  for( idx=0; idx<3u; idx++ )
//  {
//    OLED_Circle( CLOCK_CENTER_X, CLOCK_CENTER_Y, idx, 1);     // Small Circle
//  }
  OLED_Circle( CLOCK_CENTER_X, CLOCK_CENTER_Y, 31, 1);  // Big Circle
  OLED_Circle( CLOCK_CENTER_X, CLOCK_CENTER_Y, 2u, 1);  // Small Circle
  // Draw Small Mark for Every Hour
  // hour ticks
  for( idx=0; idx<360; idx=idx+30 )
  {
    // Begin at 0 degree and stop at 360 degree
    // To convert degree into radians, we have to multiply degree by (pi/180)
    // degree (in radians) = degree * pi / 180
    // degree (in radians) = degree / (180/pi)
    // pi = 57.29577951 ( Python math.pi value )
    float angle = idx ;
    angle = (angle/57.29577951) ;         //Convert degrees to radians
    x1 = (CLOCK_CENTER_X + (sin(angle)*31));
    y1 = (CLOCK_CENTER_Y - (cos(angle)*31));
    x2 = (CLOCK_CENTER_X + (sin(angle)*(31-5)));
    y2 = (CLOCK_CENTER_Y - (cos(angle)*(31-5)));
    OLED_Line(x1, y1, x2, y2, 1);
  }
}

/**
 * @brief Display the time
 * @param hour hour values in decimal
 * @param minute minute values in decimal
 * @param second second values in decimal
 */
void display_time( uint8_t hour, uint8_t minute, uint8_t second)
{
  float angle = 0.0;
  uint16_t temp = 0;
  uint16_t x, y;
  // display second hand
  temp = (uint16_t)second * (uint16_t)6u;
  angle= ((float)temp/57.29577951) ;      //Convert degrees to radians  
  x = ( CLOCK_CENTER_X + (sin(angle)*(29)) );
  y = ( CLOCK_CENTER_Y - (cos(angle)*(29)) );
  OLED_Line( CLOCK_CENTER_X, CLOCK_CENTER_Y, x, y, 1);
  
  // display minute hand
  temp = minute * 6u;
  angle= ((float)temp/57.29577951) ;      //Convert degrees to radians  
  x = ( CLOCK_CENTER_X + (sin(angle)*(22)) );
  y = ( CLOCK_CENTER_Y - (cos(angle)*(22)) );
  OLED_Line( CLOCK_CENTER_X, CLOCK_CENTER_Y, x, y, 1);
  
  // display hour hand
  angle = hour * 30 + ((minute*6)/12 )   ;
  angle=(angle/57.29577951) ;             //Convert degrees to radians  
  x = ( CLOCK_CENTER_X + (sin(angle)*(15)) );
  y = ( CLOCK_CENTER_Y - (cos(angle)*(15)) );
  OLED_Line( CLOCK_CENTER_X, CLOCK_CENTER_Y, x, y, 1);
}