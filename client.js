var net = require('net');

var client  = new net.Socket();

client.connect({
  host: '0.0.0.0',
  port: 3000
});

client.on('connect', () => console.log('Connected to server'));

client.setEncoding('utf8');

client.on('data',function(data){
  console.log(`SERVER> ${data}`);
});

setTimeout(() => client.end('Disconnected from server'), 5000);