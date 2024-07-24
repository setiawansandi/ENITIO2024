class Profile {
  private:
    int CLAN_pointer = 0;
    int CLAN;

  public:
    Profile() {
        // Constructor: Initialize values from EEPROM
        if (EEPROM.read(PROFILE_enable_add) == 1) {
            CLAN = EEPROM.read(CLAN_add);
            CLAN_pointer = CLAN;  // Initialize pointer to the saved CLAN

        // Serial.print("Profile Initialized: CLAN=");
        // Serial.println(CLAN);
    }
  }

    int getClanPointer() const {
        return CLAN_pointer;
    }

    bool isRegistered() const {
        return EEPROM.read(PROFILE_enable_add) == 1;
    }

    int getRegisteredClan() const {
        return CLAN;
    }

    void handleJoystickRegistered() {
        joystick_pos joystick_pos = TreasureBase_joystick.read_Joystick();
        if (TreasureBase_joystick.get_state() == 0) {
            switch (joystick_pos) {
                case button:
                    TreasureBase_joystick.set_state();
                    CLAN = CLAN_pointer;

                    // Store CLAN and enable profile in EEPROM
                    EEPROM.write(CLAN_add, CLAN);
                    EEPROM.write(PROFILE_enable_add, 1);
                    EEPROM.commit();
                    Serial.print("CLAN Registered: CLAN=");
                    Serial.println(CLAN);
                    break;

                case up:
                    CLAN_pointer = max(CLAN_pointer - 1, 0);
                    TreasureBase_joystick.set_state();
                    break;

                case down:
                    CLAN_pointer = min(CLAN_pointer + 1, 4); // Adjusted for array size 5
                    TreasureBase_joystick.set_state();
                    break;

                case idle:
                    break;

                default:
                    TreasureBase_joystick.set_state();
                    break;
            }
        }
    }

    void CLANRegistered() {
        if (isRegistered()) {
            CLAN = EEPROM.read(CLAN_add);
            CLAN_pointer = CLAN;
        }
    }
};

Profile TreasureBase_profile;