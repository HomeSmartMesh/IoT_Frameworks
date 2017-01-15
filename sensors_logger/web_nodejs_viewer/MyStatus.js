var w = window.innerWidth,
    h = window.innerHeight;

//d3.select("body");

//var svgo = d3.select("body").append("svg")
var svgo = d3.select("#StatusDisp")
	.style("background-color", "#dae0ea")
    .attr("width", w)
    .attr("height", 200)
    ;
//var coloursTempArray = ["#2c7bb6", "#00a6ca","#00ccbc","#90eb9d","#ffff8c","#f9d057","#f29e2e","#e76818","#d7191c"];
var coloursTempArray = ["#2c7bb6", "#00a6ca","#ffff8c","#e76818","#d7191c"];
var coloursTempRange = d3.range(0, 1, 1.0 / (coloursTempArray.length - 1));////[0.125, 0.25,...,0.875]
coloursTempRange.push(1);//[0.125, 0.25,...,1]

//A color scale
var colorTempScale = d3.scale.linear()
	.domain(coloursTempRange)
	.range(coloursTempArray)
	.interpolate(d3.interpolateHcl);

var TempRange = [10,30];
//Needed to map the values of the dataset to the color scale
var colorTempInterpolate = d3.scale.linear()
	.domain(TempRange)
	.range([0,1]);

function TempToColor(temp)
{
	return colorTempScale(colorTempInterpolate(temp));
}

var temp_rect_min = 100;
var temp_rect_height = 200;
var temp_rect_max = -100;

var TempToPos = d3.scale.linear()
	.domain(TempRange)
	.range([temp_rect_min,temp_rect_max]);

//Append multiple color stops by using D3's data/enter step
//Append a defs (for definition) element to your SVG
var defs = svgo.append("defs");

//Append a linearGradient element to the defs and give it a unique id	
var linearGradient = defs.append("linearGradient")
    .attr("id", "temp-color-gradient");

linearGradient.selectAll("stop") 
    .data( colorTempScale.range() )
    .enter().append("stop")
    .attr("offset", function(d,i) { return i/(colorTempScale.range().length-1); })
    .attr("stop-color", function(d) { return d; })
	;
linearGradient	.attr("x1", "0%")
				.attr("y1", "100%")
				.attr("x2", "0%")
				.attr("y2", "0%")
				;

var NodesGroups = svgo.selectAll("g").data(Status.data).enter().append("g");

var TempNodesGroups = NodesGroups.append("g").attr("class","cTemp");
var LightNodesGroups = NodesGroups.append("g").attr("class","cLight");
var HumNodesGroups = NodesGroups.append("g").attr("class","cHum");

NodesGroups.attr	("transform", function(d,i)
										{
											return "translate("	+d.x+","+d.y+")";
										}
					)
				.append("text")	.text(function(d,i){return d.Name;})
								.style("font-size", "16px")
								.style("fill", "#011a4c")
								.attr('x',-30)
								.attr('y',90)
				;

TempNodesGroups.attr	("transform", "translate(-30,0)");
LightNodesGroups.attr	("transform", "translate(40,-40)");
HumNodesGroups.attr	("transform", "translate(40,40)");

TempNodesGroups.append("circle")
				.attr('r',30)
				.attr('cx',0)
				.attr('cy',0)
				.style("fill", "White")
				//.style("stroke", "#000")
				//.style("stroke-width", "2px")
				;
TempNodesGroups.append("rect")
				.attr('x',-50)
				.attr('y',-temp_rect_min)
				.attr('width',20)
				.attr('height',temp_rect_height)
				.style("fill", "url(#temp-color-gradient)")
				//.style("stroke", "#000")
				//.style("stroke-width", "2px")
				;
TempNodesGroups.append("text")
				.text("Temperature")
				.style("font-size", "12px")
				.style("font-weight", "bold")
				.style("fill", "#011a4c")
				.attr('x',-25)
				.attr('y',5)
				;
LightNodesGroups.append("circle")
				.attr('r',30)
				.attr('cx',0)
				.attr('cy',0)
				.style("fill", "White")
				.style("stroke", "#000")
				.style("stroke-width", "2px")
				;
LightNodesGroups.append("text")
				.text("Light")
				.style("font-size", "12px")
				.style("fill", "#011a4c")
				.attr('x',-22)
				.attr('y',5)
				;
HumNodesGroups.append("circle")
				.attr('r',30)
				.attr('cx',0)
				.attr('cy',0)
				.style("fill", "White")
				.style("stroke", "#000")
				.style("stroke-width", "2px")
				;
HumNodesGroups.append("text")
				.text("Hum")
				.style("font-size", "12px")
				.style("fill", "#011a4c")
				.attr('x',-22)
				.attr('y',5)
				;

var TempGroupUpdate = svgo.selectAll("g.cTemp").data(Status.data);
var LightGroupUpdate = svgo.selectAll("g.cLight").data(Status.data);
var HumGroupUpdate = svgo.selectAll("g.cHum").data(Status.data);

function d3_SetTemperatureValue(id,Value)
{
	if((id==6) || (id==7))
	{
		Status.data[nmap[id]].Temperature = Value;
		TempGroupUpdate.select("text")	.text(function(d){return d.Temperature + " °C"})
										.attr("y",function(d){return TempToPos(d.Temperature)+5});
		//TempGroupUpdate.select("circle").style("fill",TempToColor(Value));
		TempGroupUpdate.select("circle")	.style("fill",function(d){return TempToColor(d.Temperature)})
											.attr("cy",function(d){return TempToPos(d.Temperature)});
		//TempGroupUpdate.select("circle").style("fill","Blue");
		//console.log(TempGroupUpdate.select("circle"));
		//TempGroupUpdate.select("circle").style("fill","Blue"));
	}
}

function d3_SetLightValue(id,Value)
{
	if((id==6) || (id==7))
	{
		Status.data[nmap[id]].Light = Value;
		LightGroupUpdate.select("text").text(function(d){return d.Light});
	}
}

function d3_SetHumidityValue(id,Value)
{
	if((id==6) || (id==7))
	{
		Status.data[nmap[id]].Humidity = Value;
		HumGroupUpdate.select("text").text(function(d){return d.Humidity + " %RH"});
	}
}

