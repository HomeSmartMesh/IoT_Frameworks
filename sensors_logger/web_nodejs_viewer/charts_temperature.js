
var sRoom = AmCharts.makeChart( "chartserRoom", {
  "type": "serial",
  "theme": "dark",
  "autoMargins": true,
  "marginTop": 30,
  "marginLeft": 10,
  "marginBottom": 30,
  "marginRight": 50,
  "dataProvider": [ {
    "category": "Room Temperature",
    "Variable": 35,
    "bullet": 34,
  } ],
  "valueAxes": [ {
	"maximum": 35,
	"stackType": "regular",
	"gridAlpha": 0
  } ],
  "startDuration": 1,
	  "graphs": 
	  [
			{
				"valueField": "Variable",
				"showBalloon": false,
				"type": "column",
				"lineAlpha": 0,
				"fillAlphas": 0.8,
				"fillColors": [ "#c9e3ef", "#20a8e8", "#82f291", "#f2b130","#bf1903"],
				"gradientOrientation": "vertical",
			  }
			, {
				"clustered": false,
				"columnWidth": 0.3,
				"fillAlphas": 1,
				"lineColor": "#000000",
				"stackable": false,
				"type": "column",
				"valueField": "bullet"
			  } 
	  ],
  "rotate": false,
  "columnWidth": 1,
  "categoryField": "category",
  "categoryAxis": {
	"gridAlpha": 0,
	"position": "left"
  }
} );

var sBath = AmCharts.makeChart( "chartserBath", {
  "type": "serial",
  "theme": "dark",
  "autoMargins": false,
  "marginTop": 30,
  "marginLeft": 80,
  "marginBottom": 30,
  "marginRight": 50,
  "dataProvider": [ {
    "category": "Room Temperature",
    "Variable": 35,
    "bullet": 34,
  } ],
  "valueAxes": [ {
	"maximum": 35,
	"stackType": "regular",
	"gridAlpha": 0
  } ],
  "startDuration": 1,
	  "graphs": 
	  [
			{
				"valueField": "Variable",
				"showBalloon": false,
				"type": "column",
				"lineAlpha": 0,
				"fillAlphas": 0.8,
				"fillColors": [ "#c9e3ef", "#20a8e8", "#82f291", "#f2b130","#bf1903"],
				"gradientOrientation": "vertical",
			  }
			, {
				"clustered": false,
				"columnWidth": 0.3,
				"fillAlphas": 1,
				"lineColor": "#000000",
				"stackable": false,
				"type": "column",
				"valueField": "bullet"
			  } 
	  ],
  "rotate": false,
  "columnWidth": 1,
  "categoryField": "category",
  "categoryAxis": {
	"gridAlpha": 0,
	"position": "left"
  }
} );

//setInterval( randomValue, 2000 );

// set random value
function ChartSetTemperatureValue(id,value) 
{
	if(id == 6)
	if ( sRoom ) 
	{
		sRoom.dataProvider.shift();
		sRoom.dataProvider.push( {"category": "Room Temperature "+value+" °C", Variable: 35,bullet: value} );
		sRoom.validateData();
	}
	if(id == 7)
	if ( sBath ) 
	{
		sBath.dataProvider.shift();
		sBath.dataProvider.push( {"category": "Bathroom Temperature "+value+" °C", Variable: 35,bullet: value} );
		sBath.validateData();
	}
	
}
