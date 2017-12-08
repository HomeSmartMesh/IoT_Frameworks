import React, { Component } from 'react';
import Websocket from 'react-websocket';


function Sensors(props){
        const listitems = props.sensors.map((sensor) =>
        <li key={sensor.type}>{sensor.type} : {sensor.value}</li>
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
            update: [
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
            ]
        };
    }

    handleData(data) {
        //console.log(data);
        let result = JSON.parse(data);
        console.log(result);
        //this.setState({update: result.update});
    }
    componentWillMount()
    {
        console.log("componentWillMount");
        //console.log(this.state.update);
    }
    render(){
        const numbers = this.state.update;
        const listitems = numbers.map((node) =>
        <li key={node.NodeId}>
            NodeId : {node.NodeId} 
            <Sensors sensors={node.sensors} /> 
        </li>
        );

    return(
        <div>
            <ul>{listitems}</ul>
            <Websocket  url='ws://10.0.0.12:4348/measures'
                onMessage={this.handleData.bind(this)}
                debug={true}
                />
        </div>
        );
    }
}


export default StatusLogger;