# arduino
judovana's set of small arduino projects

## 4digitsAthleticClocks
Four digits clock  counting 90 minutes with granularity of  1 second.
based on:
 * https://www.sparkfun.com/wish_lists/148231
 * https://learn.sparkfun.com/tutorials/large-digit-driver-hookup-guide

## 4digitsAthleticClocks2/clocks.c
Reworked clocks, so insteda of big digits, the WS2812 strips are used to create the numbers

## FreeBoard/WS2812blinks
Controller for https://github.com/judovana/FlashFreeBoardDesktop

## globus/globus.c
It just programmed one led, to be glued to battery, and then light for ever in the middle of puzzle globe.

## musicInstrument
Demo for keybard + speaker

## v3 are fully controllable gym clocks
 * 'A' pause. Unpause by any key.
 * 'D' reset - hard to press. If you are paused, then reset is easy to press/.
 * '*' will enter the menu. The time is stopped now. It is intentionally ahrd to press simialry as reset.
 * '#' will exit menu immediately and save settings
 * now you can edit time which is the deadline for clock watch or timer start for count down
   *  red is the digit yo are modyfying now, green reflects current setup
   *  if yo do not edit it, the time will persits as it was, otherwise it will reset
 *  '*'wil move you to next settings page.
   *  +1 for stop-watch (any odd number), -1 for contdown (any pair number)
 *  '*'wil move you to next settings page - brightness
    *  as time, you can edit brightness of leds
    *  it is 0-250
    *  be aware!  if yo use 0, the leds will be off, and you are on audio only!
    *   the firt number do not belong to the brightnes, but is flag 0/1 - if on 0, the brightness will not be saved, but always reset.
      *   That is another precaution to unintentionally blank your s clock for ever
 *  (todo, iterations, breaks, sound settings (1/2, 1/3, 3,2,1) and so on...)
 *  another '*' will move you to beggining of setup - time
 *  once setup ends, it is saved for next launch (including brigntess!)
