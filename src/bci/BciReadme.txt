=====
 BCI
=====

This application is the Boltwood Cloud Interface (BCI). The application connects
to a virtual serial port to communicate with a Boltwood Cloud Sensor, and 
outputs a network weather file suitable for serving to the WMS/RCS. 

Usage: Bci <device file> {<output path> | reset} {raw file}
   <device file>              Name of device file for port
   <output path>              Path for output files
   reset                      Issue reset to device then terminate
   <raw file>                 Path/name of raw data output file

Example :-
# ./Bci /dev/BoltwoodExt /ttl/sw/data /ttl/sw/data/WmsBoltwoodExt.raw

In the above example the virtual serial port is mapped to '/dev/BoltwoodExt'.
This will create two files in '/ttl/sw/data' (with examples) :-
- WmsBoltwoodExt.txt - 2008,037,163807,25.5,29.0,6.4,-999.0,0,999.0
- RcsBoltwoodExt.txt - 2008-037T163807 25.5 29.0 6.4 -999.0 0 999.0
The raw file is optional and may be used to debug the interface by logging the 
raw status coming from the device, however the file will get large fairly 
quickly.

The WMS weather file contains the following fields separated by commas :-
- year (YYYY)
- day (0-365)
- time (HHMMSS)
- temperature (°C)
- humidity (%)
- dew point (°C)
- wind speed (m/s), outputs -999.0 when sensor is wet
- wetness (boolean)
- sky-ambient temperature (°C), outputs < -998.0 when sensor is wet or saturated

The RCS weather file contains the following fields separated by spaces :-
- timestamp (YYYY-DDDTHHMMSS)
- temperature (°C)
- humidity (%)
- dew point (°C)
- wind speed (m/s), outputs -999.0 when sensor is wet
- wetness (boolean)
- sky-ambient temperature (°C), outputs < -998.0 when sensor is wet or saturated

The protocol details as recieved from Boltwood are pasted further below.


History:

   BCI_1_04
   Interface reworked to sleep for 0.1 seconds between attempts to receive 
   incoming data. Don't send 'request' message after each received status, but
   send when no incoming status has been received for 5 seconds.

   BCI_1_03
   Added trap for non-existence of output directory.

   BCI_1_02
   Wind speed converted from km/hr into m/s.

   BCI_1_01
   Command-line arguments revised to specify an output path rather than an 
   output filename. The respective output filenames are then constructed using
   the device name. A separate file is also now created in a format for the RCS.

   BCI_1_00
   First functional version of BCI application. 
   Not yet implemented :-
   a) reset command to the device
   b) command to send ASCII string as required for Boltwood 
   c) commands to re-configure the thresholds for the device
   We will probably never need c) as the WMS/RCS will do the thesholding.


Files:

   See Bci.lis


Documentation:

   -


Notes:

   -


29-Feb-2008, MJF.

=====

PROTOCOL.TXT - CSII protocol  20071029
Copyright (C) 2007 Boltwood Systems Corporation


This documents is excerpts from the comments in the internal source code for CSII.
Study the CSII User's Manual first.  Examine log files from Clarity II.

This document is not supported.  

If you wish any support of the CSII product itself
your software must provide a log file the same way and format as the one ClarityII does 
including the memory dump capability.  This document plus a current log file from ClarityII 
should allow you to figure out the log file format.  Everything on the link is ASCII
except for some use of "breaks".



HARDWARE
========

UART in the sensor head CPU uses an RS485 link to the adaptor box.  
Async 4800 BPS, 8 data, 1 stop, no parity.

