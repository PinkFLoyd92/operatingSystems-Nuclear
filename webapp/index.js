var io = require('socket.io')(app);
var http = require('http');
var ip = require('ip');
var bodyParser = require('body-parser')
var express = require('express'),

app = module.exports.app = express();
app.use( bodyParser.json() );       // to support JSON-encoded bodies
app.use(bodyParser.urlencoded({     // to support URL-encoded bodies
  extended: true
}));
var path = require('path');
app.use(express.static(path.join(__dirname, '')));
app.set('views', __dirname + '/views');
app.set('view engine', 'ejs')
app.use(express.static(path.join(__dirname, 'public'))) //static files served by public folder
app.use('/bower_components',  express.static(__dirname + '/bower_components')); //static files served by bower
var server = http.createServer(app);

var io = require('socket.io').listen(server);  //pass a http.Server instance
server.listen(9000);  

app.get('/ip',function (req, res) {
    res.send(ip.address())
})

app.get('/', function(req, res) {
    res.render('home');
});


app.io = io;

app.post('/barValue', function(req, res) {
    console.log(req.body)
    req.app.io.emit('barValue', {resource:req.body});
    res.send('');
});
app.post('/deltak', function(req, res) {
    console.log(req.body)
    req.app.io.emit('deltak', {resource:req.body});
    res.send('');
});

app.post('/kparcial', function(req, res) {
    console.log(req.body)
    req.app.io.emit('kparcial', {resource:req.body});
    res.send('');
});

app.post('/ktotal', function(req, res) {
    console.log(req.body)
    req.app.io.emit('ktotal', {resource:req.body});
    res.send('');
});

app.post('/exit', function(req, res) {
    req.app.io.emit('exit', {});
    res.send('');
});

app.listen(3000);

io.set('origins', '*:*');
io.on('connection', function (socket) {
    console.log("nueva conexion");
});
