const express = require('express');
const { SerialPort } = require('serialport');
const path = require('path');

const app = express();
const HTTP_PORT = 3000;
const SERIAL_PORT_NAME = 'COM7'; // Ton port validé

// --- GESTION DU CORS ---
// Permet à React (port 5173) de faire des requêtes vers Node.js (port 3000)
app.use((req, res, next) => {
    res.setHeader('Access-Control-Allow-Origin', 'http://localhost:5173');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
    next();
});

// --- CONFIGURATION DU PORT SÉRIE ---
// Initialisation de la communication série avec la Tiva C
const port = new SerialPort({
    path: SERIAL_PORT_NAME,
    baudRate: 115200,
    autoOpen: false // On gère l'ouverture manuellement pour intercepter les erreurs
});

// Ouverture du port série
port.open((err) => {
    if (err) {
        return console.error("Erreur lors de l'ouverture du port série : ", err.message);
    }
    console.log(`Port série ${SERIAL_PORT_NAME} ouvert avec succès à 115200 bauds.`);
});

// --- CONFIGURATION EXPRESS ---
app.use(express.json()); // Permet de lire le format JSON dans les requêtes
app.use(express.static(path.join(__dirname, 'public'))); // Sert les fichiers du dossier /public (si besoin)

// --- ROUTES API ---
// API pour recevoir les couleurs du front-end React et les envoyer à la Tiva C
app.post('/api/led', (expressReq, expressRes) => {
    const { r, g, b } = expressReq.body;

    // Validation rapide des données reçues
    if (r === undefined || g === undefined || b === undefined) {
        return expressRes.status(400).json({ error: 'Paramètres r, g ou b manquants.' });
    }

    // Formatage de la trame au format "R,G,B\n" attendu par ton code C
    const frame = `${r},${g},${b}\n`;

    // Envoi de la trame sur le port série
    port.write(frame, (err) => {
        if (err) {
            console.error("Erreur d'écriture série :", err.message);
            return expressRes.status(500).json({ error: "Impossible d'envoyer la commande à la carte." });
        }
        console.log(`Trame envoyée avec succès : ${frame.trim()}`);
        return expressRes.json({ success: true, sent: frame.trim() });
    });
});

// --- DÉMARRAGE DU SERVEUR ---
app.listen(HTTP_PORT, () => {
    console.log(`Serveur Web Node.js démarré sur http://localhost:${HTTP_PORT}`);
});