// Scheme (FRIDGE) ARDUINO NANO v3.0
// RX0 - RO    (rs485)
// TX1 - DI    (rs485)
// D2 - DE,RE  (rs485)
// D3 - key    (key)

class Block {
  public:
    // rs485 номер вывода к которому подключены выводы RE и DE конвертирующего модуля
    static const uint8_t PIN_DIRECTION_TX_RX = 2;       // D2, 485

    // max buf for serial port
    static const uint8_t MAX_BUF_SIZE = 128;

    // number of blocks
    enum BlockType : byte {
      BLOCK_ALL    = 0,  // всем блокам
      BLOCK_MAIN   = 1,  // главный контроллер
      BLOCK_FRIDGE = 3,  // холодильник
      BLOCK_GARDEN = 4,  // грядка
      BLOCK_TABLE  = 5,  // стол

      BLOCK_END          // must be last
    };

    // commands interblock interaction
    enum Command : byte {
      CMD_ANSWER       = 0,
      CMD_GET_STATUS   = 1,
      CMD_TEST_QUEST   = 2,

      CMD_END               // must be last
    };

    // block status
    enum Status : byte {
      STATUS_OK    = 0,
      STATUS_TEST  = 1,
      STATUS_ERROR = 2,

      STATUS_END            // must be last
    };

    Block(BlockType blockType) : _blockType(blockType) {
    }

    virtual ~Block() {
    }

    virtual bool configSpecific() = 0;
    virtual bool testQuest() = 0;

    void commandProcessing(const String& data) {
      if (isLog(data) == false) {
        // command mode
        int blockType = getBlockRecipient(data);
        if (blockType == _blockType /*it is i am?*/ || blockType == BLOCK_ALL /*broadcast*/) {
          int numCommand = getNumCommand(data);
          LOG(String("command: ") + numCommand);

          switch (numCommand) {
            case CMD_GET_STATUS: {
                int sendingBlockType = getBlockSending(data);
                cmd(sendingBlockType, CMD_ANSWER, statusToString());
              } break;
            case CMD_TEST_QUEST: {
                testQuest();
              } break;
          };
        }
      }
    }

    bool config() {
      configRs485();
      configSpecific();
    }

    String readSerialPort() {
      String data = "";
      while (Serial.available() > 0) {
        data += Serial.read();
      }
      return data;
    }

    // Анализ номера принимающего блока в data, формат: "TOХХX"
    int getBlockRecipient(const String& data) {
      return getData(data, "TO");
    }

    // Анализ номера передающего блока в data, формат: "FROMХХX"
    int getBlockSending(const String& data) {
      return getData(data, "FROM");
    }

    // Анализ команды, формат "CMDXXХ" (CMD0, CMD1...)
    int getNumCommand(const String& data) {
      return getData(data, "CMD");
    }

    // Анализ данных команды, формат "<тут передаются данные>"
    String getDataFromCommand(const String& data) {
      int beginPos = data.indexOf('<');
      if (beginPos > -1) {
        int endPos = data.indexOf('>');
        if (endPos > -1 && endPos > beginPos) {
          return data.substring(beginPos, endPos);
        }
      }
      return "";
    }

    // Команда TOXXXFROMXXXCMDXXX<Данные передающиеся в блок TOXXX из FROMXXX командой CMDXXX>
    bool cmd(int receiverBlock, int command, const String& data = "") {
      if (receiverBlock < 0 || receiverBlock >= BLOCK_END ||
          command < 0 || command >= CMD_END) {
        return false;
      } else {
        String cmdData = "";
        if (data.length() > 0) {
          cmdData += "<";
          cmdData += data;
          cmdData += ">";
        }
        Serial.print(String("TO") + receiverBlock + "FROM" + _blockType + "CMD" + command + cmdData);
        return true;
      }
    }

    bool isLog(const String& data) {
      return data.substring(0, 3) == "LOG";
    }

    void LOG(const String& data) {
      Serial.println(String("LOG(B") + _blockType + "): " + data);
    }

    String statusToString() {
      switch (_status) {
        case STATUS_OK: return "OK";
        case STATUS_TEST: return "TEST";
        case STATUS_ERROR:  return "TEST";
        default: return "";
      };
    }

  private:
    int getData(const String& data, const String& key, int beginPosition = 0) {
      int position = data.indexOf(key, beginPosition);
      if (position > -1) {
        int i = 0, size = key.length();
        String strResult = "";
        for (; i < 3; ++i) {
          char ch = data.charAt(position + size + i);
          if (isDigit(ch)) {
            strResult += ch;
          } else {
            break;
          }
        }
        return strResult.toInt();
      }
    }

    void configRs485() {
      //  pinMode(PIN_direction_TX_RX, OUTPUT);      // устанавливаем режим работы вывода PIN_direction_TX_RX, как "выход"
      //  digitalWrite(PIN_DIRECTION_TX_RX, LOW);    // устанавливаем уровень логического «0» на выводе PIN_direction_TX_RX (переводим модуль в режим приёма данных)
      Serial.begin(9600);
    }

  private:
    // number of current controller
    uint8_t _blockType;
    uint8_t _status = STATUS_OK;
    String _currentError = "";

};

