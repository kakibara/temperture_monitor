# temperture monitor

ESP32と温度湿度計DHT20を用いて気温と湿度を計測してInfluxDBに送信する。

# requirements
## device
- ESP32
- DHT20

## 接続について
I2C通信で接続する。
ESP32はp21がSDA p22がSCLである。
DHT20は3.3v入力で、ESP32はプルアップ抵抗を内蔵しているので自分で実装する必要はない。

## library for arduino IDE
Arduino IDEのライブラリ検索からインストールできる。

- [DHT20](https://github.com/RobTillaart/DHT20) v0.2.0
- [ESP8266 Influxdb](https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino) v 3.12.2
