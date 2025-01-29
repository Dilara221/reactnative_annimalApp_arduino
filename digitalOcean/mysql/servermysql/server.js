//
var aedes = require('aedes'); // MQTT protokolünü destekleyen Aedes sunucusu kütüphanesi.
var mysql = require('mysql2'); // MySQL veritabanıyla bağlantı kurmak için kullanılan kütüphane.

// MySQL bağlantı ayarları
var db = mysql.createConnection({
  host: '142.93.101.74', // MySQL sunucu adresi
  user: 'root', 
  password: 'dilara0234',
  port: 3306,          
  database: 'aedes'  
});

// MySQL bağlantısını başlat
db.connect(function (err) {
  if (err) {
    console.error('MySQL connection error:', err);
    return;
  }
  console.log('Connected to MySQL');
});


var server = aedes(); //Bu sunucu, MQTT istemcilerinden gelen mesajları alır ve işler kapanıyor

// Bir istemci sunucuya bağlandığında çalışacak olay işleyici.
server.on('client', function (client) {
  // Bağlanan istemcinin kimliğini konsola yazdırıyoruz.
  console.log('Client connected:', client.id);
});
// Bir istemci sunucuya veri (publish) gönderdiğinde çalışacak olay işleyici.
server.on('publish', function (packet, client) {
  if (!client) return;

  let data;   // Gelen veri (payload) JSON formatında olduğu varsayılıyor. Bu veriyi JSON olarak ayrıştırıyoruz.
  try {
    data = JSON.parse(packet.payload.toString()); // JSON ayrıştırma.
  } catch (error) {
    console.error('Invalid JSON format:', packet.payload.toString());
    return;
  }

  // GPS verisini kontrol et
  if (!data.latitude || !data.longitude) {  // Ayrıştırılan veride GPS bilgileri (latitude ve longitude) kontrol ediliyor.
    console.error('Invalid GPS data:', data);
    return;
  }

  // GPS verisini MySQL'e kaydet
  var query = 'INSERT INTO gps_data (topic, deviceID, latitude, longitude) VALUES (?, ?, ?, ?)';
  var values = [packet.topic,  data.deviceID, data.latitude, data.longitude];

  db.query(query, values, function (err) {
    if (err) {
      console.error('Error saving GPS data to MySQL:', err);
    } else {
      console.log('GPS data saved to MySQL:', data);
    }
  });
});

// MQTT sunucusunun hazır olduğunu bildiren bir olay işleyici.
server.on('ready', function () {
  console.log('Aedes MQTT server is up and running');
});

// MQTT sunucusunu dinlet
var mqttServer = require('net').createServer(server.handle);
mqttServer.listen(1883, function () {
  console.log('Aedes MQTT server listening on port 1883');
});
