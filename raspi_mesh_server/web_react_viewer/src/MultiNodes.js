import React, { Component } from 'react';
import Websocket from 'react-websocket';
import merge from 'deepmerge';

import NodeView from './NodeView';

import {Grid,Row,Col, Clearfix} from 'react-bootstrap';

function mergeSensors(map_old,map_new){
    function overwriteMerge(destinationArray, sourceArray, options) {
        return sourceArray
    }
    //Overwrite required in order not to grow arrays (verified)
    let result = merge(map_old,map_new,{arrayMerge:overwriteMerge});
    return result;
}

function NodesMap(props){
    const listitems = Object.keys(props.updatemap).map((node) =>
        <NodeView nodeName={node} sensors={props.updatemap[node]}/>
    );
    return(
        <Grid>
            <Row>
                <Col xs={10} xsOffset={1}>
                    {listitems}
                </Col>
            </Row>
        </Grid>
    );

}

class MultiNodes extends Component{
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


export default MultiNodes;
