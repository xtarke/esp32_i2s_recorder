# I2S Standard Microphone Recording with UDP Streaming Project

In this example, we record a sample audio file captured from two digital microphone son the I2S peripheral. This code initializes the I2S channel in Standard mode (i2s_std.h). The audio is recorded using the WAVE file format. Additionally, this project includes headers for Wi-Fi station (wifi_sta.h) and UDP client (udp_client.h) capabilities, allowing the audio data to be streamed over a network.

## How to use this project

### Hardware Required 

- A development board with ESP32 or ESP32S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.).
- A USB cable for power supply and programming.
- Two digital I2S microphones (Standard I2S output) 

The digital microphone is connected to the I2S interface, utilizing the i2s_channel_init_std_mode configuration. The digital microphones are connected on the I2S interface `I2S_NUM_0`.

The default GPIO configuration is the following:

|Mic             |  GPIO  |
|:--------------:|:------:|
| I2S_CLK_GPIO   | GPIO04 |
| I2S_DATA_GPIO  | GPIO15 |
| I2S_WS_GPIO    | GPIO02 |

The microphones conection is the following:

|Mic_1   |  PINS  |
|:------:|:------:|
| SD     | I2S_DATA_GPIO |
| VCC    | 3V3 |
| GND    | GND |
| L/R    | GND |
| WS     | I2S_WS_GPIO |
| SCK    | I2S_CLK_GPIO|

|Mic_2   |  PINS  |
|:------:|:------:|
| SD     | I2S_DATA_GPIO |
| VCC    | 3V3 |
| GND    | GND |
| L/R    | 3V3 |
| WS     | I2S_WS_GPIO |
| SCK    | I2S_CLK_GPIO|

**Make sure one mic has 3V3 in the L/R signal and the other has GND.**


## Configure the Project

In the Configuration menu (idf.py menuconfig):

- Use I2S Configuration to assign the I2S peripheral GPIOs, Sample Rate, and Bit Sample sizes. **Tested only with defulat configurations**.

- Use WiFi Configuration to configure your Wi-Fi credentials.

- Use UDP Server Configuration to change UDP target IP/Port for the network streaming features included in the project.

```bash
idf.py build flash monitor -p SERIAL_PORT
```

(To exit the serial monitor, type Ctrl-].) See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

* [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)

### Project output 

Running this project, you will see follwing outputs logs:

#### ESP32 monitor

```
(...)
I (12938) esp_netif_handlers: sta ip: 192.168.0.136, mask: 255.255.255.0, gw: 192.168.0.1
I (12938) wifi station: got ip:192.168.0.136
I (12938) wifi station: connected to ap SSID:------- password: --------
I (12948) example: Socket created, sending to 192.168.0.132:3333
I (12948) main: Starting recording for 30 seconds!
I (12948) main: Opening file
udp_wr_size: 32768 btes read: 32768
udp_wr_size: 65536 btes read: 32768
udp_wr_size: 98304 btes read: 32768
udp_wr_size: 131072 btes read: 32768
udp_wr_size: 163840 btes read: 32768
udp_wr_size: 196608 btes read: 32768
udp_wr_size: 229376 btes read: 32768
udp_wr_size: 262144 btes read: 32768
udp_wr_size: 294912 btes read: 32768
udp_wr_size: 327680 btes read: 32768
(...)
udp_wr_size: 3670016 btes read: 32768
udp_wr_size: 3702784 btes read: 32768
udp_wr_size: 3735552 btes read: 32768
udp_wr_size: 3768320 btes read: 32768
udp_wr_size: 3801088 btes read: 32768
udp_wr_size: 3833856 btes read: 32768
udp_wr_size: 3866624 btes read: 32768
I (43168) main: Recording done!
I (43168) main: File send!. Make sure UDP server is restarted.
I (43168) main_task: Returned from app_main()
```

#### Python UDP server (udp_server_bin.py)

```
UDP server listening on 0.0.0.0:3333...
Ready to record data into 'received_file.wav'...
Received 44 bytes from ('192.168.0.136', 58957) (0)
Received 32768 bytes from ('192.168.0.136', 58957) (1)
Received 32768 bytes from ('192.168.0.136', 58957) (2)
Received 32768 bytes from ('192.168.0.136', 58957) (3)
Received 32768 bytes from ('192.168.0.136', 58957) (4)
Received 32768 bytes from ('192.168.0.136', 58957) (5)
Received 32768 bytes from ('192.168.0.136', 58957) (6)
Received 32768 bytes from ('192.168.0.136', 58957) (7)
(...)
Received 32768 bytes from ('192.168.0.136', 58957) (112)
Received 32768 bytes from ('192.168.0.136', 58957) (113)
Received 32768 bytes from ('192.168.0.136', 58957) (114)
Received 32768 bytes from ('192.168.0.136', 58957) (115)
Received 32768 bytes from ('192.168.0.136', 58957) (116)
Received 32768 bytes from ('192.168.0.136', 58957) (117)
Received 32768 bytes from ('192.168.0.136', 58957) (118)
```

After the reception, a file **received_file.wav** is created. Don't forter to stop the server after the file is received. (To exit the udp server, type Ctrl-C.)

## Troubleshooting

* Program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub.