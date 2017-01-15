
margin = {top: 30, right: 40, bottom: 30, left: 50};
width = 600 - margin.left - margin.right;
height = 270 - margin.top - margin.bottom;
// Set the ranges
var scale_x = d3.time.scale();
var scale_y = d3.scale.linear();

var timeFormat = d3.time.format('%H:%M:%S');
//var timeFormat = d3.time.format('(%Y-%m-%d %H:%M:%S)');

scale_x.range([0, width]);
scale_y.range([height, 0]);
scale_x.domain([new Date(1484478466000),new Date(1484478470000)]);
scale_y.domain([200,2000]);

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
svg.append("path")
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
