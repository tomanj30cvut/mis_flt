#include "encoder_decoder.h"

void initEncoderDecoder(encoder_decoder_t *decoder, unsigned char initValue) {
    decoder->lastStateA = false;
    decoder->lastStateB = false;
    decoder->edgeCount = (initValue > 255) ? 255 : initValue;
    decoder->directionCW = true;
    decoder->stateChanged = false;
}

unsigned char runEncoderDecoder(encoder_decoder_t *decoder, bool channelA, bool channelB) {
    decoder->stateChanged = false;

    // Detekuj zm?nu stavu
    if ((channelA != decoder->lastStateA) || (channelB != decoder->lastStateB)) {
        
        // Sekvence Gray code: 00->01->11->10->00 (dop?edu)
        unsigned char currentState = (channelA << 1) | channelB;
        unsigned char lastState = (decoder->lastStateA << 1) | decoder->lastStateB;

        bool isClockwise = false;

        // Rozhodnutí o sm?ru
        switch(lastState) {
            case 0: // 00
                if(currentState == 1) isClockwise = true;  // 00->01 dop?edu
                if(currentState == 2) isClockwise = false; // 00->10 vzad
                break;
            case 1: // 01
                if(currentState == 3) isClockwise = true;  // 01->11 dop?edu
                if(currentState == 0) isClockwise = false; // 01->00 vzad
                break;
            case 3: // 11
                if(currentState == 2) isClockwise = true;  // 11->10 dop?edu
                if(currentState == 1) isClockwise = false; // 11->01 vzad
                break;
            case 2: // 10
                if(currentState == 0) isClockwise = true;  // 10->00 dop?edu
                if(currentState == 3) isClockwise = false; // 10->11 vzad
                break;
            default:
                isClockwise = true;
                break;
        }

        decoder->directionCW = isClockwise;
        decoder->stateChanged = true;

        // Aktualizuj po?ítadlo
        if(isClockwise) {
            if(decoder->edgeCount < 255) {
                decoder->edgeCount++;
            } else {
                decoder->edgeCount = 0; // P?ete?ení -> reset
            }
        } else {
            if(decoder->edgeCount > 0) {
                decoder->edgeCount--;
            } else {
                decoder->edgeCount = 255; // Podte?ení -> max
            }
        }

        // Ulo? aktuální stav pro p?í?t?
        decoder->lastStateA = channelA;
        decoder->lastStateB = channelB;
    }

    return decoder->edgeCount;
}

unsigned char getEncoderEdgeCount(encoder_decoder_t *decoder) {
    return decoder->edgeCount;
}

bool getEncoderDirection(encoder_decoder_t *decoder) {
    return decoder->directionCW;
}

void resetEncoderCounter(encoder_decoder_t *decoder, unsigned char newValue) {
    decoder->edgeCount = (newValue > 255) ? 255 : newValue;
}
