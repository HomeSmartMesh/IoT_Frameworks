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

var ws_rf = new WebSocketServer({server: server_rf, path: '/rf_logger'});

var ws_measures = new WebSocketServer({server: server_measures, path: '/measures'});


ws_rf.on(
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
			ws.send('got it');
			ws_measures.clients.forEach(
			function each(client)
			{
				client.send('rf_logger ' + data);
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
ws_measures.on(	
	'connection', 
	function(ws) 
	{
		console.log('measures client is connected');

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
