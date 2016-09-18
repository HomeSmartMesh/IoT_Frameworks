var SerialPort = require("serialport");
var port = new SerialPort("/dev/ttyUSB0", {
    baudRate: 115200,
    parser: SerialPort.parsers.readline('\n')
});

port.on('open', function () {
        console.log('Port is open');
});

// open errors will be emitted as an error event 
port.on('error', function (err) {
    console.log('Error: ', err.message);
	process.exit(1);
})

var webSocketsServerPort = 4785;
var webSocketServer = require('websocket').server;
var http = require('http');
// list of currently connected clients (users)
var clients = [];

port.on('data', function (datalog) {
    console.log(datalog);
    // broadcast message to all connected clients
    var json = JSON.stringify({ type: 'message', data: datalog });
    for (var i = 0; i < clients.length; i++) {
        clients[i].sendUTF(json);
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


    // user disconnected
    connection.on('close', function (connection) {
            console.log((new Date()) + " Peer "+ connection.remoteAddress + " disconnected.");
    });

});