The PC software needs to be able to force a sensor head reset:
    ' The sensor head uses a 16 bit ripple counter running from 8 MHz to
    ' implement the hardware reset.  While the RXD line from the RS485
    ' interface chip is high, the ripple counter is held in reset.  When
    ' the RXD line goes low, the counter counts up.  At a count of 32768
    ' the ATMEGA32's reset line is activated and 1.5us later the reset occurs.
    ' To avoid multiple resets RXD needs to go high before the counter
    ' overflows and reaches 32768 again.  That is 65536 counts after the
    ' reset is initially generated.
    '
    ' The minimum RXD low pulse is 32768/8,000,000 + 1.5 us = 4097.5 us.
    ' The maximum allowed is 3*32768/8,000,000 = 12,288 us.
    '
    ' The  minimum is 4.917 bit times at 1200 BPS.  The maximum is 14.74
    ' bit times.  So sending a single null character at 1200 BPS will do
    ' the reset with a pulse that is 9/1200 = 7500 us.

    1200 BPS is not the normal data rate on the link.



ADAPTOR BOX HARDWARE
====================

FTDI FT232RL USB interface chip in the adaptor box converts RS485 to USB.  The programmable
features of this chip are all left at the factory defaults.  We use FTDI's WXP
drivers that make the USB look like a serial port.  Contact FTDI for chip info.



SENSOR HEAD FIRMWARE PROTOCOL MODULE
====================================

/* protocol.c - Frame protocol handler for Cloud Sensor 2  20071129
   Copyright (C) 2005 Boltwood Systems Corporation */

#include <stdafx.h>  



