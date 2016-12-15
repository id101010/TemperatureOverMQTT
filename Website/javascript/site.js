function onButtonConnectPressed(){
    console.log("Connect button pressed");
    
    client.connect({
        userName: "gruppe_15",
        password: "JfC65L05",
        onSuccess: onBrokerConnect,
        onFailure: onBrokerConnectionFailure
    });
}

function onButtonTestPressed(){
    console.log("Test button pressed, sending testdata.");
    sendTestMessages();
}

function onButtonDisconnectPressed(){
    console.log("Disconnect button pressed");
    if(client){
        client.disconnect();
    }
}

function onButtonRestartPressed(){
    console.log("Restart button pressed");
}
