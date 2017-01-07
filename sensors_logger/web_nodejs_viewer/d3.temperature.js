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

var NodesGroups = svgo.selectAll("g").data(Nodes).enter().append("g");

var TempNodesGroups = NodesGroups.append("g").attr("class","cTemp");
var LightNodesGroups = NodesGroups.append("g").attr("class","cLight");

NodesGroups.attr	("transform", function(d,i)
										{
											return "translate("	+d.x+","+d.y+")";
										}
					);

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
LightNodesGroups.append("text")
				.text("Light")
				.style("font-size", "12px")
				.style("fill", "#011a4c")
				.attr('x',+22)
				.attr('y',5)
				;
var TempGroupUpdate = svgo.selectAll("g.cTemp").data(Nodes);
var LightGroupUpdate = svgo.selectAll("g.cLight").data(Nodes);

function d3_SetTemperatureValue(id,Value)
{
	if((id==6) || (id==7))
	{
		Nodes[nmap[id]].Temperature = Value;
		//TempTextsUpdate.text(function(d){return d.Temperature + " °C"});
		TempGroupUpdate.select("text").text(function(d){return d.Temperature + " °C"});
	}
}
