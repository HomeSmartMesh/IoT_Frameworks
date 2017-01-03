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

function myTimestamp() {
  return new Date().toString();
};

 var winston = require('winston');

  var logger = new (winston.Logger)({
    transports: [
      new winston.transports.Console(),
      new winston.transports.File({ 
			filename: '/home/pi/share/sensors_server.log',
			level : 'verbose',
			json : false,
			//handleExceptions: true,
			maxsize: 5242880,//5MB
			timestamp: myTimestamp
			})
    ],
    exceptionHandlers: [
      new winston.transports.File({ filename: '/home/pi/share/node_exceptions.log' })
    ]
  });

ws_sensors.on(
	'connection', 
	function(ws) 
	{
		logger.info('rf_logger is connected');

		ws.on(
		'message',
		function(data, flags)
		{
			if (flags.binary) { return; }
			logger.verbose('rf_logger> ' + data);
			
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
				logger.verbose(lastVals);
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
		  logger.info('Closing the connection');
		}		);

		ws.on(
		'error',
		function(e) 
		{
			logger.error(e);
		}	);
	}	);
//------------------------------------------------------------------------
ws_browser.on(	
	'connection', 
	function(ws) 
	{
		logger.info('measures client is connected');
		//start by updating the client with the last available values
		var client_message = JSON.stringify(lastVals);
		ws.send(client_message);
				
		ws.on(
		'message',
		function(data, flags)
		{
			if (flags.binary) { return; }
			logger.info('rf_logger> ' + data);
			ws.send('got it');
		}	);

		ws.on	(
		'close',
		function()
		{
		  logger.info('Closing the connection');
		}		);

		ws.on(
		'error',
		function(e) 
		{
			logger.error(e);
		}	);
	}	);


server_rf.listen(8126);
logger.info('RF server Listening on port 8126...');
server_measures.listen(4348);
logger.info('Measures server Listening on port 4348...');
