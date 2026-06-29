import React from 'react';

// Configuration des notes du piano
const WHITE_NOTES = [
    { name: 'DO', freq: 262 },
    { name: 'RÉ', freq: 294 },
    { name: 'MI', freq: 330 },
    { name: 'FA', freq: 349 },
    { name: 'SOL', freq: 392 },
    { name: 'LA', freq: 440 },
    { name: 'SI', freq: 494 },
    { name: 'DO5', freq: 523 }
];

const BLACK_NOTES = [
    { name: 'DO#', freq: 277, leftOffset: '44px' },
    { name: 'RÉ#', freq: 311, leftOffset: '104px' },
    { name: '', freq: 0, isGap: true }, // Espace vide entre MI et FA
    { name: 'FA#', freq: 370, leftOffset: '224px' },
    { name: 'SOL#', freq: 415, leftOffset: '284px' },
    { name: 'LA#', freq: 466, leftOffset: '344px' }
];

export default function Buzzer() {
    
    // Fonction d'envoi de la fréquence au serveur Node.js (Route /api/buzzer)
    const playNote = async (frequency) => {
        if (frequency === 0) return;
        try {
            await fetch('http://localhost:3000/api/buzzer', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ frequency })
            });
        } catch (error) {
            console.error("Erreur API Buzzer :", error);
        }
    };

    // Fonction pour couper le signal (Fréquence = 0)
    const stopNote = async () => {
        try {
            await fetch('http://localhost:3000/api/buzzer', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ frequency: 0 })
            });
        } catch (error) {
            console.error("Erreur d'arrêt du buzzer :", error);
        }
    };

    return (
        <div style={{
            padding: '30px',
            border: '1px solid #333',
            borderRadius: '16px',
            backgroundColor: '#1a1a1a',
            boxShadow: '0 10px 30px rgba(0,0,0,0.5)',
            maxWidth: '520px',
            margin: '40px auto',
            fontFamily: 'system-ui, sans-serif',
            color: '#fff'
        }}>
            <div style={{ textAlign: 'center', marginBottom: '25px' }}>
                <h2 style={{ margin: '0 0 5px 0', letterSpacing: '1px' }}>🎛️ Contrôle du Buzzer</h2>
                <p style={{ color: '#888', margin: 0, fontSize: '14px' }}>Mode Piano Numérique - PWM Matériel (Broche PF1)</p>
            </div>

            {/* Clavier de Piano */}
            <div style={{ 
                display: 'flex', 
                position: 'relative', 
                backgroundColor: '#000', 
                padding: '15px 10px', 
                borderRadius: '8px',
                boxShadow: 'inset 0 0 10px rgba(0,0,0,0.8)',
                height: '220px'
            }}>
                
                {/* TOUCHES BLANCHES */}
                {WHITE_NOTES.map((note) => (
                    <button
                        key={note.name}
                        onMouseDown={() => playNote(note.freq)}
                        onMouseUp={stopNote}
                        onMouseLeave={stopNote}
                        onMouseEnter={(e) => { if(e.buttons === 1) playNote(note.freq); }}
                        style={{
                            flex: 1,
                            height: '100%',
                            backgroundColor: '#f5f5f5',
                            border: '1px solid #111',
                            borderRadius: '0 0 6px 6px',
                            cursor: 'pointer',
                            display: 'flex',
                            alignItems: 'flex-end',
                            justifyContent: 'center',
                            paddingBottom: '15px',
                            fontWeight: 'bold',
                            color: '#222',
                            fontSize: '13px',
                            zIndex: 1,
                            userSelect: 'none',
                            outline: 'none',
                            boxShadow: 'inset 0 -5px 0 #ccc, 0 4px 5px rgba(0,0,0,0.3)'
                        }}
                    >
                        {note.name}
                    </button>
                ))}

                {/* TOUCHES NOIRES */}
                {BLACK_NOTES.map((note, index) => {
                    if (note.isGap) return null;
                    return (
                        <button
                            key={index}
                            onMouseDown={() => playNote(note.freq)}
                            onMouseUp={stopNote}
                            onMouseLeave={stopNote}
                            onMouseEnter={(e) => { if(e.buttons === 1) playNote(note.freq); }}
                            style={{
                                position: 'absolute',
                                left: note.leftOffset,
                                width: '32px',
                                height: '130px',
                                backgroundColor: '#222',
                                border: '1px solid #000',
                                borderRadius: '0 0 4px 4px',
                                cursor: 'pointer',
                                display: 'flex',
                                alignItems: 'flex-end',
                                justifyContent: 'center',
                                paddingBottom: '12px',
                                color: '#aaa',
                                fontSize: '11px',
                                fontWeight: '600',
                                zIndex: 2,
                                userSelect: 'none',
                                outline: 'none',
                                boxShadow: 'inset 0 -4px 0 #444, 0 3px 5px rgba(0,0,0,0.5)'
                            }}
                        >
                            {note.name}
                        </button>
                    );
                })}
            </div>

            <div style={{ marginTop: '20px', fontSize: '12px', color: '#666', textAlign: 'center' }}>
                * Maintenez le clic enfoncé pour activer le PWM. Le son se coupe dès que vous relâchez.
            </div>
        </div>
    );
}