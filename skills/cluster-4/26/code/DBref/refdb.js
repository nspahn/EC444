// Modules
var level = require('level')
var express = require('express');
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
const fs = require('fs');

// Create or open the underlying LevelDB store
var db = level('./mydb', {valueEncoding: 'json'});

// Random number function -- this is a helper function to generate dummy data
function getRndInteger(min, max) {
    return Math.floor(Math.random() * (max - min + 1) ) + min;
}

// Points to index.html to serve webpage
app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
});



// Function to stream from database
function readDB(arg) {
  db.createReadStream()
    .on('data', function (data) {
      console.log(data.key, '=', data.value)
      // Parsed the data into a structure but don't have to ...
      var dataIn = {[data.key]: data.value};
      // Stream data to client
      io.emit('message', dataIn);
    })
    .on('error', function (err) {
      console.log('Oh my!', err)
    })
    .on('close', function () {
      console.log('Stream closed')
    })
    .on('end', function () {
      console.log('Stream ended')
    })
}

// When a new client connects
var clientConnected = 0; // this is just to ensure no new data is recorded during streaming
io.on('connection', function(socket){
  console.log('a user connected');
  clientConnected = 0;

  // Call function to stream database data
  readDB();
  clientConnected = 1;
  socket.on('disconnect', function(){
    console.log('user disconnected');
  });
});

// Listening on localhost:3000
//var time[], ID[], smoke[], temp[];
var time, ID, smoke, temp, flag =0;
http.listen(3000, function() {
  console.log('listening on *:3000');
  // Open the data text file
	fs.readFile(__dirname + '/smoke.txt', 'utf8', (err,data) => {
		if(err) {
			console.error(err);
			return;
		}
		len = data.length;
		var stringtmp = '';
		var counter = 0, loopc = 0;
		// Parsing the line to numbers
		for(i = 0; i < len; i++) {
			if(data[i] != data[4] && data[i] != '\n' && data[i] != ' ' && data[i] != '	')
			{
				stringtmp = stringtmp + data[i];
				flag = 0;
			}
			else {
				number = Number(stringtmp);
				stringtmp = ''; // Reset
				counter++;
				flag = 1;

			}
			// Only after four substrings and when a number is ready
			if(counter > 4 && flag) {
				if(loopc%4 == 0)
					time = number;
				else if(loopc%4 == 1)
					ID = number;
				else if(loopc%4 == 2)
					smoke = number;
				else{
					temp = number;
					time = time + ID; // Mark
					var value = [{id: ID, Smoke: smoke, Temp: temp}];
					console.log(time, value);
					db.put([time],value, function(err) {
						if(err) return console.log('OOPS',err);
					})
					var msg = {[time]:value};
					io.emit('message', msg);
					console.log(Object.keys(msg));
				}
				loopc++;

			}
		}
	})
});