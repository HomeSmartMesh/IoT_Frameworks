
// open connection
var connection = new WebSocket('ws://localhost:4785');


//8 leds array status
var LedStatus = ["0","0","0","0","0","0","0","0"];
//used from html led div onclick
function applyStatus()
{
	var Text = LedStatus.join('');
	console.log(Text);
	var a = parseInt(Text,2);
	var cmd;
	if(a <= 0xF)
		cmd = "Se0" + a.toString(16).toUpperCase();
	else
		cmd = "Se" + a.toString(16).toUpperCase();
	
	console.log(cmd);
	
	connection.send(cmd);
	for(var i = 0;i<8;i++)
	{
		if(LedStatus[i] == "1")
		{
			document.getElementById(i).setAttribute("class", "led-green-on");
		}
		else
		{
			document.getElementById(i).setAttribute("class", "led-green-off");
		}
	}
}
function btLedCall(elem)
{
	if(LedStatus[elem.id] == "1")
	{
		LedStatus[elem.id] = "0";
	}
	else
	{
		LedStatus[elem.id] = "1";
	}
	applyStatus();
}

function btAllLedCall(elem)
{
	if(elem.id == "AllOn")
	{
		LedStatus = ["1","1","1","1","1","1","1","1"];
	}
	else
	{
		LedStatus = ["0","0","0","0","0","0","0","0"];
	}
	applyStatus();
}


$(function () {
    "use strict";

	
    // for better performance - to avoid searching in DOM
    var content = $('#content');
    var input = $('#input');
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
        input.hide();
        $('span').hide();
        return;
    }


    connection.onopen = function () {
        // first we want users to enter their names
        input.removeAttr('disabled');
        status.text('Serial Port On');
    };

    connection.onerror = function (error) {
        // just in there were some problems with conenction...
        content.html($('<p>', { text: 'Sorry, but there\'s some problem with your '
                                    + 'connection or the server is down.' } ));
    };

    // most important part - incoming messages
    connection.onmessage = function (message) {
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
            input.removeAttr('disabled').focus();
            // from now user can start sending messages
        } else if (json.type === 'history') { // entire message history
            // insert every single message to the chat window
            for (var i=0; i < json.data.length; i++) {
                addMessage(json.data[i]);
            }
        } else if (json.type === 'ack') { // acknowledge of sent message
            input.removeAttr('disabled'); // let the user write another message
            addMessage(json.data);
        } else if (json.type === 'message') { // acknowledge of sent message
            input.removeAttr('disabled'); // let the user write another message
            addMessage(json.data);
        } else {
            console.log('Hmm..., I\'ve never seen JSON like this: ', json);
        }
    };

    /**
     * Send mesage when user presses Enter key
     */
    input.keydown(function(e) {
        if (e.keyCode === 13) {
            var msg = $(this).val();
            if (!msg) {
                return;
            }
            // send the message as an ordinary text
            connection.send(msg);
            $(this).val('');
            // disable the input field to make the user wait until server
            // sends back response
            input.attr('disabled', 'disabled');

            // we know that the first message sent from a user their name
            if (myName === false) {
                myName = msg;
            }
        }
    });

    /**
     * This method is optional. If the server wasn't able to respond to the
     * in 3 seconds then show some error message to notify the user that
     * something is wrong.
     */
    setInterval(function() {
        if (connection.readyState !== 1) {
            status.text('Error');
            input.attr('disabled', 'disabled').val('Unable to comminucate '
                                                 + 'with the WebSocket server.');
        }
    }, 3000);

    /**
     * Add message to the chat window
     */
    function addMessage(message) {
        content.prepend('<p><span style="color:Black"></span>' + message + '</p>');
    }
	
	//document.getElementById("myBtn").onclick = function() {document.getElementById("demo").innerHTML = "Hello World";};
	//$("#myBtn").onclick = function() {document.getElementById("demo").innerHTML = "Hello You";};
	/*$("button").button({
		icon: { icon: "ui-icon-gear" },
  classes: {
    "ui-button": "highlight"
  }
});*/
	
});