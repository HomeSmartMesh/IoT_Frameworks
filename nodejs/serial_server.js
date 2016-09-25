var SerialPort = require("serialport");

var port;

var webSocketsServerPort = 4785;
var webSocketServer = require('websocket').server;
var http = require('http');
// list of currently connected clients (users)
var clients = [];


/**
 * Helper function for escaping input strings
 */
function htmlEntities(str) {
    return String(str).replace(/&/g, '&amp;').replace(/</g, '&lt;')
                      .replace(/>/g, '&gt;').replace(/"/g, '&quot;');
}


SerialPort.list(function (err, ports) {

	var listOnly = false;
	var requestedPortNum = -1;
	if(process.argv.length > 2)
	{
		if(process.argv[2] == '--list')
		{
			listOnly = true;
		}
		else
		{
			
		}
	}
	else
	{
		console.log("\r\nCommands help:\r\nList ony available ports and exit with 'node serial_server.js --list'");		
		console.log("then request a particular port number with e.g. 'node serial_server.js 2': \r\n");
	}
	if(ports.length > 0)	
	{
		console.log(ports.length+ ' Ports Available: indexes from 0 to ' + (ports.length-1));
		
		ports.forEach(function(i_port) {
			console.log('Name: ' + i_port.comName + ' ; pnpId: ' +i_port.pnpId + " ; Man: "+ i_port.manufacturer);
		  });
		
		//if a listing only request then exit at this stage
		if(!listOnly)
		{
			var portNum = ports.length - 1;//we take the last one as it's likely that it's the last connected USB adapter
			if(requestedPortNum == -1)
			{
				console.log("no portNum requested, last one taken: "+portNum);
				portNum = ports.length - 1;//we take the last one as it's likely that it's the last connected USB adapter
				console.log("request a particular port number with e.g. 'node serial_server.js 2': ");
			}
			port = new SerialPort(ports[portNum].comName, 
									{
									autoOpen: false,
									baudRate: 115200,
									parser: SerialPort.parsers.readline('\n')
									}
								);		
			port.on('open', function () {
					console.log('Port '+ ports[portNum].comName +' is open');
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
				console.log(ports[portNum].comName+'>'+datalog);
				// broadcast message to all connected clients
				var json = JSON.stringify({ type: 'message', data: ports[portNum].comName+'>'+datalog });
				for (var i = 0; i < clients.length; i++) {
					clients[i].sendUTF(json);
				}
			});
		}
	}
	else
	{
		console.log('No Serial Ports Available');
	}
	if(listOnly)
	{
		console.log("Listing only with param '--list' now exiting");
		process.exit();
	}


	  
  
});





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
				if(port != undefined)
				if(port.isOpen())
				{
					port.write(command+'\r');
				}
        }
    });

    // user disconnected
    connection.on('close', function (connection) {
            console.log((new Date()) + " Peer "+ connection.remoteAddress + " disconnected.");
    });

});