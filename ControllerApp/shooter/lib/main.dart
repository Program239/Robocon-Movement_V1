import 'package:flutter/material.dart';
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
  const JoystickPage({super.key});

  @override
  _JoystickPageState createState() => _JoystickPageState();
}

class _JoystickPageState extends State<JoystickPage> {
  String esp32Ip = '192.168.4.101'; // Default ESP32 IP
  final TextEditingController ipController = TextEditingController();
  double sliderValue = 0;
  double netValue = 0;
  int lastX = 0;
  int lastY = 0;
  bool motorValue = false;
  Timer? _cameraTimer;

  @override
  void initState() {
    super.initState();
    _cameraTimer = Timer.periodic(Duration(seconds: 1), (timer) {
      sendCameraData();
    });
  }

  @override
  void dispose() {
    _cameraTimer?.cancel();
    super.dispose();
  }

  // Send slider (rotation) data
  void sendNetDirection(double val) async {
    final url = Uri.parse('http://$esp32Ip/net?val=$val');
    try {
      await http.get(url);
      print('Sent net data: val=$val');
    } catch (e) {
      print('Error sending net data: $e');
    }
  }

  void sendShooterDirection(double val) async {
    final url = Uri.parse('http://$esp32Ip/shooter?direction=$val');
    try {
      await http.get(url);
      print('Shooter Direction sent');
    } catch (e) {
      print('Error sending shooter direction: $e');
    }
  }

  void sendCameraData() async {
    final url = Uri.parse('http://$esp32Ip/camera?x=$lastX&y=$lastY');
    try {
      await http.get(url);
      print('Sent camera coordinates: x=$lastX, y=$lastY');
    } catch (e) {
      print('Error sending camera command: $e');
    }
  }
  
  void sendMotorValue(int val) async {
    final url = Uri.parse('http://$esp32Ip/motor?value=$val');
    try {
      await http.get(url);
      print('Sent motor value: value=$val');
    } catch (e) {
      print('Error sending motor command: $e');
    }
  }

  void sendShootValue() async {
    final url = Uri.parse('http://$esp32Ip/shoot');
    try {
      await http.get(url);
      print('Shoot command sent');
    } catch (e) {
      print('Error sending shoot command: $e');
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body:
      Column(
        children: [
          //Basket Position Display
          Padding(
            padding: const EdgeInsets.all(12.0),
            child: Row(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  'Basket Position = X: $lastX, Y: $lastY',
                  style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
                ),
                SizedBox(height: 8),
              ],
            )
          ),
          
          // Main control row: joystick + slider + buttons
          Expanded(
            child: Row(
              children: [
                const SizedBox(width: 20), // Add some space on the left
                // Buttons on left
                Expanded(
                  child: Padding(
                    padding: const EdgeInsets.symmetric(vertical: 20, horizontal: 10),
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: [
                        // Net Up Button
                        Row(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            GestureDetector(
                              onTapDown: (_) => sendNetDirection(1),
                              onTapUp: (_) => sendNetDirection(0),
                              onTapCancel: () => sendNetDirection(0),
                              child: SizedBox(
                                width: 130, // Set width
                                height: 80, // Set height
                                child: ElevatedButton(
                                  onPressed: null, // Disabled to use GestureDetector
                                  style: ElevatedButton.styleFrom(
                                    minimumSize: Size(80, 50), // Minimum size
                                  ),
                                  child: Text('Up'),
                                ),
                              ),
                            ),
                          ],
                        ),
                        const SizedBox(height: 20),
                        //Shooter Rotation Buttons
                        Row(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          // Left rotation button
                          GestureDetector(
                            onTapDown: (_) => sendShooterDirection(-1),
                            onTapUp: (_) => sendShooterDirection(0),
                            onTapCancel: () => sendShooterDirection(0),
                            child: SizedBox(
                              width: 130, // Set width
                              height: 80, // Set height
                              child: ElevatedButton(
                                onPressed: null, // Disabled to use GestureDetector
                                style: ElevatedButton.styleFrom(
                                  minimumSize: Size(80, 50), // Minimum size
                                ),
                                child: Text('Left'),
                              ),
                            ),
                          ),
                          SizedBox(width: 60),
                          // Right rotation button
                          GestureDetector(
                            onTapDown: (_) => sendShooterDirection(1),
                            onTapUp: (_) => sendShooterDirection(0),
                            onTapCancel: () => sendShooterDirection(0),
                            child: SizedBox(
                              width: 130,
                              height: 80,
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
                      
                      // Net Down Button
                      Row(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            GestureDetector(
                              onTapDown: (_) => sendNetDirection(-1),
                              onTapUp: (_) => sendNetDirection(0),
                              onTapCancel: () => sendNetDirection(0),
                              child: SizedBox(
                                width: 130, // Set width
                                height: 80, // Set height
                                child: ElevatedButton(
                                  onPressed: null, // Disabled to use GestureDetector
                                  style: ElevatedButton.styleFrom(
                                    minimumSize: Size(80, 50), // Minimum size
                                  ),
                                  child: Text('Down'),
                                ),
                              ),
                            ),
                          ],
                        ),
                        const SizedBox(height: 30),
                      ],
                    ),
                  ),
                ),
                
                // Shooter Buttons
                Expanded(
                  flex: 1,
                  child: Padding(
                    padding: const EdgeInsets.symmetric(vertical: 20, horizontal: 10),
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: [
                        Row(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            SizedBox(
                              height: 130,
                              width: 150,
                              child: Column(
                                mainAxisAlignment: MainAxisAlignment.center,
                                children: [
                                  Text('Motor on/off'),
                                  Switch(
                                    value: motorValue,
                                    onChanged: (value) {
                                      setState(() {
                                        motorValue = value;
                                      });
                                      if (motorValue) {
                                        sendMotorValue(1);
                                      } else {
                                        sendMotorValue(0);
                                      }
                                    },
                                  ),
                                ],
                              ),
                            ),
                            
                            const SizedBox(width: 20),
                            SizedBox(
                              width: 130, // Set width
                              height: 80, // Set height
                              child: ElevatedButton(
                                onPressed: sendShootValue,
                                child: const Text('Shoot'),
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
