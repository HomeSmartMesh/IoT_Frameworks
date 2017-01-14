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
		/*var jReq = {request : {id : 362}};
		var Req = JSON.stringify(jReq);
		ws.send(Req);*/

		ws.on(
		'message',
		function(data, flags)
		{
			if (flags.binary) { return; }
			
			var json = JSON.parse(data);
			//------------------------------- update -------------------------------
			if(json && ("update" in json) )
			{
				var message = json["update"];
				console.log('rf_logger:update> ',message);
				logger.verbose('rf_logger:update> ',message);
				for (var key in message) 
				{
					if(!(key in lastVals))
						lastVals[key] = {};
					for(var sk in message[key])
					{
						lastVals[key][sk] = message[key][sk];
					}
				}
				
				ws_browser.clients.forEach(
				function each(client)
				{
					var client_message = JSON.stringify(lastVals);
					client.send(client_message);
				}	);
			}
			//------------------------------- response -------------------------------
			else if(json && ("response" in json) )
			{
				var message = json.response;
				if("Values" in json.response)
					console.log('rf_logger:response Nb Values> ',json.response.Values.length);
				//logger.verbose('rf_logger:response> ',message);
				if("id" in json.response)
				{
					//console.log("--- looking for clients to give response>>>>");
					var clients = 
					ws_browser.clients.filter(
						function( obj )
						{
							return ("lastRequestId" in obj && obj.lastRequestId == json.response.id);
						}					);
					//console.log("--- found", clients.length, "clients");
	
					if(clients.length >= 1)
					{
						clients[0].send(data);
						console.log('rf_logger:send to client');
					}
				}
			}
			//------------------------------- others -------------------------------
			else
			{
				console.log('rf_logger:data> ',data);
				logger.verbose('rf_logger:data> ',data);
			}
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
			var json = JSON.parse(data);
			//------------------------------- request -------------------------------
			if(json && ("request" in json) )
			{
				if("id" in json.request)
				{
					ws.lastRequestId = json.request.id;
					var message = json.request;
					console.log('browser:request> ',message);
					logger.verbose('browser:request> ',message);
					//only one client expected for the DE_Sensors - simple forward
					if(ws_sensors.clients.length >= 0)
					{
						ws_sensors.clients[0].send(data);
					}
				}
			}
			//------------------------------- others -------------------------------
			else
			{
				console.log('browser:data> ',data);
				logger.verbose('browser:data> ',data);
			}
			
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
