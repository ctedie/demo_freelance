import React, { useState } from 'react';

function LedControl() {
  // On initialise les trois couleurs à 10 comme dans ton ancien projet
  const [red, setRed] = useState(10);
  const [green, setGreen] = useState(10);
  const [blue, setBlue] = useState(10);

  // Fonction pour envoyer les valeurs au serveur Node.js (Port 3000)
  const updateLED = async (r, g, b) => {
    try {
      // Note : On cible explicitement localhost:3000 car Vite tourne sur le port 5173
      await fetch('http://localhost:3000/api/led', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({ r: parseInt(r), g: parseInt(g), b: parseInt(b) })
      });
    } catch (error) {
      console.error("Erreur lors de l'envoi HTTP :", error);
    }
  };

  // Handlers pour chaque slider
  const handleRedChange = (e) => {
    const val = e.target.value;
    setRed(val);
    updateLED(val, green, blue);
  };

  const handleGreenChange = (e) => {
    const val = e.target.value;
    setGreen(val);
    updateLED(red, val, blue);
  };

  const handleBlueChange = (e) => {
    const val = e.target.value;
    setBlue(val);
    updateLED(red, green, val);
  };

  return (
    <div>
      <div className="mb-4">
        <h2>🔴 Contrôle de la LED RGB</h2>
        <p className="text-muted">Ajustez les curseurs pour modifier la couleur de la LED de la Tiva C en temps réel.</p>
      </div>

      <div className="row g-4">
        {/* Colonne des contrôles */}
        <div className="col-12 col-md-6">
          <div className="card p-4 border-0 bg-light">
            
            {/* Slider Rouge */}
            <div className="mb-4">
              <div className="d-flex justify-content-between font-weight-bold">
                <label htmlFor="redRange" className="form-label text-danger fw-bold">Rouge</label>
                <span className="badge bg-danger fs-6">{red}</span>
              </div>
              <input 
                type="range" 
                className="form-range" 
                id="redRange" 
                min="0" 
                max="255" 
                value={red} 
                onChange={handleRedChange} 
              />
            </div>

            {/* Slider Vert */}
            <div className="mb-4">
              <div className="d-flex justify-content-between">
                <label htmlFor="greenRange" className="form-label text-success fw-bold">Vert</label>
                <span className="badge bg-success fs-6">{green}</span>
              </div>
              <input 
                type="range" 
                className="form-range" 
                id="greenRange" 
                min="0" 
                max="255" 
                value={green} 
                onChange={handleGreenChange} 
              />
            </div>

            {/* Slider Bleu */}
            <div className="mb-4">
              <div className="d-flex justify-content-between">
                <label htmlFor="blueRange" className="form-label text-primary fw-bold">Bleu</label>
                <span className="badge bg-primary fs-6">{blue}</span>
              </div>
              <input 
                type="range" 
                className="form-range" 
                id="blueRange" 
                min="0" 
                max="255" 
                value={blue} 
                onChange={handleBlueChange} 
              />
            </div>

          </div>
        </div>

        {/* Colonne de Prévisualisation web */}
        <div className="col-12 col-md-6 d-flex flex-column align-items-center justify-content-center">
          <h5 className="text-secondary mb-3">Aperçu Web</h5>
          <div 
            className="shadow rounded-circle border border-secondary-subtle" 
            style={{ 
              width: '180px', 
              height: '180px', 
              backgroundColor: `rgb(${red}, ${green}, ${blue})`,
              transition: 'background-color 0.05s ease'
            }}
          ></div>
        </div>
      </div>
    </div>
  );
}

export default LedControl;