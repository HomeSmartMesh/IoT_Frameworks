import React, { Component } from 'react';

import { Panel } from 'react-bootstrap';


function time_to_text(timestamp){
    var t = new Date(timestamp * 1000);//as per javascript time in ms and c++ time_t in sec
    return t.toLocaleTimeString() + " , " + t.toLocaleDateString();
}

function SensorsMap(props){
    const listitems = Object.keys(props.sensors).map((sensor) =>
        <li key={sensor}>
            {sensor} : {props.sensors[sensor].Values[0].toFixed(2)}  ({time_to_text(props.sensors[sensor].Times[0])})
        </li>
        );

        return(
            <div>
                <ul>{listitems}</ul>
            </div>
        );
}

class NodeView extends Component{
    render(){
        return(
        <Panel header={"NodeId : "+this.props.nodeName} bsStyle="primary">
            <SensorsMap sensors={this.props.sensors} /> 
        </Panel>
        );
    }
}

export default NodeView;
