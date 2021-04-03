# quad-light-controller

## Key Features
* Powered with any USB outlet or battery pack
* Up to 4 separate LED strands, with JST-PH connector
* 30 LEDs per strand (can increase or decrease with NUM_LEDS constant)
* `Individual`, `All`, and `Pattern` mode for multiple ways to control LED color
* Adjust brightness and saturation of LEDs
* Add diffusion film 6 inches from LEDs for glowing effect

## How to Use
LEDs will power on upon USB connection, default mode is `All` mode

- Select LED mode using `Mode` button [`Button #1`]
    1. `All` mode: all four strand colors are set using the first dial [`Dial #1`]
    2. `Individual` mode: each strand color is controlled by the corresponding dial [`Dial #1-#4`]
    3. `Pattern` mode: various patterns can be selected using the `Pattern` button [`Button #2`], you must be in `Pattern` mode to use `Pattern` button (most other functionality is disabled in pattern mode)

- Global Brightness and Saturation (Only in `All` mode or `Individual` mode)
    1. LED brightness can be selected using `Dimmer` button [`Button #3`], there are 10 brightness levels that can be cycled through
    2. LED saturation can be selected using `Misc` button [`Button #4`], there are 10 saturation levels that can be cycled through

## Future Improvements
- Software
    - Global brightness and saturation values are not easily reset - reset these values after mode change
    - Color, rate, or other variable changes in `Pattern` mode using dials or buttons
- Hardware
    - LEDs on PCB for each button to show current status

## License

The hardware of this project is licensed under the [CERN Open Hardware License](https://ohwr.org/cern_ohl_s_v2.txt), and the underlying source code is licensed under the [MIT license](LICENSE.md).
