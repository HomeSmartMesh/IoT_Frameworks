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

require(['MyStatus']);
//---------------------------------------------------------------------------
//					Chart
//---------------------------------------------------------------------------
var Chart = 
{
	svgID : "#ChartDisp",
	data : 
		[
			{time: new Date(1484478467000), value: 1223},
			{time: new Date(1484478468000), value: 1500},
			{time: new Date(1484478469000), value: 327}
		]
};

require(['MyChart']);

//---------------------------------------------------------------------------

require(['sensors_client.js']);
