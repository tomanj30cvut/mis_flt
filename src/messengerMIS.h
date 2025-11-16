#ifndef MESSENGER_MIS_H
#define MESSENGER_MIS_H

#include <stdbool.h>

//--- Komunikační konstanty z knihovny ----------------------------------------

#define COM_GO false          ///< Pokračovat v komunikaci
#define COM_RESET true        ///< Reset komunikace
#define MSG_LEN_IX 0          ///< Index délky zprávy

#define RX_MESSAGE_LENGTH_MASK 0x3F           ///< Maska délky zprávy
#define RX_FROM_TEXT_TERMINAL 0x40            ///< Zpráva z textového terminálu
#define RX_FROM__COMMAND_EDITOR 0x00          ///< Zpráva z příkazového editoru
#define RX_MESSAGE_TYPE_MASK 0xC0             ///< Maska typu zprávy

//--- RTM Messenger API funkce z platformMIS knihovny -------------------------

void initSendGetMessageUSB(void);

bool sendMessageUSB(unsigned char *msg, bool reset);

bool getMessageUSB(unsigned char *msg, bool reset);

bool sendTableTerminalMessageUSB(char *cellAddr, char *cellText);

bool sendTextTerminalMessageUSB(char *text);

//--- Konverzní funkce pro proměnné z platformMIS knihovny -------------------

void integerToBytes(const signed short intIn, unsigned char msgOut[]);

signed short bytesToInteger(const unsigned char rxMsg[]);

void floatToBytes(const float floatIn, unsigned char msgOut[]);

float bytesToFloat(const unsigned char rxMsg[]);

//--- RTM Messenger modul funkce -----------------------------------------------

void initMessengerRTM(void);

void runMessengerRTM(void);

bool isRTMCommunicationActive(void);

void resetMessengerRTM(void);

#endif // MESSENGER_MIS_H

//--- Konec messengerMIS.h ---------------------------------------------------