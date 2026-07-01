import React, { useState } from 'react';

// Dictionnaires de solfège
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
    const [bpm, setBpm] = useState(120);
    const [composition, setComposition] = useState([]);
    const [selectedNote, setSelectedNote] = useState(262);
    const [selectedDuration, setSelectedDuration] = useState(8);

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

    // --- Fonctions Séquenceur ---
    const addNoteToComposition = () => {
        if (composition.length >= 128) {
            alert("Maximum de 128 notes atteint.");
            return;
        }

        const newNotes = [{ freq: Number(selectedNote), duration: Number(selectedDuration) }];
        
        // Ajout automatique d'un micro-silence si ce n'est pas déjà un silence
        if (Number(selectedNote) !== 0) {
            newNotes.push({ freq: 0, duration: 1 });
        }

        setComposition([...composition, ...newNotes].slice(0, 128));
    };

    const updateNote = (index, field, value) => {
        const updated = [...composition];
        updated[index][field] = Number(value);
        setComposition(updated);
    };

    const removeNote = (index) => {
        setComposition(composition.filter((_, i) => i !== index));
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
        } catch (err) { console.error("Erreur envoi séquence:", err); }
    };

    return (
        <div style={{ padding: '20px', maxWidth: '800px', margin: '0 auto', fontFamily: 'system-ui', color: '#fff', backgroundColor: '#121212', borderRadius: '12px' }}>
            <h2 style={{ textAlign: 'center' }}>🎵 Studio Musical Tiva C</h2>

            {/* SECTION 1: PIANO LIVE */}
            <div style={{ backgroundColor: '#1e1e1e', padding: '15px', borderRadius: '8px', marginBottom: '20px' }}>
                <h4 style={{ color: '#aaa', marginTop: 0 }}>Clavier d'écoute Live</h4>
                <div style={{ display: 'flex', height: '80px', backgroundColor: '#000', borderRadius: '4px' }}>
                    {AVAILABLE_NOTES.filter(n => n.val !== 0).map(note => (
                        <button key={note.val} onMouseDown={() => playNoteLive(note.val)} onMouseUp={stopNoteLive} onMouseLeave={stopNoteLive} style={{ flex: 1, margin: '2px', cursor: 'pointer' }}>
                            {note.label.split(' ')[0]}
                        </button>
                    ))}
                </div>
            </div>

            {/* SECTION 2: ÉDITEUR */}
            <div style={{ backgroundColor: '#1e1e1e', padding: '15px', borderRadius: '8px' }}>
                <h4 style={{ color: '#aaa', marginTop: 0 }}>Timeline de Partition</h4>
                
                <div style={{ display: 'flex', gap: '10px', marginBottom: '15px' }}>
                    <select value={selectedNote} onChange={(e) => setSelectedNote(e.target.value)} style={{ flex: 1, padding: '8px', background: '#333', color: '#fff', border: 'none' }}>
                        {AVAILABLE_NOTES.map(n => <option key={n.val} value={n.val}>{n.label}</option>)}
                    </select>
                    <select value={selectedDuration} onChange={(e) => setSelectedDuration(e.target.value)} style={{ flex: 1, padding: '8px', background: '#333', color: '#fff', border: 'none' }}>
                        {DURATIONS.map(d => <option key={d.val} value={d.val}>{d.label}</option>)}
                    </select>
                    <button onClick={addNoteToComposition} style={{ padding: '8px 20px', background: '#28a745', border: 'none', color: '#fff', cursor: 'pointer' }}>+ Ajouter</button>
                </div>

                <div style={{ display: 'flex', overflowX: 'auto', gap: '10px', padding: '10px', backgroundColor: '#090909', borderRadius: '6px', marginBottom: '15px', border: '1px solid #333' }}>
                    {composition.map((note, index) => (
                        <div key={index} style={{ padding: '8px', backgroundColor: note.freq === 0 ? '#3a1c1c' : '#1c3a5a', borderRadius: '6px', minWidth: '100px' }}>
                            <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                                <span style={{ fontSize: '10px' }}>{note.freq === 0 ? 'Silence' : `Note ${index + 1}`}</span>
                                <button onClick={() => removeNote(index)} style={{ background: 'none', border: 'none', color: '#f00', cursor: 'pointer' }}>✖</button>
                            </div>
                            <select value={note.freq} onChange={(e) => updateNote(index, 'freq', e.target.value)} style={{ width: '100%', background: '#222', color: '#fff', fontSize: '12px' }}>
                                {AVAILABLE_NOTES.map(n => <option key={n.val} value={n.val}>{n.label.split(' ')[0]}</option>)}
                            </select>
                            <select value={note.duration} onChange={(e) => updateNote(index, 'duration', e.target.value)} style={{ width: '100%', background: '#222', color: '#fff', fontSize: '12px' }}>
                                {DURATIONS.map(d => <option key={d.val} value={d.val}>{d.label.split(' ')[0]}</option>)}
                            </select>
                        </div>
                    ))}
                </div>

                <div style={{ marginBottom: '15px' }}>
                    <label>BPM: {bpm}</label>
                    <input type="range" min="60" max="200" value={bpm} onChange={(e) => setBpm(e.target.value)} style={{ width: '100%' }} />
                </div>

                <div style={{ display: 'flex', gap: '10px' }}>
                    <button onClick={clearComposition} style={{ flex: 1, padding: '10px', background: '#dc3545', border: 'none', color: '#fff', cursor: 'pointer' }}>Effacer tout</button>
                    <button onClick={sendPartitionToTiva} style={{ flex: 2, padding: '10px', background: '#007bff', border: 'none', color: '#fff', cursor: 'pointer', fontWeight: 'bold' }}>▶️ Jouer le morceau</button>
                </div>
            </div>
        </div>
    );
}