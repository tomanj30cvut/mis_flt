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
#define TABLE_TERMINAL_MSG_LEN 80   ///< Délka zprávy pro table terminál

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
                    // CMD(1): Ode?li v�stupn� hodnotu + V9 + V12 do grafick�ho okna

                    {
                        signed short outputValue = (signed short)appState->adc_0.outputInBaseRange;  // Fin�ln� v�stup (0-255)
                        signed short ledV9 = appState->adc_0.isMin ? 1 : 0;                         // LED V9 (minimum)
                        signed short ledV12 = appState->adc_0.isMax ? 1 : 0;                        // LED V12 (maximum)
                        
                        txMsgNum[0] = RTM_TX_INTEGER_MSG_LENGTH;
                        integerToBytes(outputValue, &txMsgNum[1]);
                        integerToBytes(ledV9, &txMsgNum[3]);
                        integerToBytes(ledV12, &txMsgNum[5]);
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
                        char messageBuffer[TABLE_TERMINAL_MSG_LEN];
                        
                        sprintf(messageBuffer, "Out=%3d V1=%d V2=%d V9=%d V12=%d",
                                appState->adc_0.outputInBaseRange,        // Fin�ln� v�stup
                                appState->button_s1.outputMemory,          // LED V1
                                appState->button_s2.outputMemory,          // LED V2 (p?ep�na?)
                                appState->adc_0.isMin,                     // LED V9 (minimum)
                                appState->adc_0.isMax);                    // LED V12 (maximum)
                        
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