import React, { Component } from 'react';
import logo from './logo.svg';
import './App.css';
import StatusLogger from './StatusLogger';

class App extends Component {
  render() {
    return (
      <div className="App">
        <header className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
          <h1 className="App-title">Home Smart Mesh</h1>
        </header>
        <StatusLogger/>
      </div>
    );
  }
}

export default App;
