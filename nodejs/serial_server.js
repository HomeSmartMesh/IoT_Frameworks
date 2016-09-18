var SerialPort = require("serialport");

var port;

/**
 * Helper function for escaping input strings
 */
function htmlEntities(str) {
    return String(str).replace(/&/g, '&amp;').replace(/</g, '&lt;')
                      .replace(/>/g, '&gt;').replace(/"/g, '&quot;');
}


SerialPort.list(function (err, ports) {
	
		
	if(ports.length > 0)	
	{
		console.log(ports.length+ ' Ports Available:');
		
		ports.forEach(function(i_port) {
			console.log('Name: ' + i_port.comName + ' ; pnpId: ' +i_port.pnpId + " ; Man: "+ i_port.manufacturer);
		  });
		
		port = new SerialPort(ports[0].comName, 
								{
								autoOpen: false,
								baudRate: 115200,
								parser: SerialPort.parsers.readline('\n')
								}
							);		
							
		port.on('open', function () {
				console.log('Port '+ ports[0].comName +' is open');
		});
		
		port.open(function (err){
			if (err) 
			{
				console.log('Error opening port: ', err.message);
				process.exit(1);
				return;
			}
		});

		port.on('data', function (datalog) {
			console.log(datalog);
			// broadcast message to all connected clients
			var json = JSON.stringify({ type: 'message', data: datalog });
			for (var i = 0; i < clients.length; i++) {
				clients[i].sendUTF(json);
			}
		});
	}
	else
	{
		console.log('No Serial Ports Available');
	}
	

	  
  
});



var webSocketsServerPort = 4785;
var webSocketServer = require('websocket').server;
var http = require('http');
// list of currently connected clients (users)
var clients = [];


var server = http.createServer(function (request, response) {
    // Not important for us. We're writing WebSocket server, not HTTP server
});
server.listen(webSocketsServerPort, function () {
	

    console.log((new Date()) + " Server is listening on port " + webSocketsServerPort);
	
	
});
var wsServer = new webSocketServer({
    // WebSocket server is tied to a HTTP server. WebSocket request is just
    // an enhanced HTTP request. For more info http://tools.ietf.org/html/rfc6455#page-6
    httpServer: server
});
wsServer.on('request', function (request) {
    console.log((new Date()) + ' Connection from origin ' + request.origin + '.');

    var connection = request.accept(null, request.origin);
    // we need to know client index to remove them on 'close' event
    var index = clients.push(connection) - 1;

    console.log((new Date()) + ' Connection accepted.');

    // user sent some message
    connection.on('message', function(message) {
        if (message.type === 'utf8') { // accept only text
                // remember user name
                command = htmlEntities(message.utf8Data);
                connection.sendUTF(JSON.stringify({ type:'ack', data: command }));
                console.log((new Date()) + ' Command: ' + command );
        }
    });

    // user disconnected
    connection.on('close', function (connection) {
            console.log((new Date()) + " Peer "+ connection.remoteAddress + " disconnected.");
    });

});