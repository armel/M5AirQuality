# M5AirQuality
![basic](https://img.shields.io/badge/M5Stack-BASIC-blue)
![grey](https://img.shields.io/badge/M5Stack-GREY-blue)
![fire](https://img.shields.io/badge/M5Stack-FIRE-orange)
![core2](https://img.shields.io/badge/M5Stack-CORE2-green)
![aws](https://img.shields.io/badge/M5Stack-AWS-orange)
![cores3](https://img.shields.io/badge/M5Stack-CORES3-purple)

![licence](https://img.shields.io/github/license/armel/HAL9000)
![language](https://img.shields.io/github/languages/top/armel/HAL9000)
![size](https://img.shields.io/github/repo-size/armel/HAL9000)
![version](https://img.shields.io/github/v/release/armel/HAL9000)
![activity](https://img.shields.io/github/commit-activity/y/armel/HAL9000)

**Many thanks to them and all my [donors](#donations)🙏🏻** 

This project is a little air quality monitor for M5Stack and Unit CO2 ([SCD40](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/unit/co2/SCD40.pdf)).

# Compilation

First, edit `platformio.ini` file and change `default_envs` target :

- `default_envs = core` for Core, 
- `default_envs = core2` for Core2
- `default_envs = cores3` for Core S3.

You can compile the source code and flash your M5Stack.

# Config

## I2C Port
By default Unit CO2 ([SCD40](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/unit/co2/SCD40.pdf)) use PORT A of your M5Stack. But you can use PORT C (the blue one) if you're using an M5GO2 module for M5Stack Core2 or an M5GO3 for CoreS3. Take a look at file `M5AirQuality.h`, line 22 to change I2C,

- `#define I2C I2C_PORT_A` if you want to use PORT A,
- `#define I2C I2C_PORT_C_M5GO2` if you want to use PORT C (the blue one) on M5GO2 for Core2,
- `#define I2C I2C_PORT_C_M5GO3` if you want to use PORT C (the blue one) on M5GO3 for CoreS3.

## Temperature Unit
You can change the temperature unit. Take a look at file `M5AirQuality.h`, line 23 to change UNIT,

- `#define UNIT CELCIUS` if you want to use degrees CELCIUS,
- `#define UNIT FAHRENHEIT` if you want to use degrees FAHRENHEIT.

## LED Alert
If you're using an M5GO, M5GO2 or M5GO3 module, you can set a CO2 alert level that will light up in red, the module's LEDs to alert you. Take a look at file `M5AirQuality.h`, line 24 to change LED,

- `#define LED 1200` if you want to set CO2 alert to 1200ppm (red if > 1200ppm),
- `#define LED 0` if you want to use de default animation (green < 1000ppm, yellow < 2000ppm, orange < 3000ppm or red).

# Usage

Connect Unit CO2 to port A (I2C).

Use left and right buttons to adjust brightness.

# About M5GO2 module for M5Stack Core2 and port C

It's possible to use PORT C (the blue one) if you're using an M5GO2 module for M5Stack Core2. Take a look at file `functions.h`, lines  _105 ~ 107_ and change this,

```
  // Init I2C
  Wire.begin();          // Port A
  //Wire.begin(14, 13);  // Port C available on M5GO2 for Core2
  //Wire.begin(17, 18);  // Port C available on M5GO3 for CoreS3
```

by this, 

```
  // Init I2C
  //Wire.begin();        // Port A
  Wire.begin(14, 13);    // Port C available on M5GO2 for Core2
  //Wire.begin(17, 18);  // Port C available on M5GO3 for CoreS3
```

# About M5GO3 module for M5Stack CoreS3 and Port C

It's possible to use PORT C (the blue one) if you're using an M5GO3 module for M5Stack CoreS3. Take a look at file `functions.h`, lines  _105 ~ 107_ and change this,

```
  // Init I2C
  Wire.begin();          // Port A
  //Wire.begin(14, 13);  // Port C available on M5GO2 for Core2
  //Wire.begin(17, 18);  // Port C available on M5GO3 for CoreS3
```

by this, 

```
  // Init I2C
  //Wire.begin();        // Port A
  //Wire.begin(14, 13);  // Port C available on M5GO2 for Core2
  Wire.begin(17, 18);    // Port C available on M5GO3 for CoreS3
```

# Credits
 
Many thanks to...

| Project             | Author                                                |  Link                                           |
|:------------------- | :---------------------------------------------------- | :---------------------------------------------- |
| M5Unified           | [Lovyan03](https://twitter.com/lovyan03)              | https://github.com/m5stack/M5Unified            |
| FastLED             | FastLED                                               | https://github.com/FastLED/FastLED              |

# Donations
A big thanks to Ken from [desktopgauges.com](http://desktopgauges.com) !

Special thanks to Rolf Schroeder DL8BAG, Brian Garber WB8AM, Matt B-Wilkinson M6VWM, Robert Agnew KD0TVP, Meinhard Frank Günther DL0CN, Johan Hansson SM0TSC, Tadeusz Pater VA7CPM, Frederic Ulmer F4ESO, Joshua Murray M0JMO, Mark Hammond N8MH, Angel Mateu Muzzio EA4GIG (2 times 🍷🍷), Hiroshi Sasaki JL7KGW, Robert John Williams VK3IE, Mark Bumstead M0IAX, Félix Symann F1VEO, Patrick Ruhl DG2YRP, Michael Beck DH5DAX, Philippe Nicolas F4IQP, Timothy Nustad KD9KHZ, Martin Blanz DL9SAD, Edmund Thompson AE4TQ, Gregory Kiyoi KN6RUQ, Patrick Samson F6GWE, George Kokolakis SV3QUP, Ambrose "Bo" Barry W4GHV, Roger Bouche F1HCN, Christopher Platt, Pascal Paquet F4ICR, Gregory Kiyoi, Ning Yang BH7JAG, Mitsuhiko Nagasawa JL1LYT, Mike Mann G4GOC, David Cappello, Matt Brinkhoff KB0RXC, Franklin Beider WD9GZ, Robrecht Laurens ON4ROB, Florian Wolters DF2ET, James Gatwood WA9JG, Christoph Gässler DL6SEZ, Roger Kenyon WB2YOJ, Jean-pierre Billat F1RXM (2 times 🍷🍷), John Sheppard G4WOD, Jean-Cyrille Vercollier F6IWW, Kenneth Goins Jr, Tim Cowell G6GEI, Andreas Macrides M0FXB, Bogdan Manolache W4MHI and Jacob Mol N8JCM for their donations. That’s so kind of them. Thanks so much 🙏🏻

If you find this project fun and useful then [buy me a glass of wine](https://www.paypal.me/F4HWN) 🍷 🤗 

~~You could use the code F4HWN in order to get 5% discount on the [M5Stack shop](https://shop.m5stack.com/?ref=LUxetaH4) 🎁~~

By the way, you can follow me on [Twitter](https://twitter.com/F4HWN) and post pictures of your installation with your M5Stack. It always makes me happy ;) 
