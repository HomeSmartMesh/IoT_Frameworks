/*
  Dependencies :
  - node.js
  - npm
  - ws (npm install ws)

*/

"use strict";

var WebSocketServer = require('ws').Server;

var http = require('http');

var server = http.createServer();

var ws_serv = new WebSocketServer({server: server, path: '/rf_logger'});




ws_serv.on(	
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

server.listen(8126);

console.log('Listening on port 8126...');
