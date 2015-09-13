# AVR timers support for AVR 8, 128, 168P chips on WinAVR. #

Common way to use, include timers.c and timers.h into your project.
They (in next versions) will include implementation files (timers8.\*, ...).

Atmega168 and Atmega8 now does not supported. Atmega128 tested, seems
to works good.
Usually I put these files into common/ dir and them include
common/timers.c in my project and common/timers.h where I need it.
Another way is to use library archive files (\*.a).

Supports functions such as:

- alarm_action() - setup function as timer handler
- \_alarm() - setup new alarm
- alarm() - the same but wrapped with cli()/sei()
- tclock() - like standard C's clock() but use timer number timer.
- millis() - mseconds from last call
- micros() - useconds from last call
- etc... 

Before use call init_timers() first!

Tested with WinAVR-20100110. 