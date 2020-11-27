// To compile and run use:
// g++ -std=c++11 -Wall -o test test.cpp && ./test && rm test

#include<cassert>
#include<cstdio>
#include<iostream>
#include<list>
#include<string>

// Mock Arduino SDK
// See also /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino

#define LED_BUILTIN 13
#define PROGMEM

#define OUTPUT 42

#define HIGH 1
#define LOW 0

#define DEC 10

std::string morseOutput = "";
const std::string initialSerialPrintlnStringOutput = "(was not set)";
std::string serialPrintlnStringOutput = initialSerialPrintlnStringOutput;
const long initialSerialPrintNumberOutput = -1;
long serialPrintNumberOutput = initialSerialPrintNumberOutput;
std::list<int> mockSerialBuffer = {};
uint8_t mockPinValue = -1;

class Serial {
public:
  void begin(unsigned long baud_count) {
    assert(baud_count == 9600);
  }

  int available() {
    return !mockSerialBuffer.empty();
  }

  int read() {
    if(mockSerialBuffer.empty()) {
      assert("Reading from empty serial buffer");
    }
    int result = mockSerialBuffer.front();
    mockSerialBuffer.pop_front();
    return result;
  }

  void print(std::string string) {
    assert(serialPrintlnStringOutput == initialSerialPrintlnStringOutput);
    serialPrintlnStringOutput = string;
  }

  void println(long n, int base) {
    assert(base == 10);
    assert(serialPrintNumberOutput == initialSerialPrintNumberOutput);
    serialPrintNumberOutput = n;
  }
} Serial;

void pinMode(uint8_t pin, uint8_t mode) {
  assert(pin == LED_BUILTIN);
  assert(mode == OUTPUT);
}

void delay(unsigned long ms) {
  assert(ms % 120 == 0);
  auto dits = ms / 120;
  assert(mockPinValue == HIGH || mockPinValue == LOW);
  morseOutput.append(dits, mockPinValue == HIGH ? '=' : '_');
}

void digitalWrite(uint8_t pin, uint8_t value) {
  assert(pin == LED_BUILTIN);
  mockPinValue = value;
}

uint8_t pgm_read_byte_near(const uint8_t *position) {
  return *position;
}

#define F(string_literal) string_literal

// Load sketch code

#include "morse.ino"

// Helpers

void assertEqual(std::string expected, std::string actual, std::string headline) {
  if(expected != actual) {
    std::cout << headline <<  std::endl;
    std::cout << "Expected: " << expected << std::endl;
    std::cout << "Actual:   " << actual << std::endl;
    assert(false);
  }
}

void expectMorseOutput(std::string expected) {
  assertEqual(expected, morseOutput, "Morse output was wrong");
  morseOutput = "";
}

void expectSerialPrintlnString(std::string expected) {
  assertEqual(expected, serialPrintlnStringOutput, "Serial output was wrong");
  serialPrintlnStringOutput = "(was not set)";
}

void expectSerialPrintNumber(long expected) {
  if(expected != serialPrintNumberOutput) {
    std::cout << "Serial output number was wrong" <<  std::endl;
    std::cout << "Expected: " << expected << std::endl;
    std::cout << "Actual:   " << serialPrintNumberOutput << std::endl;
    assert(false);
  }
  serialPrintNumberOutput = -1;
}

void setupSerialBuffer(std::string string) {
  mockSerialBuffer.clear();
  for (int c : string) {
    mockSerialBuffer.push_back(c);
  }
}

void runIt() {
  setup();
	for(int i = 0; i < 5000; i++) {
		loop();
	}
}

// The actual tests

std::string morseCodeAsMorseCode = "===_===___===_===_===___=_===_=___=_=_=___=_______===_=_===_=___===_===_===___===_=_=___=___";

int main(int argc, char const *argv[]) {
  std::cout << "Testing uppercase letters ";
  setupSerialBuffer("MORSE CODE");
  runIt();
  expectMorseOutput(morseCodeAsMorseCode);
  std::cout << "✓" << std::endl;

  std::cout << "Testing lowercase letters ";
  setupSerialBuffer("morse code");
  runIt();
  expectMorseOutput(morseCodeAsMorseCode);
  std::cout << "✓" << std::endl;

  std::cout << "Testing new line ";
  setupSerialBuffer("a\nb");
  runIt();
  expectMorseOutput("=_===______________===_=_=_=___");
  std::cout << "✓" << std::endl;

  std::cout << "Test message 1 ";
  setupSerialBuffer("The quick brown fox");
  runIt();
  expectMorseOutput("===___=_=_=_=___=_______===_===_=_===___=_=_===___=_=___===_=_===_=___===_=_===_______===_=_=_=___=_===_=___===_===_===___=_===_===___===_=_______=_=_===_=___===_===_===___===_=_=_===___");
  std::cout << "✓" << std::endl;

  std::cout << "Test message 2 ";
  setupSerialBuffer("Jumps over the lazy dog");
  runIt();
  expectMorseOutput("=_===_===_===___=_=_===___===_===___=_===_===_=___=_=_=_______===_===_===___=_=_=_===___=___=_===_=_______===___=_=_=_=___=_______=_===_=_=___=_===___===_===_=_=___===_=_===_===_______===_=_=___===_===_===___===_===_=___");
  std::cout << "✓" << std::endl;

  std::cout << "Test numbers 0-4 ";
  setupSerialBuffer("01234");
  runIt();
  expectMorseOutput("===_===_===_===_===___=_===_===_===_===___=_=_===_===_===___=_=_=_===_===___=_=_=_=_===___");
  std::cout << "✓" << std::endl;

  std::cout << "Test numbers 5-9 ";
  setupSerialBuffer("56789");
  runIt();
  expectMorseOutput("=_=_=_=_=___===_=_=_=_=___===_===_=_=_=___===_===_===_=_=___===_===_===_===_=___");
  std::cout << "✓" << std::endl;

  std::cout << "Testing invalid letter ";
  setupSerialBuffer(".");
  runIt();
  expectMorseOutput("");
  expectSerialPrintlnString("Ignoring unknown byte ");
  expectSerialPrintNumber('.');
  std::cout << "✓" << std::endl;

  return 0;
}
