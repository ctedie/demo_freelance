import React from 'react';
import { Link, useLocation } from 'react-router-dom';

function Navbar() {
  const location = useLocation();

  // Fonction pour appliquer le style "actif" de Bootstrap selon la page courante
  const getLinkClass = (path) => {
    const baseClass = "nav-link text-white my-1 rounded py-2 px-3 d-block transition-all";
    return location.pathname === path ? `${baseClass} bg-primary active` : `${baseClass} hover-bg-dark`;
  };

  return (
    <div className="d-flex flex-column p-3 h-100">
      {/* Titre du Dashboard */}
      <h5 className="text-center text-primary fw-bold mb-4 py-2 border-bottom border-secondary">
        ⚡ Tiva C Control
      </h5>

      {/* Liens de navigation */}
      <ul className="nav nav-pills flex-column mb-auto">
        <li className="nav-item">
          <Link to="/" className={getLinkClass('/')}>🏠 Tableau de bord</Link>
        </li>
        <li className="nav-item">
          <Link to="/led" className={getLinkClass('/led')}>🔴 LED RGB</Link>
        </li>
        <li className="nav-item">
          <Link to="/joystick" className={getLinkClass('/joystick')}>🕹️ Joystick & Boutons</Link>
        </li>
        <li className="nav-item">
          <Link to="/buzzer" className={getLinkClass('/buzzer')}>🔊 Buzzer</Link>
        </li>
        <li className="nav-item">
          <Link to="/micro" className={getLinkClass('/micro')}>🎙️ Microphone</Link>
        </li>
        <li className="nav-item">
          <Link to="/screen" className={getLinkClass('/screen')}>📺 Écran LCD</Link>
        </li>
        <li className="nav-item">
          <Link to="/accelero" className={getLinkClass('/accelero')}>📐 Accéléromètre</Link>
        </li>
      </ul>
    </div>
  );
}

export default Navbar;