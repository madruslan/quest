#include <EEPROM.h>

// Scheme (FRIDGE) ARDUINO NANO v3.0
// RX0 - RO    (rs485)
// TX1 - DI    (rs485)
// D2 - DE,RE  (rs485)
// D3 - key
// D4 - LED
// D5 - REL
// A0 -	SS49E

//-------TEST CODE------
int countTest = 0;
String cmds[] = {
  "TO0FROM1CMD1", // get status
  "TO3FROM1CMD2"  // run quest test
};
//-------TEST CODE------



class Led {
  public:
    Led(int ledPin) : _ledPin(ledPin) {
    }
    void config() {
      pinMode(_ledPin, OUTPUT);
      off();
    }

    void on() {
      digitalWrite(_ledPin, HIGH);
    }

    void off() {
      digitalWrite(_ledPin, LOW);
    }

  private:
    int _ledPin;
};


class Button {
  public:
    Button(int buttonPin) : _buttonPin(buttonPin) {
    }

    void config() {
      pinMode(_buttonPin, INPUT);
    }

    bool state() {
      return digitalRead(_buttonPin) == HIGH;
    }

  private:
    int _buttonPin;
};

class Relay {
  public:
    Relay(int relayPin) : _relayPin(relayPin) {
    }

    void config() {
      pinMode(_relayPin, OUTPUT);
      close();
    }

    void open() {
      digitalWrite(_relayPin, HIGH);
    }

    void close() {
      digitalWrite(_relayPin, LOW);
    }

  private:
    int _relayPin;
};

// класс добавляет уникальность квестовым блокам
class QuestBlock : public Block {
  public:
    // пин кнопки
    static const int PIN_BUTTON = 3;
    // пин светодиода
    static const int PIN_LED = 4;
    // пин реле
    static const int PIN_RELAY = 5;

    QuestBlock(Block::BlockType blockType) : Block(blockType), _button(PIN_BUTTON), _led(PIN_LED), _relay(PIN_RELAY) {
    }

    virtual void configSpecific() {
      _button.config();
      _led.config();
      _relay.config();
    }

  protected:
    Button _button;
    Led _led;
    Relay _relay;
};


// Fridge
class FridgeBlock : public QuestBlock {
  public:
    // адрес в EEPROM постоянной состовляющей аналогово датчика Холла SS49E после калибровки
    static const int CONST_COMPONENT_ADDR = 0;
    // значение постоянной состовляющей датчика Холла в лабораторных условиях
    static const int DEFAULT_CONST_COMPONENT = 501;
    // пин датчика Холла
    static const int PIN_SS49E_ANALOG = 0;

    FridgeBlock() : QuestBlock(BLOCK_FRIDGE) {
    }

    virtual void configSpecific() {
      LOG("config specific begin");
      QuestBlock::configSpecific();

      // читаем из EEPROM постоянную составляющую для датчика Холла (минимум параболы усиления)
      constComponent = getFloatFromEeprom(CONST_COMPONENT_ADDR);
      if (constComponent == NAN) {
        constComponent = DEFAULT_CONST_COMPONENT;
        LOG("read of const component, value is NaN set default value");
      } else {
        LOG(String("read of const component (") + constComponent + ")");
      }
    }

    // усиливаем сигнал, минимум параболы в значении постоянной составляющей
    float gain(int value) const {
      value -= constComponent;
      return value * value;
    }

    void calibration() {
      LOG("begin calibaration");
      // 1. измеряем постоянную составляющую с установленным датчиком и без магнита
      constComponent = analogRead(PIN_SS49E_ANALOG);
      // 2. измерение с магнитом
      LOG("end calibaration");
    }

    //
    bool questTest() {
      LOG("Test of quest");
      bool isPassed = questIsPassed();
    }

    bool questIsPassed() {
      LOG("questIsPassed");
    }

  private:
    float measure() const {
      return gain(analogRead(PIN_SS49E_ANALOG));
    }

  private:
    int constComponent = DEFAULT_CONST_COMPONENT;
};




FridgeBlock block;

void setup() {
  block.config();
}

void loop() {
  block.loop();
}

