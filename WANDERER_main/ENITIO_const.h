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

#define PROFILE_enable_add 0
#define OG_add 1
#define NAME_add 2
#define isGL_add 3
#define ID_add 4
#define PLAYER_enable_add 5
#define PLAYER_HP_add 6
#define PLAYER_EN_add 7
#define PLAYER_MaxHP_add 8
#define PLAYER_MaxEn_add 9
#define PLAYER_MANA_add 10
#define PLAYER_numKilled_add 11
#define PLAYER_numL1Treasure_add 12
#define PLAYER_numL2Treasure_add 13
