
margin = {top: 30, right: 40, bottom: 30, left: 50};
width = 600 - margin.left - margin.right;
height = 270 - margin.top - margin.bottom;
// Set the ranges
var scale_x = d3.time.scale();
var scale_y = d3.scale.linear();

var timeFormat = d3.time.format('%H:%M:%S');
var FulltimeFormat = d3.time.format('(%Y-%m-%d %H:%M:%S)');

scale_x.range([0, width]);
scale_y.range([height, 0]);
scale_x.domain(Chart.scale_x_domain);
scale_y.domain(Chart.scale_y_domain);

// Define the axes
var xAxis = d3.svg.axis().scale(scale_x)
    .orient("bottom").ticks(5)
	.tickFormat(timeFormat);

var yAxis = d3.svg.axis().scale(scale_y)
    .orient("left").ticks(5);

// Define the line
var valueline = d3.svg.line()
    .x(function(d) { return scale_x(d.time); })
    .y(function(d) { return scale_y(d.value); });

// Adds the svg canvas
var svg = d3.select(Chart.svgID)
//var svg = d3.select("body").append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
    .append("g")
        .attr("transform", 
              "translate(" + margin.left + "," + margin.top + ")");
			  
	
// Add the valueline path.
var PLine = svg.append("path")
		.attr("class", "line")
		.attr("d", valueline(Chart.data));

// Add the X Axis
svg.append("g")
	.attr("class", "x axis")
	.attr("transform", "translate(0," + height + ")")
	.call(xAxis);

// Add the Y Axis
svg.append("g")
	.attr("class", "y axis")
	.call(yAxis);

svg.append("text")
		.attr("text-anchor", "middle")  // this makes it easy to centre the text as the transform is applied to the anchor
		.attr("transform", "translate("+ (width/2) +","+(height-20)+")")  // centre below axis
		.text("Bathroom Temperature");//could not use Chart.SensorName
		
function d3_SetChartValues(Times,Values)
{
	if(Times.length == Values.length)
	{
		Chart.data = [];
		for(var i = 0; i<Times.length ; i++)
		{
			var datum = {time:Times[i]*1000, value:Values[i]};
			Chart.data.push(datum);
		}
	}
	PLine.attr("d", valueline(Chart.data));	
}
	
//------------------------------------------------------------------------------
