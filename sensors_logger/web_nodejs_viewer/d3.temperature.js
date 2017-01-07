var w = window.innerWidth,
    h = window.innerHeight;

d3.select("body");

var svgo = d3.select("body").append("svg")
	.style("background-color", "#dae0ea")
    .attr("width", w)
    .attr("height", 200)
	.append("g")
    ;

var Temp_Labels = [
	{ id: 0, x: 10, y: 10, r: 20, color: "Blue" },
	{ id: 0, x: 100, y: 10, r: 20, color: "Blue" }
];

svgo.selectAll("g").data(Temp_Labels).enter().append("g")
		.attr('class', 'cTemperature') 
        .append("circle")
        .attr("r", 20)
		.attr('cx', function(d, i) {	return d.x;	})
		.attr('cy', function(d, i) {	return d.y;	})
		.style("fill", "purple")
        .style("stroke", "#000")
        .style("stroke-width", "2px")
;
/*
var indexToVetPos = function(i)
{
    return (h * 0.1 + (h * 0.8) * (i) / 7);
}

svgo.selectAll("g")
	.transition().duration(500)
	.attr("transform", function (d, i) { return "translate(" + indexToVetPos(i) + ",50)"; })
;
*/
console.log(document.body.parentNode.innerHTML);

function d3_SetTemperatureValue(id,Value)
{
	
}
