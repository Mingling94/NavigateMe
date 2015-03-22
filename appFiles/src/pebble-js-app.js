var dest =  {
	lon : 42,		// Changer to update from server
	lat : 73
};


Pebble.addEventListener('ready',  function(e) {
    console.log('JavaScript app ready and running!');
  }
);


 // Gets location from Phone
function locationSuccess(pos) {
  // Construct URL
	console.log("coordinated are " + pos.coords.latitude + " " + pos.coords.longitude);

	console.log("Destination Longitude is " + dest.lon);
 
	console.log("Destination Longitude is " + dest.lat);

	//Find direction and distance
	var diffLat = dest.lat - pos.coords.latitude;
	var diffLon = dest.lon - pos.coords.longitude;
	var dist = Math.sqrt((Math.pow(diffLat, 2)) + (Math.pow(diffLon, 2)));
	console.log("Dist is " + dist);

	var angle;

	if(diffLon > 0){
		angle = 2.35619449019 - Math.atan(diffLat/diffLon) ; 
	}
	else{
		angle = Math.atan(diffLat/diffLon) + 0.7853981633;
	}

  console.log("Angle is " + angle);

Pebble.sendAppMessage({
          'KEY_DIST': dist ,
		'KEY_ANG' : angle*360/2/3.14}
        );
}


function locationError(err) {
  console.log("Error requesting location!");
}


function getLocation() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}




// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
 
    // Get the initial weather
    getLocation();
  }
);
 


Pebble.addEventListener('appmessage',
  function(e) {
    console.log('Received message: ' + JSON.stringify(e.payload[1]));
        getLocation();
  }
);
