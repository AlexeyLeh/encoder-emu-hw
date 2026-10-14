# Encoder Emulator Hardware Part

## Description

A device for generating a sequence of square-wave pulses.

The direction is determined by the sequence in which signals appear on the two outputs.

Key operating parameters:

* Pulse output speed (set by frequency)
* Number of pulses

Implemented on Arduino UNO (ATmega328P chip).

## Control

Controlled via COM port (COM 9600) by sending commands.

### Setting parameters

Number of steps (pulses):

* c100\n - 100 steps forward
* c-200\n - 200 steps backward

Pulse frequency (set in Hz):

* f1000\n - 1000 Hz

Get current parameters:

* i
```
FREQ:7250
CLICKS:100000000
WAY:>
TIMER:ON:F7250:HW29000:L0:T64985:98178369
```

### Execution

* r\n - start
* e\n - stop

### Command reference

```
$ h
Encoder emulator v1.0 Jan 20 2026 (Jan 20 2026 14:56:00)
CMD:
c [-]<1-100000000> - clicks
f <1-16000> - freq
i - info
p - ping
r - run
e - stop
ret: START DONE OK FAIL BADCMD BADFREQ BADCLICKS ERANGE";

```