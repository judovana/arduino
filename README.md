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
 * 'A' pause. Unpause by any key. Except 'D'
   * while in pause, 'D' will reset the time
 * '*' will enter the menu. The time is stopped now.
   * another '*' turn page in menu.
   * position in setup is visible on middle delimiter strip
 * '#' will exit menu immediately and save settings
 * first to edit is time which is the deadline for clock watch or timer start for count down
   * red is the digit you are modifying now, green reflects current setup
   * if yo **do not edit** it, the **time will persist** as it was, otherwise it will reset
 * '*'will move you to next settings page.
   * +1 for stop-watch (any odd number), -1 for countdown (any pair number)
 * '*'will move you to next settings page - brightness
    * as time, you can edit brightness of LEDs
    * it is 0-250
    * be aware!  if yo use 0, the LEDs will be off, and you are on audio only!
    * the first number do not belong to the brightnes, but is flag 0/1 - if on 0, the brightness will not be saved, but always reset.
      * That is another precaution to not unintentionally blank your s clock for ever
 * '*'will move you to next settings page - sound 1/2
    * first to edit is pitch bigger the number, more annoying the sounds are
      * it is 0-9
    * last two digits are stereo
      * L...R ; canbe 0 or 1, 1 is for on
      * 0/1 0/1
      * if you put both to 0, you have finally silenced the clocks
 * '*'will move you to next settings page - sound 2/2
  * final long beep once timer/clock reaches end
  * 1/2 sound
  * 2/3 sound
  * 3,2,1 sounds
 *  (todo, iterations, multiple times and so on...)
 *  another '*' will move you to beggining of setup - time
 *  once setup ends, it is saved for next launch (including brigntess!)
