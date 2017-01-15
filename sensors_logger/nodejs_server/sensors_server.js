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

function jsout(data)
{
	return JSON.stringify(data,null,'  ');
}

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
		logger.info('streamer is connected');
		/*var jReq = {request : {id : 362}};
		var Req = JSON.stringify(jReq);
		ws.send(Req);*/

		ws.on(
		'message',
		function(data, flags)
		{
			if (flags.binary) { return; }
			var json = JSON.parse(data);
			console.log("received:streamer> ", jsout(json));
			//------------------------------- update -------------------------------
			if(json && ("update" in json) )
			{
				ws_browser.clients.forEach(
				function each(client)
				{
					client.send(data);
				}	);
				//keep all the new data in the current state array
				var upjson = json["update"];
				for (var key in upjson) 
				{
					if(!(key in lastVals))
						lastVals[key] = {};
					for(var sk in upjson[key])
					{
						lastVals[key][sk] = upjson[key][sk];
					}
				}
			}
			//------------------------------- response -------------------------------
			else if(json && ("response" in json) )
			{
				var message = json.response;
				if("Values" in json.response)
					console.log('  streamer:response:Nb Values> ',json.response.Values.length);
				if("id" in json.response)
				{
					var RequestClients = 
					ws_browser.clients.filter(
						function( obj )
						{
							return ("lastRequestId" in obj && obj.lastRequestId == json.response.id);
						}					);
	
					if(RequestClients.length >= 1)
					{
						RequestClients[0].send(data);
						console.log('  streamer:response:sent to client');
					}
				}
			}
			//------------------------------- others -------------------------------
			else
			{
				//nothing yet to do
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
		var alljson = {update:lastVals};
		var client_message = JSON.stringify(alljson);
		ws.send(client_message);
		console.log("startup update>", jsout(alljson));
		ws.on(
		'message',
		function(data, flags)
		{
			if (flags.binary) { return; }
			var json = JSON.parse(data);
			console.log('received:browser> ',jsout(json));
			//------------------------------- request -------------------------------
			if(json && ("request" in json) )
			{
				if("id" in json.request)
				{
					ws.lastRequestId = json.request.id;
					//only one client expected for the DE_Sensors - simple forward
					if(ws_sensors.clients.length > 0)
					{
						ws_sensors.clients[0].send(data);
					}
				}
			}
			//------------------------------- others -------------------------------
			else
			{
				//nothing to do
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
