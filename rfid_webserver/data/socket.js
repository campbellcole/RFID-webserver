var connection = new WebSocket('ws://' + location.hostname + ":25565/", ['arduino']);
connection.onmessage = function (e) {
    document.getElementById('UID').innerHTML = e.data;
}