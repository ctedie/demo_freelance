#include "music.h"
#include "buzzer.h"

// Variables d'état du séquenceur
static const Note_t *g_pCurrentMelody = 0;
static uint16_t g_ui16MelodyLength = 0;
static uint16_t g_ui16CurrentNoteIndex = 0;

static bool g_bIsPlaying = false;
static uint32_t g_ui32TimeRemainingMs = 0;
static uint32_t g_ui32MsPerUnit = 0; // Durée d'une double-croche en ms

void Music_Init(void) {
    g_bIsPlaying = false;
}

/**
 * @brief Lance la lecture d'une partition
 * @param melody Pointeur vers le tableau de notes
 * @param length Nombre de notes dans le morceau
 * @param bpm Battements par minute (Tempo)
 */
void Music_PlayMelody(const Note_t *melody, uint16_t length, uint16_t bpm) {
    if (length == 0 || melody == 0) return;

    // Calcul du temps en ms pour une "Noire" (4 unités)
    uint32_t msPerBeat = 60000 / bpm; 
    
    // Calcul du temps en ms pour l'unité de base (la double-croche, 1 unité)
    g_ui32MsPerUnit = msPerBeat / 4; 

    // Initialisation de la lecture
    g_pCurrentMelody = melody;
    g_ui16MelodyLength = length;
    g_ui16CurrentNoteIndex = 0;
    
    // Chargement de la première note
    g_ui32TimeRemainingMs = g_pCurrentMelody[0].duration * g_ui32MsPerUnit;
    Buzzer_PlayNote(g_pCurrentMelody[0].frequency);
    
    g_bIsPlaying = true;
}

void Music_Stop(void) {
    g_bIsPlaying = false;
    Buzzer_Stop();
}

/**
 * @brief Machine d'état à appeler STRICTEMENT TOUTES LES 1 ms
 */
void Music_Tick1ms(void) {
    if (!g_bIsPlaying) return;

    // On décrémente le temps de la note actuelle
    if (g_ui32TimeRemainingMs > 0) {
        g_ui32TimeRemainingMs--;
    } 
    // Si la note est terminée, on passe à la suivante
    else {
        g_ui16CurrentNoteIndex++;

        // Reste-t-il des notes à jouer ?
        if (g_ui16CurrentNoteIndex < g_ui16MelodyLength) {
            // On calcule la durée de la nouvelle note
            g_ui32TimeRemainingMs = g_pCurrentMelody[g_ui16CurrentNoteIndex].duration * g_ui32MsPerUnit;
            
            // Pour marquer l'attaque (détacher les notes identiques), 
            // on pourrait ajouter un micro-silence ici. 
            // Pour l'instant, on joue directement la note :
            Buzzer_PlayNote(g_pCurrentMelody[g_ui16CurrentNoteIndex].frequency);
        } 
        // Morceau terminé
        else {
            Music_Stop();
        }
    }
}
