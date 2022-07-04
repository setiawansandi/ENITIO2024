int currentProcess = 0;
String my_MAC_address;

const int MainMenuProcess = 0 ;
const int ProfileProcess = 1 ;
const int TreasureHuntProcess = 2 ; 
const int SnakeGameProcess = 3 ;
const int MusicProcess = 4 ; 
const int FactoryResetProcess = 5 ;
const int CreditProcess = 6 ;

#define NAV_WAIT 4000

#define EEPROM_SIZE 20

const int PROFILE_enable_add = 0;
const int OG_add = 1;
const int NAME_add = 2;
const int isGL_add = 3;
const int ID_add = 4;
const int PLAYER_enable_add = 5;
const int PLAYER_HP_add = 6;
const int PLAYER_EN_add = 7;
const int PLAYER_MaxHP_add = 8;
const int PLAYER_MaxEn_add = 9;
const int PLAYER_MANA_add = 10;
const int PLAYER_numKilled_add = 11;
const int PLAYER_numL1Treasure_add = 12;
const int PLAYER_numL2Treasure_add = 13;
