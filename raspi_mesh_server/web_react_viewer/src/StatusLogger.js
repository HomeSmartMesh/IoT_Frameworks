import React, { Component } from 'react';
import Websocket from 'react-websocket';
import merge from 'deepmerge';

function mergeSensors(map_old,map_new){
    function overwriteMerge(destinationArray, sourceArray, options) {
        return sourceArray
    }
    //Overwrite required in order not to grow arrays (verified)
    let result = merge(map_old,map_new,{arrayMerge:overwriteMerge});
    return result;
}

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

function NodesMap(props){
    const listitems = Object.keys(props.updatemap).map((node) =>
    <li key={node}>
        NodeId : {node} 
        <SensorsMap sensors={props.updatemap[node]} /> 
    </li>
    );
    return(
            <div>
                <ul>{listitems}</ul>
            </div>
    );

}

class StatusLogger extends Component{
    constructor(props) {
        super(props);
        this.state = {
            updatemap: {}
        }
    }

    handleData(data) {
        let notification = JSON.parse(data);
        //console.log(notification.update);
        this.setState( (prevState,props) => (
            {
                updatemap: mergeSensors(prevState.updatemap,notification.update)
            }
        )
        );
    }
    render(){
    return(
        <div>
            <NodesMap updatemap={this.state.updatemap}/>
            <hr/>
            <Websocket  url='ws://10.0.0.12:4348/measures'
                onMessage={this.handleData.bind(this)}
                debug={true}
                />
        </div>
        );
    }
}


export default StatusLogger;