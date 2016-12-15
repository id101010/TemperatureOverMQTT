// Globals
var hostname = "147.87.98.17";
var port = 9001;

// Load Google Chart visualisation API
google.charts.load('current', {packages: ['line']});
// Set onload callback
google.charts.setOnLoadCallback(initCharts);

// Create a client instance
var client = new Paho.MQTT.Client(hostname, Number(port), "WebSiteID");
// Set callback handlers
client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;

function initCharts(){
    // Generate data table for temperature values
    var dat_temperature = new google.visualization.DataTable();
    dat_temperature.addColumn('number', 'Time / s');
    dat_temperature.addColumn('number', 'Temperature / °C');

    for(i=0,j=0; i < 63; i+=Math.PI*2/63,j++){
        sine = Math.sin(0.5*i);
        dat_temperature.addRow([i,sine]);
    }

    // Generate data table for acceleration values
    var dat_acceleration = new google.visualization.DataTable();
    dat_acceleration.addColumn('number', 'Time / s');
    dat_acceleration.addColumn('number', 'Force_X / mN');
    dat_acceleration.addColumn('number', 'Force_Y / mN');
    dat_acceleration.addColumn('number', 'Force_Z / mN');
    
    for(i=0,j=0; i < 63; i+=Math.PI*2/63,j++){
        sinea = Math.sin(0.5*i);
        sineb = Math.sin(0.5*i+(120/360)*Math.PI*2);
        sinec = Math.sin(0.5*i+(240/360)*Math.PI*2);
        dat_acceleration.addRow([i,sinea,sineb,sinec]);
    }

    // Generate data table for gyroscope values
    var dat_gyroscope = new google.visualization.DataTable();
    dat_gyroscope.addColumn('number', 'Time / s');
    dat_gyroscope.addColumn('number', 'Force_X / mN');
    dat_gyroscope.addColumn('number', 'Force_Y / mN');
    dat_gyroscope.addColumn('number', 'Force_Z / mN');
    dat_gyroscope.addRow([1,0,0,0]);

    for(i=0,j=0; i < 63; i+=Math.PI*2/63,j++){
        sinea = Math.sin(0.1*i);
        sineb = Math.sin(0.1*i+(120/360)*Math.PI*2);
        sinec = Math.sin(0.1*i+(240/360)*Math.PI*2);
        dat_gyroscope.addRow([i,sinea,sineb,sinec]);
    }
    
    // Temperature chart options
    var opt_temperature = { 'title':'Temperature Data',
                            'width': 1200,
                            'height': 600};

    // Acceleration chart options
    var opt_acceleration = { 'title':'Acceleration Data',
                             'width': 1200,
                             'height': 600};
    
    // Gyroscope chart options
    var opt_gyroscope = { 'title':'Gyroscope Data',
                          'width': 1200,
                          'height': 600};

    // Instantiate and draw our chart, passing in some options.
    var chart_temperature = new google.charts.Line(document.getElementById('google_chart_temp'));
    var chart_acceleration = new google.charts.Line(document.getElementById('google_chart_accel'));
    var chart_gyroscope = new google.charts.Line(document.getElementById('google_chart_gyro'));

    // Draw blank charts
    chart_temperature.draw(dat_temperature, opt_temperature);
    chart_acceleration.draw(dat_acceleration, opt_acceleration);
    chart_gyroscope.draw(dat_gyroscope, opt_gyroscope);
}

// Draw the temperature chart
function updateChartTemperature() {
}

// Draw the accel chart
function updateChartAcceleration() {
}

// Draw the gyro chart
function updateChartGyroscope() {
}

// Called when connection to broker established
function onBrokerConnect() {
    console.log("Connection to broker established.");
    client.subscribe("EmbSy/gruppe_15/#");
    console.log("Subscribed to EmbSby/gruppe_15/#");         
}

// Called on broker connection failure
function onBrokerConnectionFailure() {
    console.log("Connection to broker failed. Something went wrong.");
}

// Called on broker connection loss
function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
        console.log("Connection to broker lost:" + responseObject.errorMessage);
    }
}

// Called when a message arrives
function onMessageArrived(message) {
    console.log("Got Message: " + message.payloadString);

    switch(message.destinationName){
        case "EmbSy/gruppe_15/temp":
            break;
        case "EmbSy/gruppe_15/ax":
            break;
        case "EmbSy/gruppe_15/ay":
            break;
        case "EmbSy/gruppe_15/az":
            break;
        case "EmbSy/gruppe_15/gx":
            break;
        case "EmbSy/gruppe_15/gy":
            break;
        case "EmbSy/gruppe_15/gz":
            break;  
        default:
            console.log("NO_ADD: Message with unknown topic=" + message.destinationName + ", message=" + message.payloadString);
    }
}

// Testmessage for testing communication to, and from the MQTT Broker
function sendTestMessages() {

    if(!client){
        console.log("Client not ready!");
        return;
    }

    var counter = 0;

    for(i = 0; i < 30; i++){
        
        sine = Math.sin(counter) / 2 + 0.5
        counter += Math.PI * 2 / 30;

        message = new Paho.MQTT.Message(sine.toString());
        message.destinationName = "EmbSy/gruppe_15/temp";
        client.send(message);
        console.log("Sending: " + message.payloadString);
    }
}
