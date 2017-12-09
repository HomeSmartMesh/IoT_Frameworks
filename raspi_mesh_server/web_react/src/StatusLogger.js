import React, { Component } from 'react';
import Websocket from 'react-websocket';


function SensorsList(props){
        const listitems = props.sensors.map((sensor) =>
        <li key={sensor.type}>{sensor.type} : {sensor.value}</li>
        );

        return(
            <div>
                <ul>{listitems}</ul>
            </div>
        );
}
    
function SensorsMap(props){
    const listitems = Object.keys(props.sensors).map((sensor) =>
        <li key={sensor}>{sensor} : {props.sensors[sensor].Values[0]}</li>
        );

        return(
            <div>
                <ul>{listitems}</ul>
            </div>
        );
}

function NodesList(props){
    const listitems = props.updatelist.map((node) =>
    <li key={node.NodeId}>
        NodeId : {node.NodeId} 
        <SensorsList sensors={node.sensors} /> 
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
            updatelist: [
                {NodeId:"26",
                    sensors:[
                        {   type:"light",
                            value:20
                        },
                        {   type:"temperature",
                            value:30
                        }
                    ]
                },
                {NodeId:"8",
                    sensors:[
                        {   type:"light",
                        value:21
                        },
                        {   type:"temperature",
                            value:31
                        }
                    ]
                },
                {NodeId:"17",
                    sensors:[
                        {   type:"light",
                        value:22
                        },
                        {   type:"temperature",
                            value:32
                        }
                    ]
                 },
            ],
            updatemap: {
                "26":{
                    light:{
                        Times:[3333],
                        Values:[20]
                    },
                    temperature:{
                        Times:[3334],
                        Values:[22]
                    }
                },
                "28":{
                    light:{
                        Times:[3333],
                        Values:[20]
                    },
                    temperature:{
                        Times:[3334],
                        Values:[22]
                    }
                }
            }
        }
    }

    handleData(data) {
        //console.log(data);
        let notification = JSON.parse(data);
        console.log(notification.update);
        this.setState( (prevState,props) => (
            {
                updatemap: Object.assign(prevState.updatemap,notification.update)
            }
        )
        );
    }
    componentWillMount()
    {
        console.log("componentWillMount");
        //console.log(this.state.update);
    }
    render(){
    return(
        <div>
            <NodesList updatelist={this.state.updatelist}/>
            <hr/>
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