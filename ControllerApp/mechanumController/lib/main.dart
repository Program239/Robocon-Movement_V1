import 'package:flutter/material.dart';
import 'package:flutter_joystick/flutter_joystick.dart';
import 'package:http/http.dart' as http;
import 'dart:async';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: JoystickPage(),
    );
  }
}

class JoystickPage extends StatefulWidget {
  JoystickPage({super.key});

  @override
  _JoystickPageState createState() => _JoystickPageState();
}

class _JoystickPageState extends State<JoystickPage> {
  String esp32Ip = '192.168.4.103'; // Default ESP32 IP
  String esp32MasterIp = '192.168.4.1'; // Default ESP32 Master IP
  final TextEditingController ipController = TextEditingController();
  int rotationValue = 0;
  double directionVal = 0;
  int lastX = 0;
  int lastY = 0;
  int lastHeight = 0;
  int lastWidth = 0;
  int _elapsedSeconds = 0;

  Timer? _gameTimer;
  Timer? _cameraTimer;

  @override
  void initState() {
    super.initState();
    _cameraTimer = Timer.periodic(Duration(milliseconds: 500), (timer) {
      receiveCameraData();
    });
    _gameTimer = Timer.periodic(Duration(seconds: 1), (timer) {
      receiveTimerData();
    });
  }

  @override
  void dispose() {
    _cameraTimer?.cancel();
    _gameTimer?.cancel();
    super.dispose();
  }

  // Send joystick x,y data
  void sendJoystickData(double x, double y) async {
    final url = Uri.parse('http://$esp32Ip/joystick?x=$x&y=$y');
    try {
      await http.get(url);
      print('Sent joystick data: x=$x, y=$y');
    } catch (e) {
      print('Error sending joystick data: $e');
    }
  }

  // Send slider (rotation) data
  void sendRotationData(int val) async {
    final url = Uri.parse('http://$esp32Ip/rotation?val=$val');
    try {
      await http.get(url);
      print('Sent rotation data: val=$val');
    } catch (e) {
      print('Error sending rotation data: $e');
    }
  }

  void sendShootData() async {
    final url = Uri.parse('http://$esp32Ip/shoot');
    try {
      await http.get(url);
      print('Shoot command sent');
    } catch (e) {
      print('Error sending shoot command: $e');
    }
  }

  void startGameTimer() async{
    final url = Uri.parse('http://$esp32MasterIp/gameTimer/start');
    try {
      await http.get(url);
      print('Game time sent');
    } catch (e) {
      print('Error sending game time: $e');
    }
  }

  void receiveTimerData() async {
    final url = Uri.parse('http://$esp32MasterIp/gameTimer');
    try {
      final response = await http.get(url);
      if (response.statusCode == 200) {
        // Example response: "/camera?x=123&y=456&width=789&height=101112"
        final body = response.body;
        final match = RegExp(r'time=(\d+)').firstMatch(body);
        if (match != null) {
          setState(() {
            _elapsedSeconds = int.parse(match.group(1)!);
          });
          print('Received game timer: $_elapsedSeconds');
        } else {
          print('Could not parse game timer data: $body');
        }
      } else {
        print('Failed to get game timer data: ${response.statusCode}');
      }
    } catch (e) {
      print('Error reading game timer data: $e');
    }
  }

  void receiveCameraData() async {
    final url = Uri.parse('http://$esp32MasterIp/camera');
    try {
      final response = await http.get(url);
      if (response.statusCode == 200) {
        // Example response: "/camera?x=123&y=456&width=789&height=101112"
        final body = response.body;
        final match = RegExp(r'x=(\d+)&y=(\d+)&width=(\d+)&height=(\d+)').firstMatch(body);
        if (match != null) {
          setState(() {
            lastX = int.parse(match.group(1)!);
            lastY = int.parse(match.group(2)!);
            lastWidth = int.parse(match.group(3)!);
            lastHeight = int.parse(match.group(4)!);
          });
          print('Received camera coordinates: x=$lastX, y=$lastY, width=$lastWidth, height=$lastHeight');
        } else {
          print('Could not parse camera data: $body');
        }
      } else {
        print('Failed to get camera data: ${response.statusCode}');
      }
    } catch (e) {
      print('Error reading camera coordinates: $e');
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Column(
        children: [
          Padding(
            padding: const EdgeInsets.all(12.0),
            child: Row(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  'Basket Position = X: $lastX, Y: $lastY, Width: $lastWidth, Height: $lastHeight',
                  style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
                ),
                SizedBox(height: 8, width: 20),
                Text(
                  'Game Time: ${_elapsedSeconds}s',
                  style: TextStyle(fontSize: 30, fontWeight: FontWeight.bold),
                ),
              ],
              ),
          ),

          // Main control row: joystick + buttons
          Expanded(
            child: Row(
              children: [
                Expanded(
                  flex: 1,
                  child: Center(
                    child: Joystick(
                      mode: JoystickMode.all,
                      listener: (details) {
                        double x = details.x;
                        double y = details.y;

                        // Deadzone
                        if (x.abs() < 0.2 && y.abs() < 0.2) {
                          sendJoystickData(0, 0);
                          return;
                        }
                        startGameTimer(); // Start timer when joystick is used
                        sendJoystickData(x, y);
                      },
                    ),
                  ),
                ),

                // Rotation control on right
                Expanded(
                  flex: 1,
                  child: Padding(
                    padding: const EdgeInsets.symmetric(vertical: 20),
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                      children: [
                        Row(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            // Left rotation button
                            GestureDetector(
                              onTapDown: (_) => sendShootData(),
                              child: SizedBox(
                                width: 150, // Set width
                                height: 100, // Set height
                                child: ElevatedButton(
                                  onPressed: null, // Disabled to use GestureDetector
                                  style: ElevatedButton.styleFrom(
                                    minimumSize: Size(80, 50), // Minimum size
                                  ),
                                  child: Text('Shoot'),
                                ),
                              ),
                            ),
                            // Right rotation button
                          ],
                        ),
                        
                        Row(
                          mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                          children: [
                            // Left rotation button
                            GestureDetector(
                              onTapDown: (_) => sendRotationData(-1),
                              onTapUp: (_) => sendRotationData(0),
                              onTapCancel: () => sendRotationData(0),
                              child: SizedBox(
                                width: 150, // Set width
                                height: 100, // Set height
                                child: ElevatedButton(
                                  onPressed: null, // Disabled to use GestureDetector
                                  style: ElevatedButton.styleFrom(
                                    minimumSize: Size(80, 50), // Minimum size
                                  ),
                                  child: Text('Left'),
                                ),
                              ),
                            ),
                            // Right rotation button
                            GestureDetector(
                              onTapDown: (_) => sendRotationData(1),
                              onTapUp: (_) => sendRotationData(0),
                              onTapCancel: () => sendRotationData(0),
                              child: SizedBox(
                                width: 150,
                                height: 100,
                                child: ElevatedButton(
                                  onPressed: null,
                                  style: ElevatedButton.styleFrom(
                                    minimumSize: Size(80, 50),
                                  ),
                                  child: Text('Right'),
                                ),
                              ),
                            ),
                          ],
                        ),
                      ],   
                    ),
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}
