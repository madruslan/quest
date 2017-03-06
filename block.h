// Scheme (FRIDGE) ARDUINO NANO v3.0
// RX0 - RO    (rs485)
// TX1 - DI    (rs485)
// D2 - DE,RE  (rs485)
// D3 - key    (key)

class Block {
  public:
    // rs485 ����� ������ � �������� ���������� ������ RE � DE ��������������� ������
    static const uint8_t PIN_DIRECTION_TX_RX = 2;       // D2, 485

    // number of blocks
    enum BlockType : byte {
      BLOCK_ALL    = 0,  // ���� ������
      BLOCK_MAIN   = 1,  // ������� ����������
      BLOCK_FRIDGE = 3,  // �����������
      BLOCK_GARDEN = 4,  // ������
      BLOCK_TABLE  = 5,  // ����

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

    // ������ ������ ������������ ����� � data, ������: "TO��X"
    int getBlockRecipient(const String& data) const;

    // ������ ������ ����������� ����� � data, ������: "FROM��X"
    int getBlockSending(const String& data) const;

    // ������ �������, ������ "CMDXX�" (CMD0, CMD1...)
    int getNumCommand(const String& data) const;

    // ������ ������ �������, ������ "<��� ���������� ������>"
    String getDataFromCommand(const String& data) const;

    // ������� TOXXXFROMXXXCMDXXX<������ ������������ � ���� TOXXX �� FROMXXX �������� CMDXXX>
    bool cmd(int receiverBlock, int command, const String& data = "") const;

    bool isLog(const String& data) const;

    void LOG(const String& data);

    String statusToString() const;

  private:
    int getData(const String& data, const String& key, int beginPosition = 0) const;

    void configRs485() {
      //  pinMode(PIN_direction_TX_RX, OUTPUT);      // ������������� ����� ������ ������ PIN_direction_TX_RX, ��� "�����"
      //  digitalWrite(PIN_DIRECTION_TX_RX, LOW);    // ������������� ������� ����������� �0� �� ������ PIN_direction_TX_RX (��������� ������ � ����� ����� ������)
      Serial.begin(9600);
    }

  private:
    // number of current controller
    uint8_t _blockType;
    uint8_t _status = STATUS_OK;
    String _currentError = "";

};
