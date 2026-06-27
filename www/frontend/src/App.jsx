import React from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';

import Dashboard from './pages/Dashboard';
import Joystick from './pages/Joystick';
import Buzzer from './pages/Buzzer';
import Micro from './pages/Micro';
import Screen from './pages/Screen';
import Accelero from './pages/Accelero';
import Navbar from './components/Navbar';
import LedControl from './pages/LedControl';

function App() {
  return (
    <Router>
      {/* On utilise le conteneur fluide de Bootstrap pour occuper 100% de la largeur */}
      <div className="container-fluid vh-100 p-0 d-flex flex-column flex-md-row">
        
        {/* Colonne de gauche : Notre Menu (Sidebar) */}
        <div className="bg-dark text-white border-end border-secondary" style={{ minWidth: '250px' }}>
          <Navbar />
        </div>

        {/* Colonne de droite : Le contenu principal de nos pages */}
        <main className="flex-grow-1 bg-light p-4 overflow-auto">
          {/* Un conteneur blanc Bootstrap pour entourer proprement nos composants */}
          <div className="card shadow-sm p-4 h-100 border-0">
            <Routes>
              <Route path="/" element={<Dashboard />} />
              <Route path="/joystick" element={<Joystick />} />
              <Route path="/buzzer" element={<Buzzer />} />
              <Route path="/micro" element={<Micro />} />
              <Route path="/screen" element={<Screen />} />
              <Route path="/accelero" element={<Accelero />} />
              <Route path="/led" element={<LedControl />} />
            </Routes>

          </div>
        </main>

      </div>
    </Router>
  );
}

export default App;