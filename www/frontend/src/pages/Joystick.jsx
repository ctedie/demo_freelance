import React, { useEffect, useState } from 'react';
import { io } from 'socket.io-client';

// Connexion réseau persistante vers le serveur Node (Port 3000)
const socket = io('http://localhost:3000');

export default function Joystick() {
    const [joystick, setJoystick] = useState({
        x: 2048,
        y: 2048,
        btnSelect: false,
        btnS1: false,
        btnS2: false
    });

    useEffect(() => {
        // Enregistrement de l'écouteur d'événements pour le flux de la carte
        socket.on('joystick-data', (data) => {
            setJoystick(data);
        });

        // Nettoyage de l'abonnement en cas de démontage du composant
        return () => {
            socket.off('joystick-data');
        };
    }, []);

    // Conversion de la plage dynamique de l'ADC (0-4095) en pourcentages centrés (-50 à 50)
    const posX = ((joystick.x / 4095) * 100 - 50).toFixed(0);
    const posY = ((joystick.y / 4095) * 100 - 50).toFixed(0);

    return (
        <div style={{ 
            padding: '20px', 
            border: '1px solid #ddd', 
            borderRadius: '12px', 
            maxWidth: '380px', 
            fontFamily: 'sans-serif', 
            boxShadow: '0 4px 6px rgba(0,0,0,0.05)',
            backgroundColor: '#ffffff'
        }}>
            <h3 style={{ marginTop: 0, color: '#2f3640' }}> Moniteur Joystick & Boutons</h3>
            <hr style={{ border: '0', borderTop: '1px solid #eee', marginBottom: '15px' }} />
            
            {/* Affichage des valeurs brutes de l'ADC */}
            <div style={{ margin: '15px 0', fontSize: '14px', color: '#57606f', lineHeight: '1.6' }}>
                <p style={{ margin: '4px 0' }}><strong>Axe X (Brut) :</strong> <code style={{ backgroundColor: '#f1f2f6', padding: '2px 6px', borderRadius: '4px' }}>{joystick.x}</code> ({posX}%)</p>
                <p style={{ margin: '4px 0' }}><strong>Axe Y (Brut) :</strong> <code style={{ backgroundColor: '#f1f2f6', padding: '2px 6px', borderRadius: '4px' }}>{joystick.y}</code> ({posY}%)</p>
            </div>

            {/* État visuel et dynamique des 3 boutons */}
            <div style={{ display: 'flex', gap: '8px', marginBottom: '25px' }}>
                <span style={{ 
                    fontSize: '12px', fontWeight: 'bold', padding: '6px 12px', borderRadius: '20px', 
                    backgroundColor: joystick.btnSelect ? '#ff4757' : '#f1f2f6', 
                    color: joystick.btnSelect ? '#fff' : '#57606f', transition: 'all 0.1s' 
                }}>
                    SEL (Stick)
                </span>
                <span style={{ 
                    fontSize: '12px', fontWeight: 'bold', padding: '6px 12px', borderRadius: '20px', 
                    backgroundColor: joystick.btnS1 ? '#2ed573' : '#f1f2f6', 
                    color: joystick.btnS1 ? '#fff' : '#57606f', transition: 'all 0.1s' 
                }}>
                    Bouton S1
                </span>
                <span style={{ 
                    fontSize: '12px', fontWeight: 'bold', padding: '6px 12px', borderRadius: '20px', 
                    backgroundColor: joystick.btnS2 ? '#1e90ff' : '#f1f2f6', 
                    color: joystick.btnS2 ? '#fff' : '#57606f', transition: 'all 0.1s' 
                }}>
                    Bouton S2
                </span>
            </div>

            {/* Représentation géométrique du Pad Analogique */}
            <div style={{ position: 'relative', width: '130px', height: '130px', backgroundColor: '#f5f6fa', borderRadius: '50%', margin: '0 auto', border: '2px solid #dcdde1' }}>
                {/* Axes de centrage orthogonaux */}
                <div style={{ position: 'absolute', top: '50%', left: 0, right: 0, height: '1px', backgroundColor: '#dcdde1' }} />
                <div style={{ position: 'absolute', left: '50%', top: 0, bottom: 0, width: '1px', backgroundColor: '#dcdde1' }} />
                
                {/* Le pointeur physique mobile du stick */}
                <div style={{ 
                    position: 'absolute', 
                    width: '20px', 
                    height: '20px', 
                    backgroundColor: joystick.btnSelect ? '#ff4757' : '#2f3640', 
                    borderRadius: '50%',
                    left: `calc(50% - 10px + ${posX * 1.1}px)`, // Facteur d'échelle pour l'amplitude visuelle
                    top: `calc(50% - 10px + ${posY * -1.1}px)`, // Inversion géométrique pour l'axe Y standard (Haut = Positif)
                    transition: 'left 0.03s, top 0.03s',
                    boxShadow: '0 2px 5px rgba(0,0,0,0.25)'
                }} />
            </div>
        </div>
    );
}

