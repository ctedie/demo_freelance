import React, { useState } from 'react';

// Dictionnaires de solfège pour l'interface
const AVAILABLE_NOTES = [
    { label: 'Silence (Pause)', val: 0 },
    { label: 'DO (262 Hz)', val: 262 },
    { label: 'RÉ (294 Hz)', val: 294 },
    { label: 'MI (330 Hz)', val: 330 },
    { label: 'FA (349 Hz)', val: 349 },
    { label: 'SOL (392 Hz)', val: 392 },
    { label: 'LA (440 Hz)', val: 440 },
    { label: 'SI (494 Hz)', val: 494 },
    { label: 'DO5 (523 Hz)', val: 523 }
];

const DURATIONS = [
    { label: 'Triple-Croche', val: 1 },
    { label: 'Double-Croche', val: 2 },
    { label: 'Croche', val: 4 },
    { label: 'Noire', val: 8 },
    { label: 'Blanche', val: 16 },
    { label: 'Ronde', val: 32 }
];

export default function Buzzer() {
    // États pour le créateur de morceau
    const [bpm, setBpm] = useState(120);
    const [composition, setComposition] = useState([]);
    const [selectedNote, setSelectedNote] = useState(262);
    const [selectedDuration, setSelectedDuration] = useState(8); // Par défaut une Noire

    // --- Fonctions Piano Live ---
    const playNoteLive = async (frequency) => {
        if (frequency === 0) return;
        try {
            await fetch('http://localhost:3000/api/buzzer', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ frequency })
            });
        } catch (err) { console.error(err); }
    };

    const stopNoteLive = async () => {
        try {
            await fetch('http://localhost:3000/api/buzzer', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ frequency: 0 })
            });
        } catch (err) { console.error(err); }
    };

    // --- Fonctions Séquenceur (Partition) ---
    const addNoteToComposition = () => {
        if (composition.length >= 24) {
            alert("Maximum 24 notes atteintes pour le buffer matériel.");
            return;
        }
        const noteObj = AVAILABLE_NOTES.find(n => n.val === Number(selectedNote));
        const durObj = DURATIONS.find(d => d.val === Number(selectedDuration));
        
        setComposition([...composition, { 
            label: noteObj.label.split(' ')[0], 
            durationLabel: durObj.label,
            freq: noteObj.val, 
            duration: durObj.val 
        }]);
    };

    const clearComposition = () => setComposition([]);

    const sendPartitionToTiva = async () => {
        if (composition.length === 0) return;
        try {
            await fetch('http://localhost:3000/api/sequence', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ bpm: Number(bpm), notes: composition })
            });
        } catch (err) {
            console.error("Erreur envoi séquence:", err);
        }
    };

    return (
        <div style={{ padding: '20px', maxWidth: '600px', margin: '0 auto', fontFamily: 'system-ui, sans-serif', color: '#fff', backgroundColor: '#121212', borderRadius: '12px' }}>
            
            <h2 style={{ textAlign: 'center', margin: '0 0 20px 0' }}>🎵 Studio Musical Tiva C</h2>

            {/* SECTION 1: PIANO LIVE */}
            <div style={{ backgroundColor: '#1e1e1e', padding: '15px', borderRadius: '8px', marginBottom: '20px' }}>
                <h4 style={{ margin: '0 0 10px 0', color: '#aaa' }}>Clavier d'écoute en temps réel</h4>
                <div style={{ display: 'flex', height: '120px', backgroundColor: '#000', padding: '5px', borderRadius: '4px' }}>
                    {AVAILABLE_NOTES.filter(n => n.val !== 0).map(note => (
                        <button 
                            key={note.val}
                            onMouseDown={() => playNoteLive(note.val)}
                            onMouseUp={stopNoteLive}
                            onMouseLeave={stopNoteLive}
                            style={{ flex: 1, margin: '2px', backgroundColor: '#fff', color: '#000', fontWeight: 'bold', border: 'none', borderRadius: '0 0 4px 4px', cursor: 'pointer' }}
                        >
                            {note.label.split(' ')[0]}
                        </button>
                    ))}
                </div>
            </div>

            {/* SECTION 2: COMPOSITEUR DE SÉQUENCE */}
            <div style={{ backgroundColor: '#1e1e1e', padding: '15px', borderRadius: '8px' }}>
                <h4 style={{ margin: '0 0 15px 0', color: '#aaa' }}>Éditeur de Partition (Séquenceur non-bloquant)</h4>
                
                {/* Outils de création */}
                <div style={{ display: 'flex', gap: '10px', marginBottom: '15px', flexWrap: 'wrap' }}>
                    <div style={{ flex: 1 }}>
                        <label style={{ fontSize: '12px', color: '#888' }}>Note</label>
                        <select value={selectedNote} onChange={(e) => setSelectedNote(e.target.value)} style={{ width: '100%', padding: '8px', borderRadius: '4px', background: '#333', color: '#fff', border: 'none' }}>
                            {AVAILABLE_NOTES.map(n => <option key={n.val} value={n.val}>{n.label}</option>)}
                        </select>
                    </div>

                    <div style={{ flex: 1 }}>
                        <label style={{ fontSize: '12px', color: '#888' }}>Durée (Solfège)</label>
                        <select value={selectedDuration} onChange={(e) => setSelectedDuration(e.target.value)} style={{ width: '100%', padding: '8px', borderRadius: '4px', background: '#333', color: '#fff', border: 'none' }}>
                            {DURATIONS.map(d => <option key={d.val} value={d.val}>{d.label}</option>)}
                        </select>
                    </div>

                    <button onClick={addNoteToComposition} style={{ alignSelf: 'flex-end', padding: '8px 15px', backgroundColor: '#28a745', color: '#fff', border: 'none', borderRadius: '4px', cursor: 'pointer', fontWeight: 'bold' }}>
                        + Ajouter
                    </button>
                </div>

                {/* Configuration Tempo */}
                <div style={{ marginBottom: '15px', display: 'flex', alignItems: 'center', gap: '10px' }}>
                    <label style={{ fontSize: '14px' }}>Tempo (BPM) : <strong>{bpm}</strong></label>
                    <input type="range" min="60" max="200" value={bpm} onChange={(e) => setBpm(e.target.value)} style={{ flex: 1 }} />
                </div>

                {/* Visualisation du morceau */}
                <div style={{ minHeight: '60px', backgroundColor: '#090909', borderRadius: '6px', padding: '10px', display: 'flex', flexWrap: 'wrap', gap: '5px', marginBottom: '15px', border: '1px solid #333' }}>
                    {composition.length === 0 && <span style={{ color: '#555', fontSize: '13px' }}>Partition vide. Ajoutez des notes ci-dessus...</span>}
                    {composition.map((note, idx) => (
                        <div key={idx} style={{ padding: '4px 8px', backgroundColor: note.freq === 0 ? '#442222' : '#224466', borderRadius: '4px', fontSize: '12px', display: 'flex', flexDirection: 'column', alignItems: 'center' }}>
                            <span style={{ fontWeight: 'bold' }}>{note.label}</span>
                            <span style={{ fontSize: '9px', color: '#aaa' }}>{note.durationLabel.split('-')[0]}</span>
                        </div>
                    ))}
                </div>

                {/* Actions de lecture */}
                <div style={{ display: 'flex', gap: '10px' }}>
                    <button onClick={clearComposition} style={{ flex: 1, padding: '10px', backgroundColor: '#dc3545', color: '#fff', border: 'none', borderRadius: '4px', cursor: 'pointer' }}>
                        Effacer tout
                    </button>
                    <button onClick={sendPartitionToTiva} disabled={composition.length === 0} style={{ flex: 2, padding: '10px', backgroundColor: '#007bff', color: '#fff', border: 'none', borderRadius: '4px', cursor: 'pointer', fontWeight: 'bold', opacity: composition.length === 0 ? 0.5 : 1 }}>
                        ▶️ Jouer le morceau sur la carte
                    </button>
                </div>
            </div>

        </div>
    );
}