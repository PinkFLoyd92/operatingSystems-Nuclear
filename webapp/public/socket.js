var socket = io('http://localhost:9000');
socket.on('barValue', function (data) {
    console.info(data)
});
