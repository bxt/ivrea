// This web server proxies its websocket messages to the serial port.
// It prints all the serial communication for debugging.
// It also serves a client page with some buttons to send messages as
// accepted by an Arduino with the ledPiano.ino sketch running on it.
//
// Start with `npm i && node index.js`.
//
// You'll probably need to configure those:
const path = '/dev/cu.usbserial-1410';
const port = 3001;
const baudRate = 9600;


const SerialPort = require('serialport');
const express = require('express');
const app = express();
const expressWs = require('express-ws')(app);

const serial = new SerialPort(path, { baudRate });

serial.pipe(process.stdout);

app.ws('/piano', function(ws, req) {
  ws.on('message', function(msg) {
    console.log(msg);
    serial.write(msg)
  });
});

app.use(express.static('public'))

app.listen(port, () => {
  console.log(`ledPiano webserver listening at http://localhost:${port}`)
})
