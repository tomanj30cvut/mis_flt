#include "messengerMIS.h"
#include "platformMIS32mk.h"
#include "appStateLibrary.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//--- Konstanty ----------------------------------------------------------------

#define MSG_MAX_NUM 17              ///< Maximální délka zprávy pro RTM príkazy
#define MSG_MAX 40                  ///< Maximální délka zprávy pro terminál
#define RTM_RX_INTEGER_MSG_LENGTH 7 ///< Délka příjaté INT zprávy
#define RTM_TX_INTEGER_MSG_LENGTH 7 ///< Délka odeslané INT zprávy
#define RTM_TIMING_MAX 50           ///< Čas RTM komunikace (50 ms)
#define TABLE_TERMINAL_MSG_LEN 40   ///< Délka zprávy pro table terminál

enum {
    TX_STOP,        ///< Zastavit přenos
    TX_INTEGER_GO,  ///< Odeslat celočíselná data do RTM
};

//--- Globální proměnné --------------------------------------------------------

static unsigned char txMsgNum[MSG_MAX_NUM];     ///< Buffer pro odesílané zprávy
static unsigned char rxMsg[MSG_MAX];            ///< Buffer pro přijaté zprávy
static signed short txCmdInteger = TX_STOP;     ///< Aktuální příkaz z RTM
static bool sendIntToRTM = false;               ///< Příznak pro povolení přenosu
static unsigned short cntPerformCom = 0;        ///< Čítač pro komunikační timing
static bool rtmCommunicationActive = false;     ///< Příznak aktivní komunikace

//--- RTM Messenger funkce ---------------------------------------------------

void initMessengerRTM(void) {
    initSendGetMessageUSB();
    sendIntToRTM = false;
    txCmdInteger = TX_STOP;
    cntPerformCom = 0;
    rtmCommunicationActive = false;
}

void runMessengerRTM(void) {
    static unsigned short cntMAX = RTM_TIMING_MAX;
    
    // Spusť komunikační algoritmus každých 50ms
    if (cntPerformCom++ >= cntMAX) {
        cntPerformCom = 0;
        
        /* ==================== ČÁST PŘÍJMU ==================== */
        
        if (getMessageUSB(rxMsg, COM_GO) == true) {
            
            unsigned char messageLen = rxMsg[MSG_LEN_IX];
            
            if ((rxMsg[MSG_LEN_IX] & RX_MESSAGE_TYPE_MASK) == RX_FROM__COMMAND_EDITOR) {
                
                if ((rxMsg[0] & RX_MESSAGE_LENGTH_MASK) == RTM_RX_INTEGER_MSG_LENGTH) {
                    
                    txCmdInteger = bytesToInteger(&rxMsg[1]);
                    
                    switch (txCmdInteger) {
                        case 0:
                            // CMD(0): Zastavit odesílání zpráv
                            sendIntToRTM = false;
                            rtmCommunicationActive = false;
                            break;
                            
                        case 1:
                            // CMD(1): Odesílat hodnotu potenciometru
                            sendIntToRTM = true;
                            rtmCommunicationActive = true;
                            break;
                            
                        case 2:
                            // CMD(2): Odesílat stavy tlačítek S1, S2
                            sendIntToRTM = true;
                            rtmCommunicationActive = true;
                            break;
                            
                        case 3:
                            // CMD(3): Odesílat do table terminálu
                            sendIntToRTM = true;
                            rtmCommunicationActive = true;
                            break;
                            
                        default:
                            break;
                    }
                }
            }
        }
        
        /* ==================== ČÁST ODESÍLÁNÍ ==================== */
        
        if (sendIntToRTM == true) {
            
            app_state_t *appState = get_app_state_address();
            
            switch (txCmdInteger) {
                case 0:
                    // Zastaveno - neděj nic
                    break;
                    
                case 1:
                    // CMD(1): Odešli hodnotu potenciometru do grafického okna
                    {
                        signed short potValue = getPotentiometerValue();
                        
                        if (potValue > 2047) potValue = 2047;
                        if (potValue < -2047) potValue = -2047;
                        
                        txMsgNum[0] = RTM_TX_INTEGER_MSG_LENGTH;
                        integerToBytes(potValue, &txMsgNum[1]);
                        sendMessageUSB(txMsgNum, COM_GO);
                    }
                    break;
                    
                case 2:
                    // CMD(2): Odešli stavy tlačítek do grafického okna
                    {
                        signed short buttonS1State = (signed short)appState->button_s1.outputMemory;
                        signed short buttonS2State = (signed short)appState->button_s2.outputMemory;
                        
                        txMsgNum[0] = RTM_TX_INTEGER_MSG_LENGTH;
                        integerToBytes(buttonS1State, &txMsgNum[1]);
                        integerToBytes(buttonS2State, &txMsgNum[3]);
                        sendMessageUSB(txMsgNum, COM_GO);
                    }
                    break;
                    
                case 3:
                    // CMD(3): Odešli data do table terminálu
                    {
                        signed short potValue = getPotentiometerValue();
                        
                        if (potValue > 2047) potValue = 2047;
                        if (potValue < -2047) potValue = -2047;
                        
                        char messageBuffer[TABLE_TERMINAL_MSG_LEN];
                        
                        sprintf(messageBuffer, "Vpot=%5d S1=%d S2=%d",
                                potValue,
                                appState->button_s1.outputMemory,
                                appState->button_s2.outputMemory);
                        
                        sendTableTerminalMessageUSB("1A", messageBuffer);
                    }
                    break;
                    
                default:
                    break;
            }
        }
    }
}

bool isRTMCommunicationActive(void) {
    return rtmCommunicationActive;
}

void resetMessengerRTM(void) {
    sendIntToRTM = false;
    txCmdInteger = TX_STOP;
    cntPerformCom = 0;
    rtmCommunicationActive = false;
}

//--- Konec messengerMIS.c ------------------------------------------------