

class TimeChart
{
	constructor(CharParams)
	{
		//Take all members of CharParams into this
		for(var param in CharParams)
		{
			this[param] = CharParams[param];
		}
		//add members
		this.data = [];

		var margin = {top: 30, right: 40, bottom: 30, left: 50};
		var width = 600 - margin.left - margin.right;
		var height = 270 - margin.top - margin.bottom;
		// Set the ranges
		var scale_x = d3.time.scale();
		var scale_y = d3.scale.linear();

		var timeFormat = d3.time.format('%H:%M:%S');
		var FulltimeFormat = d3.time.format('(%Y-%m-%d %H:%M:%S)');

		scale_x.range([0, width]);
		scale_y.range([height, 0]);
		scale_x.domain(this.scale_x_domain);
		scale_y.domain(this.scale_y_domain);

		// Define the axes
		var xAxis = d3.svg.axis().scale(scale_x)
			.orient("bottom").ticks(5)
			.tickFormat(timeFormat);

		var yAxis = d3.svg.axis().scale(scale_y)
			.orient("left").ticks(5);

		// Define the line
		this.valueline = d3.svg.line()
			.x(function(d) { return scale_x(d.time); })
			.y(function(d) { return scale_y(d.value); });

		// Adds the svg canvas
		var svg = d3.select(this.svgID)
		//var svg = d3.select("body").append("svg")
				.attr("width", width + margin.left + margin.right)
				.attr("height", height + margin.top + margin.bottom)
			.append("g")
				.attr("transform", 
					"translate(" + margin.left + "," + margin.top + ")");
					
			
		// Add the valueline path.
		this.PLine = svg.append("path")
				.attr("class", "line")
				.attr("d", this.valueline(this.data));

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
	}

	d3_SetChartValues(Times,Values)
	{
		if(Times.length == Values.length)
		{
			this.data = [];
			for(var i = 0; i<Times.length ; i++)
			{
				var datum = {time:Times[i]*1000, value:Values[i]};
				this.data.push(datum);
			}
		}
		this.PLine.attr("d", this.valueline(this.data));	
	}

	getRequestDuration()
	{
		var Request = {
						request : 
						{
							id 		: Math.floor(Math.random() * 10000),
							type : "Duration",
							NodeId 	: this.NodeId,
							SensorName 	: this.SensorName,
							start 		: this.scale_x_domain[0],
							stop 		: this.scale_x_domain[1]
						}
					};
		return Request;
	}
}

//------------------------------------------------------------------------------