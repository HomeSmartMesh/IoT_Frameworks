/*
 - Client ws websocket 
*/

$(function () {
    "use strict";

    // for better performance - to avoid searching in DOM
    var content = $('#content');
    var status = $('#status');

    // my color assigned by the server
    var myColor = false;
    // my name sent to the server
    var myName = false;

    // if user is running mozilla then use it's built-in WebSocket
    window.WebSocket = window.WebSocket || window.MozWebSocket;

    // if browser doesn't support WebSocket, just show some notification and exit
    if (!window.WebSocket) {
        content.html($('<p>', { text: 'Sorry, but your browser doesn\'t '
                                    + 'support WebSockets.'} ));
        $('span').hide();
        return;
    }

    // open connection
    var connection = new WebSocket('ws://10.0.0.2:4348/measures');

    connection.onopen = 
	function()
	{
        // first we want users to enter their names
        status.text('Connected');
    };

    connection.onerror = 
	function(error) 
	{
        // just in there were some problems with conenction...
        content.html($('<p>', { text: 'Sorry, but there\'s some problem with your '
                                    + 'connection or the server is down.' } ));
    };

    // most important part - incoming messages
    connection.onmessage = 
	function(message)
	{
		addMessage(message.data);
		console.log('received ', message.data);
		/*
        // try to parse JSON message. Because we know that the server always returns
        // JSON this should work without any problem but we should make sure that
        // the massage is not chunked or otherwise damaged.
        try {
            var json = JSON.parse(message.data);
        } catch (e) {
            console.log('This doesn\'t look like a valid JSON: ', message.data);
            return;
        }

        // NOTE: if you're not sure about the JSON structure
        // check the server source code above
        if (json.type === 'color') { // first response from the server with user's color
            myColor = json.data;
            status.text(myName + ': ').css('color', myColor);
            // from now user can start sending messages
        } else if (json.type === 'message') { // it's a single message
            addMessage(json.data.author, json.data.text,
                       json.data.color, new Date(json.data.time));
        } else {
            console.log('Hmm..., I\'ve never seen JSON like this: ', json);
        }*/
    };

    /**
     * This method is optional. If the server wasn't able to respond to the
     * in 3 seconds then show some error message to notify the user that
     * something is wrong.
     */
    setInterval(
		function()
		{
			if (connection.readyState !== 1) 
			{
				status.text('Error');
			}
		}
		, 3000);

    /**
     * Add message to the chat window
     */
    function addMessage(message) {
        content.prepend('<p>' + message + '</p>');
    }
});