var myCenter = new google.maps.LatLng(47.06231884334362, 7.6143685170973185);
var map = new google.maps.Map(document.getElementById("googleMap"),mapProp);

function initialize() {
    var mapProp = {
    center: myCenter,
    zoom: 13,
    scrollwheel: false,
    draggable: false,
    mapTypeId: google.maps.MapTypeId.ROADMAP
};

var marker = new google.maps.Marker({
  position: myCenter,
});

marker.setMap(map);
}

google.maps.event.addDomListener(window, 'load', initialize);
