/**
* Created: 12.01.2026
* Author: Lekhtin A.V.
*/

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "timer.h"
#include "uart.h"

static const char msg_start[]     = "\nSTART";
static const char msg_stop[]      = "STOP";
static const char msg_ok[]        = "OK";
static const char msg_fail[]      = "FAIL";
static const char msg_badcmd[]    = "BADCMD";
static const char msg_badfreq[]   = "BADFREQ";
static const char msg_badclicks[] = "BADCLICKS";
static const char msg_ERANGE[]    = "ERANGE";

static const char str_freq[]      = "FREQ:";
static const char str_clicks[]    = "CLICKS:";
static const char str_direction[] = "WAY:";
static const char str_forwd[]     = ">";
static const char str_backwd[]    = "<";
static const char str_timer[]     = "TIMER:";
static const char str_on[]        = "ON";
static const char str_off[]       = "OFF";

#define MAXFREQ 16000
#define MAXCLICKS 100000000

static char str_help[] =
"Encoder emulator v1.0 Jan 20 2026 ("__DATE__ " " __TIME__ ")\n"
"CMD:\n"
"c [-]<1-100000000> - clicks\n"
"f <1-16000> - freq\n"
"i - info\n"
"p - ping\n"
"r - run\n"
"e - stop\n"
"ret: START DONE OK FAIL BADCMD BADFREQ BADCLICKS ERANGE\n";

static char     cmd[16] = {0};
static int32_t  action_freq = 0;
static int32_t  action_clicks = 0;
static int8_t   action_direction = 0;
static uint8_t  forever = 1;

//---------------------------------------------------------
static void setup_ports(void)
{
  // setup ports for output
  DDRD_DDRD7 = 1;
  DDRD_DDRD6 = 1;
  DDRD_DDRD5 = 1;
  // put outputs down
  OUT1 = 0;
  OUT2 = 0;
  LED = 0;
}

//---------------------------------------------------------
// return: 0 - OK, else error
//
static uint8_t get_cmd(void)
{
  static char ch;
  static uint8_t idx = 0;

  while(1)
  {
    ch = uart_receive();
    LED = ~LED;
    if (ch == '\n' || ch == '\r' || ch == '\0')
    {
      cmd[idx] = '\0';
      if (idx == 0)
      {
        cmd[0] = 'p';   // ping
        cmd[1] = '\0';
      }
      idx = 0;
      break;
    }

    cmd[idx] = ch;
    idx++;
    if (idx == sizeof(cmd))
    {
      // слишком длинная команда - игнорируется
      idx = 0;
      return 1;
    }
  }
  return 0;
}

//---------------------------------------------------------
// return: 0 - OK, else error
//
static uint8_t do_cmd(void)
{
  long num = 0;
  if (cmd[0] == 'h')
  {
    uart_println(str_help);
    return 0;
  }

  if (cmd[0] == 'p')
  {
    // ping - pong
    uart_println(msg_ok);
    start_ack_led();
    return 0;
  }

  if (cmd[0] == 'c')
  {
    num = atol(cmd+1);
    if (errno != 0)
    {
      uart_print(msg_fail);
      uart_printch(' ');
      if (errno == ERANGE)
        uart_print(msg_ERANGE);
      else
        uart_print_decimal(errno);
      uart_printch('\n');
      action_clicks = 0;
      return 1;
    }
    action_direction = 0;
    if (num < 0) action_direction = 1;
    num = labs(num);
    if (num > MAXCLICKS)
    {
      uart_println(msg_badclicks);
      action_clicks = 0;
      return 1;
    }
    action_clicks = (num ? num : 1);
    uart_println(msg_ok);
    set_timer_settings(action_freq, action_clicks, action_direction);
    start_ack_led();
    return 0;
  }

  if (cmd[0] == 'f')
  {
    num = atol(cmd+1);
    if (errno != 0)
    {
      uart_print(msg_fail);
      uart_printch(' ');
      if (errno == ERANGE)
        uart_print(msg_ERANGE);
      else
        uart_print_decimal(errno);
      uart_printch('\n');
      action_freq = 0;
      return 1;
    }
    num = labs(num);
    if (num < 1 || num > MAXFREQ)
    {
      uart_println(msg_badfreq);
      action_freq = 0;
      return 1;
    }
    action_freq = num;
    uart_println(msg_ok);
    set_timer_settings(action_freq, action_clicks, action_direction);
    start_ack_led();
    return 0;
  }

  if (cmd[0] == 'i')
  {
    uart_print(str_freq);
    uart_print_decimal(action_freq);
    uart_printch('\n');

    uart_print(str_clicks);
    uart_print_decimal(action_clicks);
    uart_printch('\n');

    uart_print(str_direction);
    uart_println(action_direction ? str_backwd : str_forwd);

    uart_print(str_timer);
    if (TCCR1B & 0x07)
      uart_print(str_on);
    else
      uart_print(str_off);

    uart_printch(':');
    uart_printch('F');
    uart_print_decimal(action_freq);

    uart_printch(':');
    uart_printch('H');
    uart_printch('W');
    uart_print_decimal(g_timer_freq);

    uart_printch(':');
    uart_printch('L');
    uart_print_decimal(g_timer_fullpass & 0x80 ? 0 : g_timer_fullpass);

    uart_printch(':');
    uart_printch('T');
    uart_print_decimal(g_timer_ticks);

    uart_printch(':');
    int32_t _value = g_timer_clicks_counter;
    uart_print_decimal(_value);

    uart_printch('\n');
    start_ack_led();
    return 0;
  }

  if (cmd[0] == 'r')
  {
    uint8_t err = start_encoder();
    if (err == 1)
    {
      uart_println(msg_badfreq);
      return 1;
    }
    if (err == 2)
    {
      uart_println(msg_badclicks);
      return 1;
    }
    uart_println(msg_ok);
    return 0;
  }

  if (cmd[0] == 'e')
  {
    stop_encoder();
    timer_set_done_message(msg_stop);
    start_ack_led();
    return 0;
  }

  uart_println(msg_badcmd);
  return 1;
}

//---------------------------------------------------------
int main(void)
{
  uint8_t err;
  setup_ports();
  setup_timers();
  stop_encoder();
  start_idle_led();

  uart_init(BAUD_9600);
  uart_println(msg_start);

  while(forever)
  {
    err = get_cmd();
    if (err)
    {
      uart_println(msg_fail);
      uart_wait_rx_end();
      continue;
    }

    err = do_cmd();
    if (err)
      uart_wait_rx_end();
  }

  return 0;
}

