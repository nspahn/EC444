var http = require('http');
var url = require('url');
var fs = require('fs');
var SerialPort = require('serialport');
var io = require('socket.io').listen(3000);

var serialPort = new SerialPort("COM5", {
    baudRate: 9600,
    parser: new SerialPort.parsers.Readline("\n")
});

io.sockets.on('connection', function(socket){
    socket.on('message', function(msg){
        console.log(msg);
    });

    socket.on('disconnected', function(){
        console.log('disconnected');
    });
});

var clearData = "";
var readData = "";

const {StringStream} = require('scramjet');

serialport.on('open', () => console.log('open'));
serialport.pipe(new StringStream) // pipe the stream to scramjet StringStream
    .lines('\n')                  // split per line
    .each(                        // send message per every line
        data => io.sockets.emit('message',data)
    );

http.createServer(function (req, res) {
  var q = url.parse(req.url, true);
  var filename = "." + q.pathname;
  fs.readFile(filename, function(err, data) {
    if (err) {
      res.writeHead(404, {'Content-Type': 'text/html'});
      return res.end("404 Not Found");
    } 
    res.writeHead(200, {'Content-Type': 'text/html'});
    res.write(data);
    return res.end();
  });
}).listen(8080); 