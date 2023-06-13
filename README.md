# arduino
judovana's set of small arduino projects

## 4digitsAthleticClocks
Four digits clock  counting 90 minutes with granularity of  1 second.
based on:
 * https://www.sparkfun.com/wish_lists/148231
 * https://learn.sparkfun.com/tutorials/large-digit-driver-hookup-guide

## 4digitsAthleticClocks2/clocks.c
Reworked clocks, so insteda of big digits, the WS2812 strips are used to create the numbers
![IMG_20230613_153719](https://github.com/judovana/arduino/assets/2904395/f5ddc7f7-9222-4057-90e2-a7357116640d)
![IMG_20230613_153558](https://github.com/judovana/arduino/assets/2904395/faab41dc-6596-4d06-8f9c-46e733e76fc9)


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
   * In menu, 'A', 'B', 'C' and 'D' will move yo directly to edited number
     * in some setup pages, where some digit is unused, 'its letter' does nothing
 * '#' will exit menu immediately and save settings
 * first to edit is time which is the deadline for clock watch or timer start for count down
   * red is the digit you are modifying now, green reflects current setup
   * if yo **do not edit** it, the **time will persist** as it was, otherwise it will reset
 * '*'will move you to next settings page.
   * iterations - 0-9 where 0 is endless. So you can setup limited amount of iterations. The ending sound can not be turned off.
   * timers - 0-9. You can set aditional different timers to run in row. Eg 3:00 train, 2:00 rest. That is timers-1 (as 0 is default, which is here always)
     * That also mean, you can override maximum of 9 iterations - id you set this to 9, and each time to same, you will have 90 iterations:)
   * +1 for stop-watch (any odd number), -1 for countdown (any pair number)
     * if yo **do not edit** those three, the **time will persist** as it was, otherwise it will reset
     * If you have set more then 0 of additional timers, then you will now jump to submenu, where you will set up individual subtimers
       * you set them up as initial timer (the first one before you entered menu)
       * the counter - which subtimer you are setting up - is now shown in red color in the delimiter strip
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
 *  another '*' will move you to beggining of setup - time
 *  once setup ends, it is saved for next launch (including brigntess!)
