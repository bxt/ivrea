# Ivrea – a place for my electronics experiments

**Warning:** I have no idea what I'm doing here, so don't use this unless you know what you're doing.

Recently, I started experimenting a bit with electronics, using Arduino, ICs and a ton of LEDs amongst other things. This repository contains information on various [projects I did](#projects--sketches), also including code, as well as a curated list of [resources](#resources) I found useful.

## Projects / Sketches

Here are the projects I did and plan to do.

### Morse

The [morse Arduino code](morse) was my very first project, I wrote the code even before my Arduino arrived, it requires no wiring as it works with the built-in LED when the Arduino is connected to a computer via USB. It reads ASCII from the serial, converts to morse code, and then makes an LED blink accordingly. Of cource you could hook this up to a relay to drive an even bigger light.

### Fade LED array

My second project creates a smooth marquee of LEDs using PWM, and also uses an input from a poti for speed control. This showcases the analog capabilities of the arduino unit. The code in [`fadeLedArray.ino`](fadeLedArray.ino) might require some finetuning depending on the exact LED you use. The wiring is also very easy, just connect the LEDs to the PWP pins and ground with a resistor in series, and the middle pin from the poti to an analog pin. To smooth out the analog readings I used the code from [`readAnalogVoltageSmooth.ino`](readAnalogVoltageSmooth.ino).

![Animation of the fading LEDs](fadeLedArray.gif?raw=true)

### LED piano

The [LED piano](./ledPiano) is a fun and debugging project. The [arduino code](ledPiano/ledPiano.ino) is reading messages from the serial port like `H4`, `L1` etc. to toggle pins high and low. You could just send those from the serial monitor in Adruino IDE, but it's way faster and fun to do it from a browser. This way you can also do it on a phone, or some other computer in the same network. For that, there is [a server written](ledPiano/index.js) with [node.js](https://nodejs.org/), [express](https://expressjs.com/), [ws](https://github.com/websockets/ws) and [serialport](https://serialport.io/) libraries that will accept messages from a websocket connection and forward them to a serial port. It also serves a [basic client file in plain JS](ledPiano/public/index.html) that displays some buttons for control, and also accepts touch and keyboard input. Aside from driving LEDs, this can also be used to fiddle with ICs without having to wire many things and debouncing buttons and so on.

![Video of the leds being controlled by keyboard](ledPiano/ledPiano.gif?raw=true)

### Shift Registers


After using basic LEDs only I wanted to try out the bargraphs and some ICs and started with the 74HC595 8-bit shift register, creating [`shiftRegisters.ino`](shiftRegisters.ino). It can control two shift registers which in turn control bargrahps and show some nice visuals. The project is a bit like [this shift out example](https://www.arduino.cc/en/Tutorial/Foundations/ShiftOut)) except that it generates some patterns and also accepts some commands form serial port to control:

* `I`: toggle wether to show interemediate register states or only latch once all bits are in place
* `D0`, `D1`, `D2`: control delay when shifting out the bits, so you can watch either the sendout or the results
* `R000`–`R999`: control how long each "image" is shown after the serial transmission is complete

The wiring is a bit more elaborate for this one, so I have a [higher-res image showing how to wire things up on a breadboard](shiftRegisters.jpg?raw=true). White is the serial clock, blue is the output latch clock, yellow is the data and a small piece for cascading into the second shift register.

![Video showing bar graphs controlled by two shift registers](shiftRegisters.gif?raw=true)


### Planned projects / ideas

* Programming an EEPROM to drive the shift registers, so no Arduino is needed
* Well, the 8 bit CPU...
* As well as the "reliable data transmissions" series
* Maybe: Using Ardiuno as a general purpose debugger / controller via [firmata](https://github.com/firmata/arduino) and some node / React app, basically the "LED piano" in fancy
* Maybe: A 2048 inspired by [this Arduino code](https://github.com/zhaofengli/2048-arduino/blob/master/board.h) or [this JS code](https://github.com/gabrielecirulli/2048/blob/master/js/game_manager.js)

## Resources

Over time, I collected some useful resources like [instructional videos](#instructional-videos), [references](#references), a [glosary and german translations](#glosary-and-german-translations), info about [sourcing](#sourcing) components.

### Instructional videos

* [A simple guide to electronic components.](https://www.youtube.com/watch?v=6Maq5IyHSuc): pretty cool recap of the vary basics, like what are resistors, capacitors, LEDs, diodes, transistors, explaining Ohm's law, ...
* [Datasheets: 16x2 LCD By Hand (No microcontroller)](https://www.youtube.com/watch?v=cXpeTxC3_A4&feature=emb_title): good example with basic information on breadboard & IC basics, how to read datasheets, pull down/up resistors, button debounce.
* [Ben Eater (channel)](https://www.youtube.com/c/BenEater): This has a lot of great info, as you can tell from the playlist titles «Building an 8-bit breadboard computer!», «Networking tutorial», «Build a 65c02-based computer from scratch
».
* [Don't Take Capacitors for Granted](https://www.youtube.com/watch?v=rr7bPmGTQUk): how to get polarity right.
* [Beginner's Guide to Soldering Electronics](https://www.youtube.com/watch?v=M2Jf8cebwCs): explaining the various options and how poisonous they are.
* [Inside a Small Chinese Electronics Factory - From the Archives](https://www.youtube.com/watch?v=HF0E8WeEUVM&list=LL&index=6): How SMD components were printed on in a factory.

### References

#### 8 bit CPU

* [Ben Eater's website](https://eater.net/) with the [parts list](https://eater.net/8bit/parts) and the [schematics](https://eater.net/8bit/schematics) for the 8 bit computer amongst many other things
* [/r/beneater](https://www.reddit.com/r/beneater) with [lordmonoxide's posts](https://www.reddit.com/r/beneater/comments/dskbug/what_i_have_learned_a_master_list_of_what_to_do/) containing  and [CalliGuy's post](https://www.reddit.com/r/beneater/comments/ii113p/helpful_tips_and_recommendations_for_ben_eaters/) containing lots of gotchas
* [«7400-series integrated circuits» on Wikipedia](https://en.wikipedia.org/wiki/7400-series_integrated_circuits) and the [list](https://en.wikipedia.org/wiki/List_of_7400-series_integrated_circuits) explain numbers like `SN74LS08`, as does [bigger-hammer's post](https://www.reddit.com/r/beneater/comments/js3ozw/8bit_parts_online_aliex/gby3g3l/) and the [futurlec IC74LS00 Series shop page](https://www.futurlec.com/IC74LS00Series.shtml)
* [Logic Signal Voltage Levels](https://www.allaboutcircuits.com/textbook/digital/chpt-3/logic-signal-voltage-levels/) cool comparison between TTL and CMOS / LS and HC from [this post](https://www.reddit.com/r/beneater/comments/j5ixle/can_i_mix_ls_and_hc_logic_gates/), see also [this answer](https://electronics.stackexchange.com/a/358628/268618)
* [A high-res picture of a finished build](https://i.imgur.com/um8YhA2.jpg)

#### Arduino

* [Arduino Foundations](https://www.arduino.cc/en/Tutorial/Foundations) portal into most documentation
* [Arduino Built-In Examples](https://www.arduino.cc/en/Tutorial/BuiltInExamples), the codes form the examples menu but with circuits and explanations
* [Arduino "language" reference](https://www.arduino.cc/reference/en/): with the API and yeah also C++ if you want
* [Arduino nano](https://store.arduino.cc/arduino-nano), page about the board type I'm using, check the tech specs, docs and FAQ tabs as well
* [JOY-IT nano V3](https://joy-it.net/de/products/ARD_NanoV3), the actual "Adruino-compatible" board I'm using, check the driver download and "ATmega328P (Old Bootloader)" instructions
* [PROGMEM](https://www.arduino.cc/reference/en/language/variables/utilities/progmem/) and [AVR PGM Program Space Utilities](https://www.nongnu.org/avr-libc/user-manual/group__avr__pgmspace.html)
* There's the [Arduino memory](https://www.arduino.cc/en/Tutorial/Foundations/Memory) page, but really think: Flash ~ CD with program, SRAM ~ RAM – duh, EEPROM ~ hard disk.

#### General

* [ASCII table](https://de.wikipedia.org/wiki/American_Standard_Code_for_Information_Interchange#ASCII-Tabelle) for converting bits / numbers to letters and back manually
* Wikipedia articles about [ICs](https://en.wikipedia.org/wiki/Integrated_circuit)
* [How to Use a Breadboard](https://learn.sparkfun.com/tutorials/how-to-use-a-breadboard/all) compehensive tutorial about the very basics
* Also in the shopping list below you can usually find the data sheets about the particular components

### Glosary and German translations

* `V` / Voltage in volt, de: Spannung in Volt
* `I` / Current in amps / amperes, de: Strom / Stromstärke in Ampere
* `R` / Resistor in ohms, de: Wiederstand in Ohm
* Capacitor, de: Kondensator
* Resistor, de: Widerstand
* Solderless THT / through-hole technology / Breadboard / dual-in-line / DIP packaging / 0.1 inch, de: Steckboard, 2,54 mm
* THT (not Solderless but also DIP sized hole distances), de: Lochrasterplatine
* SMD / Surface-mounted device / PCP / "the very small chips", de: Platine, Leiterplatte
* VCC / VDD / "5V", de: Pluspol, Spannungsquelle
* GND / Ground / VSS / 0V, de: Minuspol, Masse

### Sourcing

#### Shops

[Reichelt](https://www.reichelt.de/), [Mouser](https://www.mouser.de/), [Conrad](https://www.conrad.de/), [Pollin](https://www.pollin.de/), [Amazon](https://www.amazon.de/), ...

#### Shopping list

You can just buy the [kit from Ben directly](https://eater.net/8bit/kits) which I would recommend. But I was not sure about shipping to Germany (expecially with taxes) and I wanted some extra / custom parts, so I procured everything indidually. I'm not sure whether I have everything, and also some things are not 100% compatible and will require some tinkering, but for me it's useful to have a list of the exact parts in my inventory and maybe someone else can buy similar things.

##### Tools and supplies

* USB power supply, Screwdriver, wire cutters, pilers, and box cutter I already had
* [Wire stripper from Jokari](https://www.amazon.de/-/en/gp/product/B002BDNL4Q)
* [Digital Multimeter from Crenova](https://www.amazon.de/-/en/gp/product/B01825GBK2)
* [22 AWG solid wire kit](https://www.amazon.de/-/en/gp/product/B07V5FVSYL)
* [Jumper wire kit](https://www.reichelt.de/flexible-drahtbruecken-15-cm-mehrfarbig-65er-pack-rnd-255-00009-p253202.html), [Bus jumper wires](https://www.amazon.de/AZDelivery-Jumper-Arduino-Raspberry-Breadboard/dp/B074P726ZR)
* Breadboards: [4k7v](https://www.reichelt.de/experimentier-steckboard-2560-700-kontakte-steckboard-4k7v-p67683.html), [BB830](https://www.mouser.de/ProductDetail/BusBoard-Prototype-Systems/BB830?qs=VEfmQw3KOauhPeTwYxNCaA%3D%3D), [BB830T](https://www.mouser.de/ProductDetail/BusBoard-Prototype-Systems/BB830T?qs=VEfmQw3KOauXY1NKV2FuEg%3D%3D)
* [Header pins](https://www.reichelt.de/stiftleiste-3-polig-raster-2-54mm-bkl-10120200-p235647.html)
* [ESD-15 tweezers, screwdriver, ...](https://www.amazon.de/gp/product/B07XTCJGR3)
* Soldering station: is on my wishlist

##### Basic components

* [Resistors](https://www.amazon.de/-/en/gp/product/B08DKT3STN)
* [Electrolytic capacitors](https://www.amazon.de/-/en/gp/product/B07NWGDH2T)
* [Ceramic capacitors](https://www.amazon.de/gp/product/B07PP7SFY8) and [0.01µF ones](https://www.reichelt.de/tantalkondensator-100-nf-35-v-avx-tap104m035cc-p246447.html)
* [1MΩ potentiometer](https://www.reichelt.de/drehpotentiometer-mono-1-0-mohm-4mm-linear-po4m-lin-1-0m-p88275.html)
* [Slide switch](https://www.reichelt.de/schiebeschalter-gerade-rm5-08-1x-ein-ein-ss-25436-nh-p105713.html)
* DIP switch [4-position](https://www.reichelt.de/dip-schalter-printmontage-liegend-4-polig-rnd-210-00164-p226385.html), [8-position](https://www.reichelt.de/dip-schalter-printmontage-liegend-8-polig-rnd-210-00166-p226387.html)
* Momentary tact pushbutton/switch: ordered various, will pick one to list later

##### LEDs

* [LEDs sortiment](https://www.reichelt.de/led-sortiment-5-mm-superhell-vorwiderstaende-fassungen-quad-1801o004-p280142.html)
* [7-segment LED displays, common cathode](https://www.reichelt.de/7-segment-anzeige-superrot-14-2mm-gem-kathode-sc-56-11-srt-p80388.html)
* Bargraph [yellow](https://www.reichelt.de/bargraph-anzeige-10-elemente-gelb-ybg-1000-p22957.html), [green](https://www.reichelt.de/bargraph-anzeige-10-elemente-gruen-gbg-1000-p8141.html), [green/red](https://www.reichelt.de/bargraph-anzeige-10-elemente-3x-rot-7x-gruen-rgbg-1000-p31501.html)
* 5V LEDs with resistor: [green](https://www.reichelt.de/led-5-mm-bedrahtet-5-v-gruen-20-mcd-30--kbt-l-7113gd-5v-p231091.html), [yellow](https://www.reichelt.de/led-5-mm-bedrahtet-5-v-gelb-20-mcd-30--kbt-l-7113yd-5v-p231097.html), [red](https://www.reichelt.de/led-5-mm-bedrahtet-5-v-rot-30-mcd-30--kbt-l-7113id-5v-p231093.html)
* [128x64 OLED screen](https://www.reichelt.de/entwicklerboards-display-0-96-oled-display-ssd1306-debo-oled2-0-96-p266107.html) ([SBC-OLED01](https://joy-it.net/de/products/SBC-OLED01), [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306), [GFX](https://learn.adafruit.com/adafruit-gfx-graphics-library))
* LED matrix / LED strip: are on my wishlist

##### Bigger ICs

* [Arduino nano clone](https://www.reichelt.de/arduino-kompatibles-nano-board-atmega328-mini-usb-ard-nano-v3-p225690.html)
* ["74189 64-bit RAM"](https://www.reichelt.de/sram-32-kb-4-k-x-8-4-5--5-5-v-dil-28-62256-80-p2673.html) **(I only got a bigger one, maybe [this post and links there](https://www.reddit.com/r/beneater/comments/hon6ar/74189_alternative/) help)**
* [28C16 EEPROMs](https://www.reichelt.de/eeprom-64-kb-8-k-x-8-5-v-150-ns-pdip-28-28c64-150-p1946.html) and [mount](https://www.reichelt.de/praezisionsfassung-28-polig-rm-2-54-mpe-001-2-028-3-p213785.html) (the 16K ones are unavailable, so I got 64K)
* [LM-555 Timer ICs](https://www.reichelt.de/timer-ic-typ-555-dip-8-lm-555-cn-p187567.html) and [mount](https://www.reichelt.de/praezisionsfassung-8-polig-rm-2-54-mpe-001-1-008-3-p213777.html)
* [Arduino UNO R3 clone with starter kit](https://www.amazon.de/gp/product/B014L11IN6)

##### 7400er ICs

* [74LS04 Hex inverter](https://www.reichelt.de/inverter-hex-4-5--5-5-v-dil-14-74hct-04-p3306.html) 5×
* [74LS08 Quad 2-input AND gate](https://www.reichelt.de/and-gate-2-element-4-5--5-5-v-dil-14-74hct-08-p3307.html) 3×
* [74LS00 quad NAND gate](https://www.reichelt.de/nand-gate-2-input-4-5--5-5-v-dil-14-74hct-00-p3303.html) 2×
* [74LS02 quad NOR gate](https://www.reichelt.de/nor-gate-2-input-4-5--5-5-v-dil-14-74hct-02-p3304.html) 1×
* [74LS32 Quad 2-input OR gate](https://www.reichelt.de/4-fach-oder-gatter-je-2-eingaenge-4-5--5-5-v-dil-14-74hct-32-p3369.html) 1×
* [74LS86 Quad XOR gate](https://www.reichelt.de/xor-gate-2-input-4-75--5-25-v-dil-14-ls-86-p10826.html) 2×
* [74LS157 quad 2-1 line mux](https://www.reichelt.de/multiplexer-lp-schottky-4-75--5-25-v-dil-16-ls-157-p10625.html) 4×
* [74LS107 dual J-K flip-flop](https://www.reichelt.de/flip-flop-d-type-dual-4-5--5-5-v-dil-14-74hct-107-p3309.html) 1×
* [74LS138 3-to-8 line decoder](https://www.reichelt.de/decoder-3-to-8-4-5--5-5-v-dil-16-74hct-138-p3318.html) 1×
* [74LS139 2-to-4 line decoder](https://www.reichelt.de/dekoder-demultiplexer-2-auf-4-4-5--5-5-v-dip-16-74hct-139-p3319.html) 1×
* [74LS161 4-bit binary counter](https://www.reichelt.de/counter-4-bit-4-75--5-25-v-dil-16-ls-163-p10630.html) 2× **(I only got the 163 with slightly different specs)**
* [74LS173 4-bit D register](https://www.mouser.de/ProductDetail/Texas-Instruments/SN74LS173AN?qs=nMmhAzRCgdAkY4Cck6ihbQ%3D%3D) 8×
* [74LS273 8-bit D register](https://www.reichelt.de/flip-flop-d-type-octal-4-5--5-5-v-dil-20-74hct-273-p3363.html) 1×
* [74HC595 8-bit shift register](https://www.reichelt.de/shift-register-3-state-ausgang-2--6-v-dil-16-74hc-595-p3269.html) 2×
* [74LS245 8-bit bus transceiver](https://www.reichelt.de/octal-bus-puffer-3-state-4-5--5-5-v-dil-20-74hct-245-p3356.html) 6×
* [74LS283 4-bit binary adder](https://www.mouser.de/ProductDetail/Texas-Instruments/SN74LS283N?qs=RnzODY3cU8uVeiPelRVWjw%3D%3D) 2×

