#ifndef MODULES_MUSIC_H_
#define MODULES_MUSIC_H_

#include <stdint.h>
#include <stdbool.h>

// --- FRÉQUENCES DES NOTES (En Hertz) ---
#define SILENCE 0
#define NOTE_C4 262  // DO
#define NOTE_D4 294  // RÉ
#define NOTE_E4 330  // MI
#define NOTE_F4 349  // FA
#define NOTE_G4 392  // SOL
#define NOTE_A4 440  // LA
#define NOTE_B4 494  // SI
#define NOTE_C5 523  // DO supérieur

// --- DURÉES DES NOTES (En Unités de Temps) ---
// L'unité de base (1) est maintenant la triple-croche (ou moitié de double-croche).
#define DUREE_TRIPLE_CROCHE  1
#define DUREE_DOUBLE_CROCHE  2
#define DUREE_CROCHE         4
#define DUREE_NOIRE          8
#define DUREE_BLANCHE        16
#define DUREE_RONDE          32

// --- STRUCTURE D'UNE NOTE ---
typedef struct {
    uint32_t frequency; // Fréquence en Hz (0 = Silence)
    uint8_t duration;   // Durée relative (ex: DUREE_NOIRE)
} Note_t;

// --- PROTOTYPES DU SÉQUENCEUR ---
void Music_Init(void);
void Music_PlayMelody(const Note_t *melody, uint16_t length, uint16_t bpm);
void Music_Stop(void);
void Music_Tick1ms(void); // Fonction à appeler toutes les millisecondes

#endif /* MODULES_MUSIC_H_ */
