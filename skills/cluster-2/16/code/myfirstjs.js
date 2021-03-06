let SerialPort = require('serialport');			// include the serialport library
let	COM5 =  process.argv[2];						// get the port name from the command line
let WebSocketServer = require('ws').Server;   // include the webSocket library

// configure the webSocket server:
const SERVER_PORT = 8081;                 // port number for the webSocket server
let wss = new WebSocketServer({port: SERVER_PORT}); // the webSocket server
let connections = new Array;            // list of connections to the server

let myPort = new SerialPort(portName, 9600);// open the port
let Readline = SerialPort.parsers.Readline;	// make instance of Readline parser
let parser = new Readline();								// make a new parser to read ASCII lines
myPort.pipe(parser);													// pipe the serial stream to the parser

// these are the definitions for the serial events:
myPort.on('open', showPortOpen);    // called when the serial port opens
myPort.on('close', showPortClose);  // called when the serial port closes
myPort.on('error', showError);   // called when there's an error with the serial port
parser.on('data', readSerialData);  // called when there's new data incoming



var SerialPort = require('serialport');			// include the serialport library
var	portName =  process.argv[2];						// get the port name from the command line
var express = require('express');	          // include the express library
var server = express();					            // create a server using express

var myPort = new SerialPort(portName, 9600);// open the port
var Readline = SerialPort.parsers.Readline;	// make instance of Readline parser
var parser = new Readline();								// make a new parser to read ASCII lines
myPort.pipe(parser);													// pipe the serial stream to the parser

// these are the definitions for the serial events:
myPort.on('open', showPortOpen);    // called when the serial port opens
myPort.on('close', showPortClose);  // called when the serial port closes
myPort.on('error', showError);   // called when there's an error with the serial port
parser.on('data', readSerialData);  // called when there's new data incoming

// configure the server's behavior:
server.use('/',express.static('public')); // serve static files from /public
server.listen(8080);                      // start the server

// these are the functions called when the serial events occur:
function showPortOpen() {
  console.log('port open. Data rate: ' + myPort.baudRate);
}

function readSerialData(data) {
  console.log(data);
}

function showPortClose() {
  console.log('port closed.');
}

function showError(error) {
  console.log('Serial port error: ' + error);
}

// ------------------------ Server function
function sendData(request) {
  // print out the fact that a client HTTP request came in to the server:
  console.log("Got a client request, sending them the data.");
  // respond to the client request with the latest serial string:
  request.respond(serialData);
}

var serialport = require('serialport');

// list serial ports:
serialport.list(function (err, ports) {
  ports.forEach(function(port) {
    console.log(port.comName);
  });
});
// ------------------------ Serial event functions:
// this is called when the serial port is opened:
function showPortOpen() {
  console.log('port open. Data rate: ' + myPort.baudRate);
}

// this is called when new data comes into the serial port:
function readSerialData(data) {
  // if there are webSocket connections, send the serial data
  // to all of them:
  console.log(data);
  if (connections.length > 0) {
    broadcast(data);
  }
}

function showPortClose() {
   console.log('port closed.');
}
// this is called when the serial port has an error:
function showError(error) {
  console.log('Serial port error: ' + error);
}

function sendToSerial(data) {
  console.log("sending to serial: " + data);
  myPort.write(data);
}

// ------------------------ webSocket Server event functions
wss.on('connection', handleConnection);

function handleConnection(client) {
  console.log("New Connection");        // you have a new client
  connections.push(client);             // add this client to the connections array

  client.on('message', sendToSerial);      // when a client sends a message,

  client.on('close', function() {           // when a client closes its connection
    console.log("connection closed");       // print it out
    let position = connections.indexOf(client); // get the client's position in the array
    connections.splice(position, 1);        // and delete it from the array
  });
}
// This function broadcasts messages to all webSocket clients
function broadcast(data) {
  for (c in connections) {     // iterate over the array of connections
    connections[c].send(JSON.stringify(data)); // send the data to each connection
  }}