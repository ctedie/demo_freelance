const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const app = express();
const HTTP_PORT = 3000;
const SERIAL_PORT_NAME = 'COM7'; // Ton port série validé

// Création du serveur HTTP natif nécessaire pour Socket.io
const server = http.createServer(app);

// Configuration de Socket.io avec le CORS pour React (port 5173)
const io = new Server(server, {
    cors: {
        origin: "http://localhost:5173",
        methods: ["GET", "POST"]
    }
});

// --- GESTION DU CORS POUR EXPRESS ---
app.use((req, res, next) => {
    res.setHeader('Access-Control-Allow-Origin', 'http://localhost:5173');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
    next();
});

app.use(express.json());

// --- CONFIGURATION DU PORT SÉRIE ---
const port = new SerialPort({
    path: SERIAL_PORT_NAME,
    baudRate: 115200,
    autoOpen: false
});

// Découpe les trames entrantes à chaque retour à la ligne (\r\n ou \n)
// Remplace '\r\n' par '\n'
const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

port.open((err) => {
    if (err) {
        return console.error("Erreur lors de l'ouverture du port série : ", err.message);
    }
    console.log(`Port série ${SERIAL_PORT_NAME} ouvert avec succès à 115200 bauds.`);
});

// --- RECEPTION DES DONNÉES (Tiva C -> Node.js -> React) ---
parser.on('data', (data) => {
    const rawLine = data.toString().trim();
    console.log("Données reçues du port série :", rawLine); //DEBUG
    // Décodage de la trame du Joystick "J:X,Y,Select,S1,S2"
    if (rawLine.startsWith('J:')) {
        const dataParts = rawLine.replace('J:', '').split(',');
        
        if (dataParts.length === 5) {
            const joystickData = {
                x: parseInt(dataParts[0], 10),
                y: parseInt(dataParts[1], 10),
                btnSelect: dataParts[2] === '1',
                btnS1: dataParts[3] === '1',
                btnS2: dataParts[4] === '1'
            };

            // Diffusion immédiate en WebSocket à l'application React
            io.emit('joystick-data', joystickData);
        }
    }
});

// GESTION DES SOCKETS
io.on('connection', (socket) => {
    console.log(`Client connecté au WebSocket (ID: ${socket.id})`);
    socket.on('disconnect', () => {
        console.log(`Client déconnecté du WebSocket.`);
    });
});

// --- ROUTE API (React -> Node.js -> Tiva C) ---
app.post('/api/led', (expressReq, expressRes) => {
    const { r, g, b } = expressReq.body;

    if (r === undefined || g === undefined || b === undefined) {
        return expressRes.status(400).json({ error: 'Paramètres r, g ou b manquants.' });
    }

    // Formatage de la trame attendue par le code C
    const frame = `${r},${g},${b}\n`;

    port.write(frame, (err) => {
        if (err) {
            console.error("Erreur d'écriture série :", err.message);
            return expressRes.status(500).json({ error: "Impossible d'envoyer la commande à la carte." });
        }
        console.log(`Trame LED envoyée : ${frame.trim()}`);
        return expressRes.json({ success: true, sent: frame.trim() });
    });
});

// DÉMARRAGE SUR LE SERVEUR UNIQUE
server.listen(HTTP_PORT, () => {
    console.log(`Serveur Web Fullstack actif sur http://localhost:${HTTP_PORT}`);
});