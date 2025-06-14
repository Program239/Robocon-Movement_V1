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
  String esp32Ip = '192.168.4.102'; // Default ESP32 IP
  final TextEditingController ipController = TextEditingController();
  int rotationValue = 0;
  double directionVal = 0;
  int lastX = 0;
  int lastY = 0;
  int _elapsedSeconds = 0;
  bool _timerRunning = false;

  Timer? _gameTimer;

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

  void sendGameTime(int val) async {
    final url = Uri.parse('http://$esp32Ip/gameTimer?time=$val');
    try {
      await http.get(url);
      print('Game time sent');
    } catch (e) {
      print('Error sending game time: $e');
    }
  }

  void startGameTimer() {
    if (_gameTimer != null && _gameTimer!.isActive) return; // Prevent multiple timers

    _timerRunning = true;
    _elapsedSeconds = 0;

    _gameTimer = Timer.periodic(Duration(seconds: 1), (timer) {
      if (_timerRunning) {
        setState(() {
          _elapsedSeconds++;
          sendGameTime(_elapsedSeconds);
        });
      }
    });
  }

  void stopGameTimer() {
    if (_gameTimer != null) {
      _elapsedSeconds = 0;
      _gameTimer!.cancel();
      _gameTimer = null;
    
    }
    _timerRunning = false;
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
                  'Basket Position = X: $lastX, Y: $lastY',
                  style: TextStyle(fontSize: 30, fontWeight: FontWeight.bold),
                ),
                SizedBox(height: 8, width: 20),
                Text(
                  'Game Time: ${_elapsedSeconds}s',
                  style: TextStyle(fontSize: 30, fontWeight: FontWeight.bold),
                ),
                SizedBox(width: 20),
                ElevatedButton(
                  onPressed: stopGameTimer, 
                  child: Text('Stop Timer')
                ),
              ],
              ),
          ),

          // Main control row: joystick + buttons
          Expanded(
            child: Row(
              children: [
                // Joystick on left
                SizedBox(width: 20),

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

                SizedBox(width: 50),

                // Rotation control on right
                Expanded(
                  flex: 2,
                  child: Padding(
                    padding: const EdgeInsets.symmetric(vertical: 20, horizontal: 10),
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: [
                        Row(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            // Left rotation button
                            GestureDetector(
                              onTapDown: (_) => sendRotationData(-1),
                              onTapUp: (_) => sendRotationData(0),
                              onTapCancel: () => sendRotationData(0),
                              child: SizedBox(
                                width: 200, // Set width
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
                            SizedBox(width: 20),
                            // Right rotation button
                            GestureDetector(
                              onTapDown: (_) => sendRotationData(1),
                              onTapUp: (_) => sendRotationData(0),
                              onTapCancel: () => sendRotationData(0),
                              child: SizedBox(
                                width: 200,
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
                        const SizedBox(height: 20),
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
