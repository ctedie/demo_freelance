import React from 'react';

// Tableau des notes avec leurs fréquences associées (en Hz)
const NOTES = [
    { name: 'DO', freq: 262, isBlack: false },
    { name: 'DO#', freq: 277, isBlack: true },
    { name: 'RÉ', freq: 294, isBlack: false },
    { name: 'RÉ#', freq: 311, isBlack: true },
    { name: 'MI', freq: 330, isBlack: false },
    { name: 'FA', freq: 349, isBlack: false },
    { name: 'FA#', freq: 370, isBlack: true },
    { name: 'SOL', freq: 392, isBlack: false },
    { name: 'SOL#', freq: 415, isBlack: true },
    { name: 'LA', freq: 440, isBlack: false },
    { name: 'LA#', freq: 466, isBlack: true },
    { name: 'SI', freq: 494, isBlack: false },
    { name: 'DO5', freq: 523, isBlack: false }
];

export default function Piano() {
    
    // Fonction pour envoyer la fréquence au serveur Node
    const playNote = async (frequency) => {
        try {
            await fetch('http://localhost:3000/api/buzzer', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ frequency })
            });
        } catch (error) {
            console.error("Erreur API Piano :", error);
        }
    };

    // Fonction pour couper le son (Fréquence = 0)
    const stopNote = () => {
        playNote(0);
    };

    return (
        <div style={{
            padding: '20px',
            border: '1px solid #ddd',
            borderRadius: '12px',
            backgroundColor: '#1e1e24',
            boxShadow: '0 4px 15px rgba(0,0,0,0.2)',
            maxWidth: '600px',
            margin: '20px auto',
            fontFamily: 'sans-serif'
        }}>
            <h3 style={{ color: '#fff', marginTop: 0, textAlign: 'center' }}>🎹 Piano Numérique Tiva C</h3>
            
            <div style={{ 
                display: 'flex', 
                position: 'relative', 
                height: '200px', 
                backgroundColor: '#000', 
                padding: '10px', 
                borderRadius: '8px',
                overflow: 'hidden'
            }}>
                {NOTES.map((note, index) => {
                    // Style pour les touches blanches
                    const whiteKeyStyle = {
                        flex: 1,
                        backgroundColor: '#fff',
                        border: '1px solid #ccc',
                        borderRadius: '0 0 4px 4px',
                        cursor: 'pointer',
                        display: 'flex',
                        alignItems: 'flex-end',
                        justifyContent: 'center',
                        paddingBottom: '10px',
                        fontWeight: 'bold',
                        color: '#333',
                        zIndex: 1
                    };

                    // Style pour les touches noires (superposées)
                    const blackKeyStyle = {
                        position: 'absolute',
                        width: '32px',
                        height: '120px',
                        backgroundColor: '#333',
                        color: '#fff',
                        cursor: 'pointer',
                        display: 'flex',
                        alignItems: 'flex-end',
                        justifyContent: 'center',
                        paddingBottom: '10px',
                        fontSize: '11px',
                        borderRadius: '0 0 3px 3px',
                        zIndex: 2,
                        // Calcul d'un décalage horizontal approximatif pour placer la touche noire entre deux blanches
                        left: `${(index * 42) + 20}px` 
                    };

                    return (
                        <button
                            key={note.name}
                            style={note.isBlack ? blackKeyStyle : whiteKeyStyle}
                            onMouseDown={() => playNote(note.freq)}
                            onMouseUp={stopNote}
                            onMouseLeave={stopNote} // Sécurité si la souris glisse hors de la touche
                        >
                            {note.name}
                        </button>
                    );
                })}
            </div>
        </div>
    );
}