// Express.js ve MySQL2 kütüphanelerini projeye dahil ediyoruz.
const express = require('express'); // Web sunucusu oluşturmak için kullanılır.
const mysql = require('mysql2');  // MySQL veritabanına bağlanmak için kullanılan kütüphane.


const app = express(); // Express uygulamasını başlatıyoruz.
const port = 3000; // Sunucunun çalışacağı port numarası.

// MySQL bağlantısı
const db = mysql.createConnection({ 
  host: '142.93.101.74',
  user: 'root',
  password: 'dilara0234',
  port: 3306,
  database: 'aedes'
});

// MySQL bağlantısını başlat
db.connect(err => {
  if (err) {
    console.error('MySQL connection error:', err);
    return;
  }
  console.log('Connected to MySQL');
});

// GPS verilerini almak için API endpoint
app.get('/gps-data', (req, res) => {
    // Veritabanındaki son 50 GPS kaydını almak için SQL sorgusu hazırlıyoruz.
  const query = 'SELECT * FROM gps_data ORDER BY id DESC LIMIT 50'; // Son 50 kaydı al
  db.query(query, (err, results) => {
    if (err) {
      console.error('Error fetching GPS data:', err);
      res.status(500).send('Database error');
    } else {
      res.json(results);
    }
  });
});
// Marker güncelleme API'si

// Server başlat
app.listen(port, () => {
  console.log(`API server running on http://localhost:${port}`);
});
