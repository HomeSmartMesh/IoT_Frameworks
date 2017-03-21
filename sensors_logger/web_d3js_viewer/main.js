//---------------------------------------------------------------------------
//					Status Panel
//---------------------------------------------------------------------------
var StatusParams = 
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
//					Time Chart
//---------------------------------------------------------------------------
var now = Date.now();
var start_disp = now - 24*60*60*1000;//1 day
var ChartsParamsList = 
[
	{
		svgID : "#ChartLivTemp",
		scale_x_domain : [start_disp,now],
		scale_y_domain : [10,25],
		NodeId : 6,
		SensorName : "Temperature",
		Label : "Livingroom Temperature °C"
	},
	{
		svgID : "#ChartLivHum",
		scale_x_domain : [start_disp,now],
		scale_y_domain : [0,100],
		NodeId : 6,
		SensorName : "Humidity",
		Label : "Livingroom Humidity %RH"
	},
	{
		svgID : "#ChartBathTemp",
		scale_x_domain : [start_disp,now],
		scale_y_domain : [10,25],
		NodeId : 7,
		SensorName : "Temperature",
		Label : "Bathroom Temperature °C"
	},
	{
		svgID : "#ChartBathHum",
		scale_x_domain : [start_disp,now],
		scale_y_domain : [0,100],
		NodeId : 7,
		SensorName : "Humidity",
		Label : "Bathroom Humidity %RH"
	},
	{
		svgID : "#ChartBlacTemp",
		scale_x_domain : [start_disp,now],
		scale_y_domain : [0,35],
		NodeId : 16,
		SensorName : "Temperature",
		Label : "Balcony Temperature °C"
	},
	{
		svgID : "#ChartBalcHum",
		scale_x_domain : [start_disp,now],
		scale_y_domain : [0,100],
		NodeId : 16,
		SensorName : "Humidity",
		Label : "Balcony Humidity %RH"
	},
	{
		svgID : "#ChartRoomLight",
		scale_x_domain : [start_disp,now],
		scale_y_domain : [0,1800],
		NodeId : 6,
		SensorName : "Light",
		Label : "Room Light"
	},
	{
		svgID : "#ChartBathLight",
		scale_x_domain : [start_disp,now],
		scale_y_domain : [0,1800],
		NodeId : 7,
		SensorName : "Light",
		Label : "Bathroom Light"
	}
];
//---------------------------------------------------------------------------
//					Status Request
//---------------------------------------------------------------------------
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
require([	"StatusPanel", //still using dirty hack global var Status
			"TimeChart",
			"websocket"
		], 
		function()
		{
			console.log("require> done");
			
			var MyPanel = new StatusPanel(StatusParams);

			var MyChartList = new TimeChartsList(ChartsParamsList);

			var reqList = [statusReq].concat(MyChartList.getAllRequestsDuration());

			initWebsocket(reqList,MyPanel,MyChartList);
		}
		);
