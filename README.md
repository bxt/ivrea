# ivrea

A place for my electronics and Arduino experiments. Warning: I have no idea what I'm doing here, so don't use this unless you know what you're doing.

## Projects / Sketches

These are the projects I did and plan to do:

* Planned: Make an LED blink
* Planned: Control bargraph with poti?
* In progress: [Morse](./morse): reading ASCII form serial, converting to morse, and making an LED blink
* Planned: Control multiple bargrahps with shift registers with nice visuals? (like [here](https://www.arduino.cc/en/Tutorial/Foundations/ShiftOut))
* Planned: Well, the 8 bit CPU...
* Maybe: Using Ardiuno as a general purpose debugger / controller via [firmata](https://github.com/firmata/arduino) and some node app

## Resources

Over time, I collected some useful resources like [instructional videos](#instructional-videos), [references](#references), [glosary and german translations](#glosary-and-german-translations), [sourcing](#sourcing).

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

* [Ben Eater's website](https://eater.net/) with the [parts list](https://eater.net/8bit/parts) and the [schematics](https://eater.net/8bit/schematics) for the 8 bit computer amongst mayn other things
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
* VCC / VDD / "5V", de: Pluspol, Spanunsquelle
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
* [Jumper wire kit](https://www.reichelt.de/flexible-drahtbruecken-15-cm-mehrfarbig-65er-pack-rnd-255-00009-p253202.html?&nbc=1&trstct=lsbght_sldr::202832 )
* Breadboards: [4k7v](https://www.reichelt.de/experimentier-steckboard-2560-700-kontakte-steckboard-4k7v-p67683.html?&trstct=pos_2&nbc=1), [BB830](https://www.mouser.de/ProductDetail/BusBoard-Prototype-Systems/BB830?qs=VEfmQw3KOauhPeTwYxNCaA%3D%3D), [BB830T](https://www.mouser.de/ProductDetail/BusBoard-Prototype-Systems/BB830T?qs=VEfmQw3KOauXY1NKV2FuEg%3D%3D)
* Soldering station: is on my wishlist

##### Basic components

* [Resistors](https://www.amazon.de/-/en/gp/product/B08DKT3STN)
* [Electrolytic capacitors](https://www.amazon.de/-/en/gp/product/B07NWGDH2T)
* [Ceramic capacitors](https://www.amazon.de/gp/product/B07PP7SFY8) and [0.01µF ones](https://www.reichelt.de/tantalkondensator-100-nf-35-v-avx-tap104m035cc-p246447.html?&trstct=pos_0&nbc=1)
* [1MΩ potentiometer](https://www.reichelt.de/drehpotentiometer-mono-1-0-mohm-4mm-linear-po4m-lin-1-0m-p88275.html?&trstct=pol_0&nbc=1)
* [Slide switch](https://www.reichelt.de/schiebeschalter-gerade-rm5-08-1x-ein-ein-ss-25436-nh-p105713.html?&trstct=pol_2&nbc=1)
* DIP switch [4-position](https://www.reichelt.de/dip-schalter-printmontage-liegend-4-polig-rnd-210-00164-p226385.html?&trstct=pos_3&nbc=1), [8-position](https://www.reichelt.de/dip-schalter-printmontage-liegend-8-polig-rnd-210-00166-p226387.html?&trstct=pos_1&nbc=1)
* Momentary tact pushbutton/switch: ordered various, will pick one to list later

##### LEDs

* [LEDs sortiment](https://www.reichelt.de/led-sortiment-5-mm-superhell-vorwiderstaende-fassungen-quad-1801o004-p280142.html?&trstct=pos_4&nbc=1)
* [7-segment LED displays, common cathode](https://www.reichelt.de/7-segment-anzeige-superrot-14-2mm-gem-kathode-sc-56-11-srt-p80388.html?&trstct=pos_0&nbc=1)
* Bargraph [yellow](https://www.reichelt.de/bargraph-anzeige-10-elemente-gelb-ybg-1000-p22957.html?&trstct=pol_2&nbc=1), [green](https://www.reichelt.de/bargraph-anzeige-10-elemente-gruen-gbg-1000-p8141.html?&trstct=pol_1&nbc=1), [green/red](https://www.reichelt.de/bargraph-anzeige-10-elemente-3x-rot-7x-gruen-rgbg-1000-p31501.html?&trstct=pol_3&nbc=1)
* 5V LEDs with resistor: [green](https://www.reichelt.de/led-5-mm-bedrahtet-5-v-gruen-20-mcd-30--kbt-l-7113gd-5v-p231091.html?&trstct=pol_12&nbc=1), [yellow](https://www.reichelt.de/led-5-mm-bedrahtet-5-v-gelb-20-mcd-30--kbt-l-7113yd-5v-p231097.html?&trstct=pol_8&nbc=1), [red](https://www.reichelt.de/led-5-mm-bedrahtet-5-v-rot-30-mcd-30--kbt-l-7113id-5v-p231093.html?&trstct=pol_1&nbc=1)
* LED matrix / LED strip: are on my wishlist

##### Bigger ICs

* [Arduino nano clone](https://www.reichelt.de/arduino-kompatibles-nano-board-atmega328-mini-usb-ard-nano-v3-p225690.html?&nbc=1&trstct=lsbght_sldr::177331)
* ["74189 64-bit RAM"](https://www.reichelt.de/sram-32-kb-4-k-x-8-4-5--5-5-v-dil-28-62256-80-p2673.html?&trstct=pol_4&nbc=1) **(I only got a bigger one, maybe [this post and links there](https://www.reddit.com/r/beneater/comments/hon6ar/74189_alternative/) help)**
* [28C16 EEPROMs](https://www.reichelt.de/eeprom-64-kb-8-k-x-8-5-v-150-ns-pdip-28-28c64-150-p1946.html?r=1)
* [LM-555 Timer ICs](https://www.reichelt.de/timer-ic-typ-555-dip-8-lm-555-cn-p187567.html?&trstct=pos_8&nbc=1)

##### 7400er ICs

* [74LS04 Hex inverter](https://www.reichelt.de/inverter-hex-4-5--5-5-v-dil-14-74hct-04-p3306.html?&trstct=pos_0&nbc=1) 5×
* [74LS08 Quad 2-input AND gate](https://www.reichelt.de/and-gate-2-element-4-5--5-5-v-dil-14-74hct-08-p3307.html?&trstct=pos_0&nbc=1) 3×
* [74LS00 quad NAND gate](https://www.reichelt.de/nand-gate-2-input-4-5--5-5-v-dil-14-74hct-00-p3303.html?&trstct=pos_0&nbc=1) 2×
* [74LS02 quad NOR gate](https://www.reichelt.de/nor-gate-2-input-4-5--5-5-v-dil-14-74hct-02-p3304.html?&trstct=pos_0&nbc=1) 1×
* [74LS32 Quad 2-input OR gate](https://www.reichelt.de/4-fach-oder-gatter-je-2-eingaenge-4-5--5-5-v-dil-14-74hct-32-p3369.html?&trstct=pos_0&nbc=1) 1×
* [74LS86 Quad XOR gate](https://www.reichelt.de/xor-gate-2-input-4-75--5-25-v-dil-14-ls-86-p10826.html?&trstct=pos_0&nbc=1) 2×
* [74LS157 quad 2-1 line mux](https://www.reichelt.de/multiplexer-lp-schottky-4-75--5-25-v-dil-16-ls-157-p10625.html?&trstct=pos_0&nbc=1) 4×
* [74LS107 dual J-K flip-flop](https://www.reichelt.de/flip-flop-d-type-dual-4-5--5-5-v-dil-14-74hct-107-p3309.html?search=74HCT+107) 1×
* [74LS138 3-to-8 line decoder](https://www.reichelt.de/decoder-3-to-8-4-5--5-5-v-dil-16-74hct-138-p3318.html?&trstct=pos_0&nbc=1) 1×
* [74LS139 2-to-4 line decoder](https://www.reichelt.de/dekoder-demultiplexer-2-auf-4-4-5--5-5-v-dip-16-74hct-139-p3319.html?&trstct=pos_0&nbc=1) 1×
* [74LS161 4-bit binary counter](https://www.reichelt.de/counter-4-bit-4-75--5-25-v-dil-16-ls-163-p10630.html?&trstct=pol_0&nbc=1) 2× **(I only got the 163 with slightly different specs)**
* [74LS173 4-bit D register](https://www.mouser.de/ProductDetail/Texas-Instruments/SN74LS173AN?qs=nMmhAzRCgdAkY4Cck6ihbQ%3D%3D) 8×
* [74LS273 8-bit D register](https://www.reichelt.de/flip-flop-d-type-octal-4-5--5-5-v-dil-20-74hct-273-p3363.html?&trstct=pol_0&nbc=1) 1×
* [74HC595 8-bit shift register](https://www.reichelt.de/shift-register-3-state-ausgang-2--6-v-dil-16-74hc-595-p3269.html?&trstct=pol_2&nbc=1) 2×
* [74LS245 8-bit bus transceiver](https://www.reichelt.de/octal-bus-puffer-3-state-4-5--5-5-v-dil-20-74hct-245-p3356.html?&trstct=pol_1&nbc=1) 6×
* [74LS283 4-bit binary adder](https://www.mouser.de/ProductDetail/Texas-Instruments/SN74LS283N?qs=RnzODY3cU8uVeiPelRVWjw%3D%3D) 2×

