var UI = require('ui');
var Vector2 = require('vector2');

var menu = new UI.Menu({
  sections: [{
    items: [{
      title: 'Name',
    },{
      title: 'name2'
    }//Print out Names Here from Server
    ]
  }]
});

menu.on('select', function(e) {
  console.log('Selected item #' + e.itemIndex + ' of section #' + e.sectionIndex);
  console.log('The item is titled "' + e.item.title + '"');
  // 
});
menu.show();


// Show splash screen while waiting for data
var navWin = new UI.Window();

// Text element to inform user
var text = new UI.Text({
  position: new Vector2(0, 0),
  size: new Vector2(144, 168),
  font:'GOTHIC_28_BOLD',
  color:'black',
  textOverflow:'wrap',
  textAlign:'center',
  backgroundColor:'white'
});

// Add to splashWindow and show
navWin.add(text);


// var locationOptions = {
//   enableHighAccuracy: true, 
//   maximumAge: 10000, 
//   timeout: 10000
// };

// function locationSuccess(pos) {
//   console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);
// }

// function locationError(err) {
//   console.log('location error (' + err.code + '): ' + err.message);
// }

// // Make an asynchronous request
// navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);