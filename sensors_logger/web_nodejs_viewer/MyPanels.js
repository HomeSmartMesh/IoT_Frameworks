//---------------------------------------------------------------------------
//					Status
//---------------------------------------------------------------------------
var Status = 
{
	svgID : "#StatusDisp",
	data  :
	[
		{ x: 100, y: 100, r: 20, color: "Blue", 
			Temperature : 26, Light : 500,
			Name : "LivingRoom"
		},
		{ x: 300, y: 100, r: 20, color: "Blue", 
			Temperature : 26, Light : 500,
			Name : "BathRoom"
		}
	]
};

var nmap = {6 : 0,
			7 : 1};

//---------------------------------------------------------------------------
//					Chart
//---------------------------------------------------------------------------
var now = Date.now();
var start_disp = now - 24*60*60*1000;//1 day
var CharParams = 
{
	svgID : "#ChartDisp",
	data : [],
	scale_x_domain : [start_disp,now],
	scale_y_domain : [10,25],
	SensorName : "Temperature"
};

//---------------------------------------------------------------------------
//					Requests
//---------------------------------------------------------------------------
var jReq = {
				request : 
				{
					id 		: Math.floor(Math.random() * 10000),
					type : "Duration",
					NodeId 	: 7,
					SensorName 	: "Temperature",
					start 		: CharParams.scale_x_domain[0],
					stop 		: CharParams.scale_x_domain[1]
				}
			};

var statusReq = {
				request : 
				{
					id 		: Math.floor(Math.random() * 10000),
					type : "update",
					NodeIds : [6,7]
				}
			};

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//					Loading modules
//---------------------------------------------------------------------------
require([	"MyStatus", 
			"LastHourChart",
			"sensors_client.js",
			"MovingChartExample"
		], 
		function()
		{
			console.log("require done");
			
			var MyChart = new MyTimeChart(CharParams);
			//initChart(Chart);
			initWebsocket(jReq,statusReq,MyChart);
		}
		);

$(function() {
    console.log( "ready!" );
});

