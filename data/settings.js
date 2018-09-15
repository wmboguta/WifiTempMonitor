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

function ValidateIPaddress(ipaddress) {  
	if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ipaddress)) {  
		return (true);
	}  
	return (false);  
}

function myFunction() {
	console.log("Submit");
	var ssid = document.getElementById('ssid');
	var pass = document.getElementById('password');
	var mySqlServerIp = document.getElementById('mySqlServerIp');
	var espIp = document.getElementById('espIp');
	var espGateway = document.getElementById('espGateway');
	var espSubnet = document.getElementById('espSubnet');
	
	var ok = true;
	
	if(!ValidateIPaddress(mySqlServerIp.value)){
		mySqlServerIp.style.boxShadow = "0 0 10px #f44242"; 
		ok = false;
	} else {
		mySqlServerIp.style.boxShadow = "none";
	}
	
	if(!ValidateIPaddress(espIp.value)){
		espIp.style.boxShadow = "0 0 10px #f44242";
		ok = false;
	} else {
		espIp.style.boxShadow = "none";
	}
	
	if(!ValidateIPaddress(espGateway.value)){
		espGateway.style.boxShadow = "0 0 10px #f44242";
		ok = false;
	} else {
		espGateway.style.boxShadow = "none";
	}
	
	if(!ValidateIPaddress(espSubnet.value)){
		espSubnet.style.boxShadow = "0 0 10px #f44242";
		ok = false;
	} else {
		espSubnet.style.boxShadow = "none";
	}
	
	console.log("ssid" + ssid.value);
	console.log("pass" + pass.value);
	console.log("mySqlServerIp" + mySqlServerIp.value);
	console.log("espIp" + espIp.value);
	console.log("espGateway" + espGateway.value);
	console.log("espSubnet" + espSubnet.value);	
	
	if(ok) {
		connection.send("#p" + ssid.value + "," + pass.value + "," + mySqlServerIp.value + "," + espIp.value + "," + espGateway.value + "," + espSubnet.value );
	} else {
		alert("ERROR! You've entered wrong IP somewhere. Nothing will be set.");
	}
}


connection.onmessage = function(evt) {
	console.log("msg rec", evt.data);
	var msgArray = evt.data.split(","); // split message by delimiter into a string array
	document.getElementById("ssid").value = msgArray[0];
	document.getElementById("password").value = msgArray[1];
	document.getElementById('mySqlServerIp').value = msgArray[2];
	document.getElementById('espIp').value = msgArray[3];
	document.getElementById('espGateway').value = msgArray[4];
	document.getElementById('espSubnet').value = msgArray[5];
};


// wait onload, otherwise got connection in use error
window.onload = function() {
	setTimeout(function(){
		connection.send("#g"); //#g code tells esp to send eeprom data
	}, 500);
	
};