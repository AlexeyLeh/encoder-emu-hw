/**
* Created: 12.01.2026
* Author: Lekhtin A.V.
*/

#include "timer.h"
#include "uart.h"

static int32_t  g_encoder_clicks;
static uint8_t  g_encoder_direction;

uint16_t        g_timer_freq;

int8_t          g_timer_fullpass;
static int8_t   g_timer_fullpass_counter;

uint16_t        g_timer_ticks;
int32_t         g_timer_clicks_counter;

int8_t          stage = 0;

static const char msg_done[] = "DONE";
static const char *g_timer_message = 0;

static uint16_t idle_mode = 0;
static uint16_t ack_mode = 0;

//__regvar __no_init uint8_t g_timer_fullpass_counter @ 14;

//---------------------------------------------------------
void set_timer_settings(int32_t freq, int32_t clicks, uint8_t direction)
{
  disable_interrupt();
  TCCR1B = 0;               // timer stop
  g_encoder_clicks = clicks;
  g_encoder_direction = (direction == 1 ? 1 : 0);

  if (freq > 0)
  {
    g_timer_freq = freq * 4;
    uint32_t ticks = (F_CPU / g_timer_freq);
    if (ticks > 65536)
    {
      g_timer_fullpass = (ticks / 65536);
      uint32_t rem = ticks - (g_timer_fullpass * 65536);
      g_timer_ticks = 65536 - rem;
    }
    else
    {
      g_timer_ticks = 65536 - ticks;
      g_timer_fullpass = -1;
    }
  }
  enable_interrupt();
}

//---------------------------------------------------------
void stop_blink_led(void)
{
  TCCR2B = 0;         // Timer stop
  TCNT2 = 0;
  LED = 0;
}

//---------------------------------------------------------
void start_idle_led(void)
{
  stop_blink_led();
  idle_mode = 1;
  LED = 1;
  TCCR2B = 1;         // Timer start
}

//---------------------------------------------------------
void stop_idle_led(void)
{
  stop_blink_led();
  idle_mode = 0;
}

//---------------------------------------------------------
void start_ack_led(void)
{
  stop_blink_led();
  ack_mode = 1;
  TCNT2  = 200;
  TCCR2B = 1;         // Timer start
}

//---------------------------------------------------------
void stop_ack_led(void)
{
  stop_blink_led();
  ack_mode = 0;
}

//---------------------------------------------------------
void timer_set_done_message(const char *msg)
{
  g_timer_message = msg;
}

//---------------------------------------------------------
void setup_timers(void)
{
  disable_interrupt();
  // Timer0 - switching output ports
  TCCR0B = 0;         // Timer stop
  TCCR0A = 0;         // normal mode
  TIMSK0 = 1;         // Overflow Interrupt Enable
  TCNT0  = 255;       // for quick timer activation

  // Timer1 - frequency generator
  TCCR1B = 0;         // Timer stop
  TCCR1A = 0;         // normal mode
  TIMSK1 = 1;         // Overflow Interrupt Enable
  TCNT1  = 0;         // for quick timer activation

  // Timer2 - send final message "done"
  TCCR2B = 0;         // Timer stop
  TCCR2A = 0;         // normal mode
  TIMSK2 = 1;         // Overflow Interrupt Enable
  TCNT2  = 255;       // for quick timer activation
  enable_interrupt();
}

//---------------------------------------------------------
uint8_t start_encoder()
{
  setup_timers();

  disable_interrupt();
  stop_idle_led();
  stop_ack_led();

  LED  = 1;
  OUT1 = 0;
  OUT2 = 0;

  if (g_timer_freq == 0)
  {
    enable_interrupt();
    return 1;
  }
  if (g_encoder_clicks == 0)
  {
    enable_interrupt();
    return 2;
  }

  //---------------------------------
  // setup frequency generator
  g_timer_clicks_counter = g_encoder_clicks;
  g_timer_fullpass_counter = 0; // first shot asap
  TCCR1B = 1;         // timer start
  enable_interrupt();
  return 0;
}

//---------------------------------------------------------
void stop_encoder()
{
  disable_interrupt();
  TCCR0B = 0;         // Timer stop
  TCCR1B = 0;         // Timer stop
  TCCR2B = 0;         // Timer stop
  OUT1   = 0;
  OUT2   = 0;
  enable_interrupt();
}

//---------------------------------------------------------
#pragma vector=TIMER0_OVF_vect
__interrupt void InterruptHandlerT0(void)
{
  // output ports switching
  if (g_encoder_direction == 0)
  {
    switch (stage)
    {
    case 0:
      OUT1  = 1;
      stage = 1;
      break;
    case 1:
      OUT2  = 1;
      stage = 2;
      break;
    case 2:
      OUT1  = 0;
      stage = 3;
      break;
    default:
      OUT2  = 0;
      stage = 0;
      g_timer_clicks_counter--;
      LED = 1;
      break;
    }
  }
  else
  {
    switch (stage)
    {
    case 0:
      OUT2  = 1;
      stage = 1;
      break;
    case 1:
      OUT1  = 1;
      stage = 2;
      break;
    case 2:
      OUT2  = 0;
      stage = 3;
      break;
    default:
      OUT1  = 0;
      stage = 0;
      g_timer_clicks_counter--;
      LED = 1;
      break;
    }
  }

  TCCR0B = 0;         // Timer stop
  TCNT0  = 0xFF;      // prepearing for next shot

  if (g_timer_clicks_counter == 0)
  {
    // job in done - timer - generator will be stopped
    TCCR1B = 0;         // timer stop
    LED = 0;
    timer_set_done_message(msg_done);
    start_idle_led();
    return;
  }
}

//---------------------------------------------------------
#pragma vector=TIMER1_OVF_vect
__interrupt void InterruptHandlerT1(void)
{
  // frequency generator
  if (g_timer_fullpass > 0)
  {
    g_timer_fullpass_counter--;
    if (g_timer_fullpass_counter == 0)
    {
      TCNT1 = g_timer_ticks + 38; // for F_CPU 16MHz
    }
    else
    {
      if (g_timer_fullpass_counter == -1)
      {
        TCCR0B = 1;
        g_timer_fullpass_counter = g_timer_fullpass;
      }
      TCNT1 = 59; // for F_CPU 16MHz
    }
    return;
  }

  TCCR0B = 1;           // switching output ports
  TCNT1 = g_timer_ticks + 36;
}

//---------------------------------------------------------
#pragma vector=TIMER2_OVF_vect
__interrupt void InterruptHandlerT2(void)
{
  // send final message about "done"
  if (g_timer_message)
  {
    uart_println(g_timer_message);
    g_timer_message = 0;
  }

  if (ack_mode != 0)
  {
    switch (ack_mode)
    {
    case 1:
      LED = 1;
      break;
    case 2000:
    case 4000:
    //case 6000:
    //case 8000:
      LED = !LED;
      break;
    case 6000: //10000:
      stop_ack_led();
      start_idle_led();
      return;
    default:
      break;
    }
    ack_mode++;
    TCNT2 = 0;
    return;
  }

  if (idle_mode != 0)
  {
    if (idle_mode == 1)
    {
      LED = 1;
    }
    else if (idle_mode == 2000)
    {
      LED = 0;
    }
    else if (idle_mode == 65500)
    {
      idle_mode = 0;
    }
    idle_mode++;
    TCNT2 = 0;
    return;
  }
}

