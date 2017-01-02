/*
  Dependencies :
  - node.js
  - npm
  - ws (npm install ws)

*/

"use strict";

var WebSocketServer = require('ws').Server;

var http = require('http');

var server_rf = http.createServer();
var server_measures = http.createServer();

var ws_sensors = new WebSocketServer({server: server_rf, path: '/rf_logger'});

var ws_browser = new WebSocketServer({server: server_measures, path: '/measures'});

var js = JSON.stringify;
var db = {};
var lastVals = {};
//nodes[6] = Obj,
//nodes[7] = Obj,
//Temperature[0].Time = date
//Temperature[0].Value = 23.5

ws_sensors.on(
	'connection', 
	function(ws) 
	{
		console.log('rf_logger is connected');

		ws.on(
		'message',
		function(data, flags)
		{
			if (flags.binary) { return; }
			console.log('rf_logger> ' + data);
			
			var str = data.toString();
			str = str.replace(',',';');//Light should use ';'
			var LineTabs = str.split('\t');
			//console.log('kv split : ', kv);
			if(LineTabs.length == 3) 
			{
				var NodeLine = LineTabs[2].split(';');
				//console.log('sv split : ', NodeLine);
				if(NodeLine.length == 2)
				{
					var nid = NodeLine[0].split(':');
					if(nid[0] == 'NodeId')
					{
						var NodeId = nid[1];
						var sval = NodeLine[1].split(':');
						if(lastVals[NodeId] === undefined)
						{
							lastVals[NodeId] = {};
						}
						lastVals[NodeId][sval[0]] = sval[1];
					}
				}
				console.log(lastVals);
			  //read  : JSON.stringify(db[kv]);
			  //write : 
			  //db[kv[0]] = kv.splice(0, 1);
			}
			
			ws.send('got it');
			ws_browser.clients.forEach(
			function each(client)
			{
				var client_message = JSON.stringify(lastVals);
				client.send(client_message);
			}	);
		}	);

		ws.on	(
		'close',
		function()
		{
		  console.log('Closing the connection');
		}		);

		ws.on(
		'error',
		function(e) 
		{
			console.log('Error : ', e);
		}	);
	}	);
//------------------------------------------------------------------------
ws_browser.on(	
	'connection', 
	function(ws) 
	{
		console.log('measures client is connected');
		//start by updating the client with the last available values
		var client_message = JSON.stringify(lastVals);
		ws.send(client_message);
				
		ws.on(
		'message',
		function(data, flags)
		{
			if (flags.binary) { return; }
			console.log('rf_logger> ' + data);
			ws.send('got it');
		}	);

		ws.on	(
		'close',
		function()
		{
		  console.log('Closing the connection');
		}		);

		ws.on(
		'error',
		function(e) 
		{
			console.log('Error : ', e);
		}	);
	}	);


server_rf.listen(8126);
console.log('RF server Listening on port 8126...');
server_measures.listen(4348);
console.log('Measures server Listening on port 4348...');
