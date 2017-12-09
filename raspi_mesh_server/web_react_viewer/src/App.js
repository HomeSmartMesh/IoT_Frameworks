import React, { Component } from 'react';
import './App.css';
import MultiNodes from './MultiNodes';

class App extends Component {
  render() {
    return (
      <div className="App">
        <header className="App-header">
          <h1 className="App-title">Home Smart Mesh</h1>
        </header>
        <MultiNodes/>
      </div>
    );
  }
}

export default App;
