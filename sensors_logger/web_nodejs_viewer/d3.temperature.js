var w = window.innerWidth,
    h = window.innerHeight;

d3.select("body");

var svgo = d3.select("body").append("svg")
	.style("background-color", "#dae0ea")
    .attr("width", w)
    .attr("height", 200)
    ;

var Nodes = [
	{ x: 100, y: 100, r: 20, color: "Blue", 
		Temperature : 26, Light : 500
		},
	{ x: 300, y: 100, r: 20, color: "Blue", 
		Temperature : 26, Light : 500}
];

var nmap = {6 : 0,
			7 : 1};
//var coloursTempArray = ["#2c7bb6", "#00a6ca","#00ccbc","#90eb9d","#ffff8c","#f9d057","#f29e2e","#e76818","#d7191c"];
var coloursTempArray = ["#2c7bb6", "#00a6ca","#ffff8c","#e76818","#d7191c"];
var coloursTempRange = d3.range(0, 1, 1.0 / (coloursTempArray.length - 1));////[0.125, 0.25,...,0.875]
coloursTempRange.push(1);//[0.125, 0.25,...,1]

//A color scale
var colorTempScale = d3.scale.linear()
	.domain(coloursTempRange)
	.range(coloursTempArray)
	.interpolate(d3.interpolateHcl);

//Needed to map the values of the dataset to the color scale
var colorTempInterpolate = d3.scale.linear()
	.domain([10,30])
	.range([0,1]);

function TempToColor(temp)
{
	return colorTempScale(colorTempInterpolate(temp));
}

console.log("coloursTempArray:",coloursTempArray);
console.log("coloursTempRange:",coloursTempRange);
console.log("colorTempScale:",colorTempScale);
console.log("colorTempInterpolate:",colorTempInterpolate);

console.log("colorTempScale(0):",colorTempScale(0));
console.log("colorTempScale(0.54):",colorTempScale(0.54));
console.log("colorTempScale(1):",colorTempScale(1));
console.log("colorTempInterpolate(-15):",colorTempInterpolate(-15));
console.log("colorTempInterpolate(12):",colorTempInterpolate(12));
console.log("colorTempInterpolate(35):",colorTempInterpolate(35));
console.log("TempToColor(-15):",TempToColor(-15));
console.log("TempToColor(12):",TempToColor(12));
console.log("TempToColor(35):",TempToColor(35));
//Append multiple color stops by using D3's data/enter step
//Append a defs (for definition) element to your SVG
var defs = svgo.append("defs");

//Append a linearGradient element to the defs and give it a unique id	
var linearGradient = defs.append("linearGradient")
    .attr("id", "linear-gradient");

linearGradient.selectAll("stop") 
    .data( colorTempScale.range() )
    .enter().append("stop")
    .attr("offset", function(d,i) { return i/(colorTempScale.range().length-1); })
    .attr("stop-color", function(d) { return d; });

var NodesGroups = svgo.selectAll("g").data(Nodes).enter().append("g");

var TempNodesGroups = NodesGroups.append("g").attr("class","cTemp");
var LightNodesGroups = NodesGroups.append("g").attr("class","cLight");
var HumNodesGroups = NodesGroups.append("g").attr("class","cHum");

NodesGroups.attr	("transform", function(d,i)
										{
											return "translate("	+d.x+","+d.y+")";
										}
					);
TempNodesGroups.attr	("transform", "translate(-31,-23)");
LightNodesGroups.attr	("transform", "translate(31,-23)");
HumNodesGroups.attr	("transform", "translate(0,30)");

TempNodesGroups.append("circle")
				.attr('r',30)
				.attr('cx',0)
				.attr('cy',0)
				.style("fill", "White")
				.style("stroke", "#000")
				.style("stroke-width", "2px")
				;
TempNodesGroups.append("text")
				.text("Temperature")
				.style("font-size", "12px")
				.style("fill", "#011a4c")
				.attr('x',-22)
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

var TempGroupUpdate = svgo.selectAll("g.cTemp").data(Nodes);
var LightGroupUpdate = svgo.selectAll("g.cLight").data(Nodes);
var HumGroupUpdate = svgo.selectAll("g.cHum").data(Nodes);

function d3_SetTemperatureValue(id,Value)
{
	if((id==6) || (id==7))
	{
		Nodes[nmap[id]].Temperature = Value;
		TempGroupUpdate.select("text").text(function(d){return d.Temperature + " °C"});
		//TempGroupUpdate.select("circle").style("fill",TempToColor(Value));
		TempGroupUpdate.select("circle").style("fill",function(d){return TempToColor(d.Temperature)});
		//TempGroupUpdate.select("circle").style("fill","Blue");
		//console.log(TempGroupUpdate.select("circle"));
		//TempGroupUpdate.select("circle").style("fill","Blue"));
	}
}

function d3_SetLightValue(id,Value)
{
	if((id==6) || (id==7))
	{
		Nodes[nmap[id]].Light = Value;
		LightGroupUpdate.select("text").text(function(d){return d.Light});
	}
}

function d3_SetHumidityValue(id,Value)
{
	if((id==6) || (id==7))
	{
		Nodes[nmap[id]].Humidity = Value;
		HumGroupUpdate.select("text").text(function(d){return d.Humidity + " %RH"});
	}
}
