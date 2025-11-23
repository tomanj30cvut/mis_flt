#ifndef ENCODER_DECODER_H
#define ENCODER_DECODER_H

#include <stdbool.h>

//--- Struktura dekodéru

typedef struct {
    bool lastStateA;           // Poslední stav kanálu A
    bool lastStateB;           // Poslední stav kanálu B
    unsigned char edgeCount;   // Po?ítadlo 0-255
    bool directionCW;          // Sm?r: true=dop?edu, false=vzad
    bool stateChanged;         // Zm?na stavu
} encoder_decoder_t;

//--- Funkce

/** Inicializuj dekodér */
void initEncoderDecoder(encoder_decoder_t *decoder, unsigned char initValue);

/** Zpracuj kvadraturní signál, vrátí po?ítadlo */
unsigned char runEncoderDecoder(encoder_decoder_t *decoder, bool channelA, bool channelB);

/** Vrátí aktuální po?ítadlo */
unsigned char getEncoderEdgeCount(encoder_decoder_t *decoder);

/** Vrátí sm?r (true=dop?edu) */
bool getEncoderDirection(encoder_decoder_t *decoder);

/** Resetuj po?ítadlo na novou hodnotu */
void resetEncoderCounter(encoder_decoder_t *decoder, unsigned char newValue);

#endif // ENCODER_DECODER_H