/*
The origional protocol for this link had to be enhanced to get around a problem
with the FTDI RS485 to USB interface chip FT232RL.  The problem was that if
there was data coming in from the sensor head when the USB link was starting
up, the sensor head was considered to be a mouse.  It then was placed in
parallel with the regular Windows mouse thus causing a mess.

What is needed to avoid this problem is to have the sensor head stop talking
soon (10 seconds later is enough) after the PC stops talking.  Doing that 
ensures that if the PC is rebooted, when Windows starts up the USB drivers 
there is no USB traffic.

Stopping the sensor head from talking is achieved by having the PC send 
"request" bytes (SOH or 0x01) occasionally when it is active.  The lack of these 
request bytes gradually causes the sensor head to stop sending.

To avoid collisions between these request bytes and the normal traffic, they are
sent when the normal traffic does not occur.  This is done by the PC sending a 
request byte whenever the PC completes receiving a message and has just 
acknowledged it, or when a poll from the sensor head has just been acknowledged. 

Each time the sensor head receives a request byte, it resets a counter 
(requestflag) to 5.  This happens at the start of both the SendMsg and Poll
functions.  Each time the sensor head sends a message it decrements 
this counter.  If the counter gets to 0, messages are not sent (and are discarded), 
and polls are not generated.

While the Clarity II program is operating and the COM port is open, a 
request is sent on a 10 second timer if there is no traffic on the link.
This allows for recovery when a sensor head powers up.



Except for the "request" described above, the Cloud Sensor's ATMEGA 32 is the 
"master" in this protocol.  For the PC to send a message, it must be polled
by the ATMEGA.  This is necessary because the RS485 link is half-duplex and 
collisions must be minimized.

This protocol does not guarantee message delivery in either direction.  It does
guarantee that any message delivered is correct (to the extent that a 16 bit CRC 
can).  If it tells the caller that a message was delivered, it really was.  However
there is a possible fault where the caller will be told that the message was not
delivered when in fact it was delivered.  If better performance than this
is required, then another higher protocol layer is required to provide it.

The PC must accept all frames sent to it.  There is no handshaking provided
that could be used to slow down the traffic.

The CS2 is master and it sends a message ordering PC to purge its input buffer if any
problem occurs.  The purge order an ASCII character sequence otherwise not used 
(!**!x) where the x is an identifying character for the cause of the purge. 


All frames are in ASCII, not binary.  Each frame starts with STX (0x02)
and ends with a newline character '\n'.  Command frames contain a 
single ASCII character after the STX.  It is in lower case when sent by the PC 
and upper case when sent by the CS2.  This is to make debugging 
logs easier to read.

The format of the command frames is:
- STX
- ASCII command character
- '\n'
  
The format of the frame that carries a message is:
- STX
- 'M' or 'm'
- body of the message in ASCII printable characters
- 4 hex/ASCII characters (16 bits worth) of CRC-16.  The CRC does not include 
  the STX, 'M', 'm', '\n', or the CRC.
- '\n'
 
           
Two possible communications scenarios as seen at the CS2 (master) end:

1. logical Poll(void) - to receive data from the PC and do housekeeping.

This function
polls the PC once to see if it has a message for us.  The function result is
YES if a message is successfully input to a_msg, NO if not.  Any 
received message will start at a_msg and be terminated with '\0'.  This
function will not return until the poll or its failure is complete.

- the CS2 sends command frame 'P' to poll the PC.  Then one of the items 
  below occurs:
  - the PC sends command frame 'a' to acknowledge the poll (i.e. there is 
    no message frame to be sent by the PC).

  - the PC sends a message frame 'm'.  Then one of the items below occurs:
    - the CS2 sends command frame 'A' as an acknowledgement if the message 
      frame was received properly.  Poll returns YES with the message at a_msg.

    - the CS2 sends command frame 'N' as a negative acknowledgement if there 
      was a CRC error.  The PC application then knows to queue the message to be 
      sent again, or to drop it.  Poll returns NO.  The computed CRC is sent
      after the 'N' so it can be examined.

    - the PC times out waiting for the ack/nak and queues the message to be sent 
      at another poll, or drops it.

    - any other type of frame from the PC results in a NO returned.

  - the PC does nothing due some problem, being off, or Microsoft 
    screwup.  Then the CS2 times out in 10 sec. and sends a purge frame (!**!x).  This
    timeout implies that under all normal circumstances the PC must respond to
    CS2 frames in well under 10 sec.  There are tighter timings as well on the
    parts of the message frame from the PC.  9 sec after the 'P' frame is sent,
    is allowed for the arrival of the STX from the PC.

We also have the situation at the PC end where it has a message to send to the CS2
and no command frame 'P' comes from the CS2 before the PC times out.  If that
happens the PC drops the message unsent.


2. logical SendMsg(void) - to send data to the PC.

Store the message starting at a_msg before calling this.  
Terminate the message with a '\0'.  This call returns YES if the message is sent 
successfully.  If NO is returned, the message still might have been sent successfully
but the acknowledgement was not received properly.  Usually NO means that
the message was not received by the PC.

- the CS2 converts the message into a message frame with header and CRC.

- the CS2 sends a message frame 'M' to the PC.  Then one 
  of the items below occurs:
    - the PC sends command frame 'a' as an acknowledgement if the message 
      frame was received properly.  If this is received by the CS2, YES is returned.

    - the PC sends command frame 'n' if there was a CRC error.  If this is received
      the CS2, NO is returned.

    - the CS2 times out in .1 sec and NO is returned.         


No frame starts with '^'.  That is reserved for the boot loader.  If
the regular PC software receives this, it ignores it.

The CS2 might send the PC a crash dump.  The format of that frame is documented
in STARTUP.ASM.  It starts with a '\', then has a lot of ASCII text, and ends with a 
newline '\a'.



STARTUP.ASM IN THE BOOT LOADER
==============================

; STARTUP.ASM for Cloud Sensor 2 boot loader on the Atmel ATMEGA32   20061012
; Copyright (C) 2006 Boltwood Systems Corporation  (613) 836-6462

This starts with a RAM dump if the previous sensor head CPU run crashed.

; After the PC request 01, a '!' is sent to tell the PC to stop making requests.
; Then there is a delay followed by the dump.
; The dump consists of a continuous stream of bytes:
;    - first is 0x7c ('|').  Then
;    - 0x800 bytes of RAM data sent as 0x800 pairs of ASCII hex digits for addresses
;      0x0060 to 0x085F.
;    - last byte is 0x0a.

  

FIRMWARE BACKGROUND CODE
========================

This starts with the initialization of all of the hardware and firmware.  It then
sends the PC a message giving the serial number, firmware version, and debug details
in case a RAM dump was done.  This announces to the PC that the Cloud Sensor
has just come out of reset.

Then a main loop starts that runs every 2.097152 seconds.  This is 2**24 CPU clocks
at 8 MHz.

The loop does:
   - an optional dump of wetness oscillator counts
   - reads the A/D channels
   - reads out other hardware and computes values
   - operates the heaters
   - computes the sky condition
   - sends a report message to the PC
   - every 50 loops sends a report message to the PC containing serial number,
     version, and calibration data
   - then while waiting for the wetness detector cycle to complete it polls
     the PC for any commands.  It executes any that arrive.




PC COMMANDS TO THE SENSOR HEAD THAT MUST BE IMPLEMENTED
=======================================================

The factory commands have been removed from this list.  Some of the factory
commands are capable of destroying the sensor head.  Do not fool around
with these commands.  Using ClarityII protects you from this danger.

There needs to be an interface where any ASCII string can be sent to the
sensor head on phone or email instruction by Boltwood Systems for
servicing purposes.

NORMAL COMMANDS (are single ASCII letters)

"c"
generates 3 reports:
    Report the serial number, firmware version, esenderrs, and thresholds.
       "T %05u %05u %05u %5.1f %5.1f %5.1f  %5.1f %5d %5d %3d %5d %5d", 
               serialnumber, version, esenderrs,
               ecloudythresh, everycloudythresh, ewindythresh, everywindythresh, 
               erainthresh, ewetthresh, 
               edaylightcode, edaythresh, everydaythresh);

    Report the calibration data for the wetness oscillator in a "K" message.
       "K %1d %5.3f %5u %4.1f %4.1f %5u", 
               ewetcal, ewetoscfactor, erawwetavg, ecaset, eshtambientt, enomosc);

    Report thermopile calibration values to the PC in a "C" message.
       "C %1d %10.3e %5.3f %5.2f",
               ethermopilecal, ebestK, ebestd, ebestoffs);


"l"
Report wetness oscillator calibration values to the PC in a "K" message.


"t cloudythresh verycloudythresh windythresh rainthresh wetthresh"
Threshold settings.
cloudythresh         relative sky temperature threshold in deg.C *10 given as an integer
verycloudythresh     relative sky temperature threshold in deg.C *10 given as an integer
windythresh          too windy threshold in km/hr *10 given as an integer
rainthresh           minimum drop of oscillator count in 16.384 msec required to declare 
                     a rain drop
wetthresh            minimum drop of oscillator count in 16.384 msec from dry required 
                     to declare the wetness sensor as wet

"t"
restores the defaults.


"b"
Roof close.
Send a break to the PC.  This will cause the adaptor box to close the roof.
This could be used by PC software such as ACP.  Note that it requires a completely
functioning Cloud Sensor II to work.  A more direct approach might be safer.



MESSAGE TYPES SENT TO PC
========================

NORMAL OPERATION

   Report the reason for the reset.
   "I %05u %05u 0x%02x %03u 0x%02x %05u",
   serialnumber, version, mcucsrv, crashcode, lastcyc, esenderrs);


   Every 50 reports, send ID and calibration data.
   Report the serial number, firmware version, esenderrs, and thresholds.
   "T %05u %05u %05u %5.1f %5.1f %5.1f  %5.1f %5d %5d %3d %5d %5d", 
   serialnumber, version, esenderrs,
   ecloudythresh, everycloudythresh, ewindythresh, everywindythresh, 
   erainthresh, ewetthresh, 
   edaylightcode, edaythresh, everydaythresh);


   Report the calibration data for the wetness oscillator.
   "K %1d %5.3f %5u %4.1f %4.1f %5u", 
   ewetcal, ewetoscfactor, erawwetavg, ecaset, eshtambientt, enomosc);


   Report the calibration data for the thermopile.
   "C %1d %10.3e %5.3f %5.2f %10.3e",
   ethermopilecal, ebestK, ebestd, ebestoffs);


   Send the sensor data to the PC.
   "D %1u %1u %6.1f %5.1f %3d ... 
   detailed below.

   Report a command received from the PC.
   "Q ~%s~", a_inmsg);

   Roof close.
   "R"




DEBUGGING AND DEVELOPMENT OPERATION

   Dump wetness oscillator data. 
   "W %5u %5u", ewetoscdry, erawwetavg);
   The line has wetness data appended.  See the code at the top of the tmain.c main loop.


   Control of the rain heater.
   "X rhr %d %f", rhr, rainheatt);


   Directly setting of the rain heater target temperature.
   "Y %f %d %d", rainheatt, suppressrh, suppressrrh);


   Directly setting of the rain heater PWM hardware value.
   "Z %d %d %d", RAINHEAT, suppressrh, suppressrrh);



D RECORD FORMAT for normal sensor data

All temperatures are Celsius and speeds are km/hr.

A heading line and then a sample line:
D E C W R 1 c    SKY   AMB  WIND w r HUM   DEW  CASE HEA  BLKT H  PWR WNDTD WDROP  WAVG  WDRY RHT AHT ASKY ACSE APSV ABLK AWND AVNE DKMPH VNE RWOSC D ADAY
D 0 1 1 1 1 0  -39.1   0.8   2.5 N N  79  -2.4   8.7   0 -99.9 0 24.3  20.0     2     4     0 000 170 0200 0705 0955 1023 0385 0916   0.0 322 13649 1 0056

D     record type
E     SHT15 humidistat and ambient temperature sensor error code: 
        For firmware versions < 54:
         0 = OK, 
         1 = write failure for humidity, 
         2 = measurement never finished for humidity.
         3 = write failure for ambient, 
         4 = measurement never finished for ambient.

        For firmware versions >= 54:
           0 = OK, 
           1 = write failure for humidity, 
           2 = measurement never finished for humidity,
           3 = write failure for ambient, 
           4 = measurement never finished for ambient,
           5 = data line was not high for humidity, 
           6 = data line was not high for ambient. 
C     CloudCond value
W     WindCond value
R     RainCond value
1     SkyCond value for Cloud Sensor I programs
c     =0 normally, =1 if roof close was requested on this cycle
SKY   sky-ambient temperature, 999.9 saturated hot, -999.9 saturated cold, or -998.0 for wet
AMB   ambient temperature
WIND  wind speed or:
         -1. if still heating up, 
         -2. if wet,
         -3. if the A/D from the wind probe is bad, 
         -4. if the probe is not heating.
w     'N' when dry, 'W' when wet now, 'w' when wet in last minute
r     'N' when no rain, 'R' when rain drops hit on this cycle, 'r' for drops in last minute
HUM   relative humidity in %
DEW   dew point temperature
CASE  thermopile case temperature, 999.9 saturated hot, -99.9 saturated cold
HEA   rain heater percentage
BLKT  calibration black body temperature (factory only), 999.9 saturated hot, 
      -99.9 saturated cold
H     rain heater control state when directly controlled:
         =0 if too hot,
         =1 if at or nearly at requested temp.,
         =2 to 5 if too cold,  
         =6 if cannot control due to a saturated case temperature (causes shutdown).  
         =7 is used by tmain to indicate that normal control is being used instead 
            of direct use of this
PWR   voltage actually on the +24V at the sensor head
WNDTD anemometer tip temperature difference from ambient, limited by reducing 
      anemometer heater power when 25C is reached
WDROP maximum drop in wetness oscillator counts this cycle due to rain drops
WAVG  wetness oscillator count difference from base dry value
WDRY  wetness oscillator count difference for current dry from base dry value
RHT   rain heater PWM value
AHT   anemometer heater PWM value
ASKY  thermopile raw A/D output
ACSE  thermopile thermistor raw A/D output
APSV  power supply voltage monitor raw A/D output
ABLK  calibration block thermistor raw A/D output
AWND  anemometer tip thermistor raw A/D output
AVNE  Davis vane raw A/D output (only for factory calibration)
DKMPH external anemometer used (only for factory calibration)
VNE   external anemometer wind direction (only for factory calibration)
RWOSC raw counts from the wetness oscillator
D     DayCond value
ADAY  daylight photodiode raw A/D output

   
