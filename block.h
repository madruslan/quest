// Scheme (FRIDGE) ARDUINO NANO v3.0
// RX0 - RO    (rs485)
// TX1 - DI    (rs485)
// D2 - DE,RE  (rs485)
// D3 - key    (key)

class Block {
  public:
    // rs485 номер вывода к которому подключены выводы RE и DE конвертирующего модуля
    static const uint8_t PIN_DIRECTION_TX_RX = 2;       // D2, 485

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

    Block(BlockType blockType);

    virtual ~Block();

    virtual bool configSpecific() = 0;
    virtual bool testQuest() = 0;

    bool config();
	void commandProcessing(const String& data);
    
    String readSerialPort();

    // Анализ номера принимающего блока в data, формат: "TOХХX"
    int getBlockRecipient(const String& data) const;

    // Анализ номера передающего блока в data, формат: "FROMХХX"
    int getBlockSending(const String& data) const;

    // Анализ команды, формат "CMDXXХ" (CMD0, CMD1...)
    int getNumCommand(const String& data) const;

    // Анализ данных команды, формат "<тут передаются данные>"
    String getDataFromCommand(const String& data) const;

    // Команда TOXXXFROMXXXCMDXXX<Данные передающиеся в блок TOXXX из FROMXXX командой CMDXXX>
    bool cmd(int receiverBlock, int command, const String& data = "") const;

    bool isLog(const String& data) const;

    void LOG(const String& data);

    String statusToString() const;

  private:
    int getData(const String& data, const String& key, int beginPosition = 0) const;

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
