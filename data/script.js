var isCreated = false;

document.addEventListener("DOMContentLoaded", function(event) {
	console.log("DOM fully loaded and parsed");
});

var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
				
connection.onopen = function () {
	connection.send('GET_TEMP');
};
			
console.log("connection opened");

connection.onerror = function (error) {
	console.log('WebSocket Error ', error);
};




// get list of radio buttons with name 'size'
var sz = document.forms['demoForm'].elements['res'];

// loop through list
for (var i=0, len=sz.length; i<len; i++) {
    sz[i].onclick = function() {
		console.log("radioVal: " + this.value);
		connection.send("#r" + this.value);
    };
}



function showVal(value) {
	document.getElementById('requestTimeValue').innerHTML = value;
}

function changeVal(value) {
	console.log("changaVal: " + value);
	connection.send("#t" + value);
}

var colorNames = Object.keys(window.chartColors);
var config = {
	type: 'line',
	options: {
		responsive: true,
		
		tooltips: {
			mode: 'index',
			intersect: false,
		},
		hover: {
			mode: 'nearest',
			intersect: true
		},
		scales: {
			xAxes: [{
				display: true,
				scaleLabel: {
					display: true,
					labelString: 'Time'
				}
			}],
			yAxes: [{
				display: true,
				scaleLabel: {
					display: true,
					labelString: 'Temp \u00B0 C'
				}
			}]
		}
	}
};

window.onload = function() {
	var ctx = document.getElementById("canvas").getContext("2d");
	window.myLine = new Chart(ctx, config);
};

/*
document.getElementById('randomizeData').addEventListener('click', function() {
	config.data.datasets.forEach(function(dataset) {
		dataset.data = dataset.data.map(function() {
			return randomScalingFactor();
		});

	});

	window.myLine.update();
});


document.getElementById('addDataset').addEventListener('click', function() {
	var colorName = colorNames[config.data.datasets.length % colorNames.length];
	var newColor = window.chartColors[colorName];
	var newDataset = {
		label: 'Dataset ' + config.data.datasets.length,
		backgroundColor: newColor,
		borderColor: newColor,
		data: [],
		fill: false
	};

	for (var index = 0; index < config.data.labels.length; ++index) {
		newDataset.data.push(randomScalingFactor());
	}

	config.data.datasets.push(newDataset);
	window.myLine.update();
});

document.getElementById('addData').addEventListener('click', function() {
	if (config.data.datasets.length > 0) {
		
		var d = new Date();
		config.data.labels.push(d.getHours() + ":" + d.getMinutes() + ":" + d.getSeconds());

		config.data.datasets.forEach(function(dataset) {
			dataset.data.push(randomScalingFactor());
		});

		window.myLine.update();
	}
});

document.getElementById('removeDataset').addEventListener('click', function() {
	config.data.datasets.splice(0, 1);
	window.myLine.update();
});

document.getElementById('removeData').addEventListener('click', function() {
	config.data.labels.splice(-1, 1); // remove the label first

	config.data.datasets.forEach(function(dataset, datasetIndex) {
		dataset.data.pop();
	});

	window.myLine.update();
});
*/

document.getElementById('start').addEventListener('click', function() {
	console.log("start");
	connection.send("#s1");
});

document.getElementById('stop').addEventListener('click', function() {
	console.log("stop");
	connection.send("#s0");	
});


document.getElementById('clearData').addEventListener('click', function() {
	
	config.data.datasets.forEach(function(dataset, datasetIndex) {
		while(dataset.data.length > 0) {
			dataset.data.pop();
		}
	});
	
	while(config.data.labels.length > 0) {
		config.data.labels.pop();
	}
	
	window.myLine.update();
		
});

connection.onmessage = function(evt) {
	
	console.log("msg rec", evt.data);
	msgArray = evt.data.split(","); // split message by delimiter into a string array
	document.getElementById('batteryVoltage').innerHTML = parseFloat(0.005938 * msgArray[0]).toFixed(2); //(adc*3.355)/565 = voltage
	var numberOfSensors = parseInt(msgArray[1]);
	
	
	var sig = "";
	for(var i=0; i<numberOfSensors; i++) {
		sig += "- Sensor signature";
		sig += i;
		sig += "<br>";
		for(var j=0; j<8; j++){
			var tmp = parseInt(msgArray[j + 2 + numberOfSensors + (i*8)]);
			sig += "0x";
			if(tmp < 16) {
				sig += "0";
			}
			sig += tmp.toString(16) + ", ";
		}
		sig += "<br><br>";
	}
	document.getElementById('sensorsSignatures').innerHTML = sig;
	console.log(sig);
	
	
	if(isCreated) {
		if (config.data.datasets.length > 0) {
			var d = new Date();
			config.data.labels.push(d.getHours() + ":" + d.getMinutes() + ":" + d.getSeconds());
			var i = 2;
			config.data.datasets.forEach(function(dataset) {
				dataset.data.push(msgArray[i]);
				i++;
			});
			
			if(config.data.labels.length > 100) {
				config.data.labels.shift();
				config.data.datasets.forEach(function(dataset, datasetIndex) {
					dataset.data.shift();
				});
			}
			
			window.myLine.update();
		}
	} else {
		for(var i=0; i<numberOfSensors; i++) {
			
			var colorName = colorNames[config.data.datasets.length % colorNames.length];
			var newColor = window.chartColors[colorName];
			var newDataset = {
				label: 'Sensor ' + config.data.datasets.length,
				backgroundColor: newColor,
				borderColor: newColor,
				lineTension: 0,
				data: [],
				fill: false
			};
			config.data.datasets.push(newDataset);
			window.myLine.update();
		}		
		isCreated = true;
	}
};
