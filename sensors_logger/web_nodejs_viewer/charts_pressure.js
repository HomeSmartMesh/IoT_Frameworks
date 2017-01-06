
var gRoom = AmCharts.makeChart("chartdivRoom",
	{
		"type": "gauge",
		"autoMargins": false,
		"marginBottom": 10,
		"marginTop": 20,
		"startDuration": 0,
		"fontSize": 13,
		"theme": "dark",
		"arrows": [
			{
				//"id": "GaugeArrow-1",
				//"value": 950
			}
		],
		"axes": [
			{
				"axisThickness": 1,
				"bottomText": "Room - Pressure hPa",
				"bottomTextYOffset": 20,
				"startValue": 850,
				"endValue": 1100,
				"id": "GaugeAxis-1",
				"valueInterval": 50,
				//http://www.theweatherprediction.com/habyhints2/410/
				"bands": [
					{
						"startValue": 850,
						"endValue": 950,
						"id": "Hurricane-CAT3",
						"alpha": 0.7,
						"color": "#ea3838"
					},
					{
						"startValue": 950,
						"endValue": 980,
						"id": "Hurricane-CAT1",
						"alpha": 0.7,
						"color": "#ffac29"
					},
					{
						"startValue": 980,
						"endValue": 1020,
						"id": "TypicalLowToSeaLevel",
						"alpha": 0.7,
						"color": "#00CC00"
					},
					{
						"startValue": 1020,
						"endValue": 1040,
						"id": "TypicalLowToSeaLevel",
						"alpha": 0.7,
						"color": "#ffac29"
					},
					{
						"startValue": 1040,
						"endValue": 1100,
						"id": "Highest-Ever",
						"alpha": 0.7,
						"color": "#ea3838"
					}
				]
			}
		],
		"allLabels": [],
		"balloon": {},
		"titles": [],
		"export": {
			"enabled": true
		}
		
	}
);


var gBath = AmCharts.makeChart("chartdivBath",
	{
		"type": "gauge",
		"marginBottom": 20,
		"marginTop": 40,
		"startDuration": 0,
		"fontSize": 13,
		"theme": "dark",
		"arrows": [
			{
				//"id": "GaugeArrow-1",
				//"value": 950
			}
		],
		"axes": [
			{
				"axisThickness": 1,
				"bottomText": "Room - Pressure hPa",
				"bottomTextYOffset": 20,
				"startValue": 850,
				"endValue": 1100,
				"id": "GaugeAxis-1",
				"valueInterval": 50,
				//http://www.theweatherprediction.com/habyhints2/410/
				"bands": [
					{
						"startValue": 850,
						"endValue": 950,
						"id": "Hurricane-CAT3",
						"alpha": 0.7,
						"color": "#ea3838"
					},
					{
						"startValue": 950,
						"endValue": 980,
						"id": "Hurricane-CAT1",
						"alpha": 0.7,
						"color": "#ffac29"
					},
					{
						"startValue": 980,
						"endValue": 1020,
						"id": "TypicalLowToSeaLevel",
						"alpha": 0.7,
						"color": "#00CC00"
					},
					{
						"startValue": 1020,
						"endValue": 1040,
						"id": "TypicalLowToSeaLevel",
						"alpha": 0.7,
						"color": "#ffac29"
					},
					{
						"startValue": 1040,
						"endValue": 1100,
						"id": "Highest-Ever",
						"alpha": 0.7,
						"color": "#ea3838"
					}
				]
			}
		],
		"allLabels": [],
		"balloon": {},
		"titles": [],
		"export": {
			"enabled": true
		}
		
	}
);

function ChartSetPressureValue(id,value) 
{
	if(id == 6)
	if ( gRoom ) 
	{
		if ( gRoom.arrows ) 
		{
		  if ( gRoom.arrows[ 0 ] ) 
		  {
			if ( gRoom.arrows[ 0 ].setValue ) 
			{
			  gRoom.arrows[ 0 ].setValue( value );
			  gRoom.axes[ 0 ].setBottomText( "Room: " + value + " hPa" );
			}
		  }
		}
	}
	if(id == 7)
	if ( gBath ) 
	{
		if ( gBath.arrows ) 
		{
		  if ( gBath.arrows[ 0 ] ) 
		  {
			if ( gBath.arrows[ 0 ].setValue ) 
			{
			  gBath.arrows[ 0 ].setValue( value );
			  gBath.axes[ 0 ].setBottomText( "Bath: " + value + " hPa" );
			}
		  }
		}
	}
	
}
