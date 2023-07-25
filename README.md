# MCC22

MPPT Battery charger

| Input Voltage     | 0 ~ 30 V  |
| ----------------: | :-------- |
|   Output Voltage  | 21 ~ 60V  |
| Output Power      | 300 W     |
| Efficiency        | > 90%     |


## 3D Model

<img style="background-color:white"  src="images/top.png" alt="3D" align="center" width="600"/>
<img style="background-color:white"  src="images/bottom.png" alt="3D" align="center" width="600"/>

## Soldered board

<img style="background-color:white"  src="images/real_top.jpeg" alt="Soldered" align="center" width="600"/>
<img style="background-color:white"  src="images/real_bottom.jpeg" alt="Soldered" align="center" width="600"/>

## Testing Guide for the MCC22 Board

Please follow the steps outlined below. In case of any discrepancy from the expected results, halt the test. At each step, power off the board and turn it back on upon completion of the step. The power supply must always be limited to 18V and 100mA. To locate components on the board, use this [link](https://htmlpreview.github.io/?https://github.com/ZeniteSolar/MCC22/blob/main/hardware/main/bom/ibom.html).

## Steps

1. Without any jumpers, power the CAN bus with 18V. The current draw should be approximately 1mA, and on the power pin header (J601) you should have 5V and 3.3V on their respective pins.
2. Connect the STM 3.3V Enable jumper (JP101). The current draw should be approximately 1mA.
3. Connect the STM32 Supply Enable jumper (JP201). The current draw should be approximately 5mA. You should have 3.3V between the pins of capacitors C208 and C210.
4. Connect the STM 5V Enable jumper (JP103). The current draw should be approximately 5mA. You should have 3V across the pins of the capacitor C204.
5. Connect the Analog_Ref jumper (JP203) between the central pin and the 3V_ref. The current draw should be approximately 30mA. You should have 3V across the pins of the capacitor C211.
6. Flash the STM32 (power should be on). When the power is cycled, all LEDs should light up in sequence, except for the Bat OverV LED. There should be a PWM signal on pin 1 of the PWM Enable jumper (JP106).

Please note that failure to meet any of these conditions may indicate a potential problem with the board.



## Tests

All tests are in [this](./tests/tests.md) separated document.
