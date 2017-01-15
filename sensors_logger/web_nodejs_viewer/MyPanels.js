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

//require(['MyStatus']);
//---------------------------------------------------------------------------
//					Chart
//---------------------------------------------------------------------------
var now = Date.now();
var start_disp = now - 24*60*60*1000;//10 mn
var Chart = 
{
	svgID : "#ChartDisp",
	data : 
		[
			{time: new Date('2017-01-14T09:00:00'), value: 1223},
			{time: new Date('2017-01-14T09:10:00'), value: 1500},
			{time: new Date('2017-01-14T09:15:00'), value: 200},
			{time: new Date('2017-01-14T09:30:00'), value: 2000}
			
		],
//	scale_x_domain : [new Date('2017-01-14T07:00:00'),new Date('2017-01-14T11:00:00')],
	scale_x_domain : [start_disp,now],
	scale_y_domain : [10,25],
	SensorName : "Temperature"
};

//require(['MyChart']);
//require(['LastHourChart']);


var UniqueRequestId = Math.floor(Math.random() * 10000);
var jReq = {
				request : 
				{
					id 		: UniqueRequestId,
					type : "Duration",
					NodeId 	: 7,
					SensorName 	: "Temperature",
					start 		: Chart.scale_x_domain[0],
					stop 		: Chart.scale_x_domain[1]
				}
			};

//---------------------------------------------------------------------------

require(["MyStatus", "LastHourChart"], function(){
	
require(["sensors_client.js"]);
	
});


require(["MovingChartExample"]);
