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
  JoystickPage({super.key});

  @override
  _JoystickPageState createState() => _JoystickPageState();
}

class _JoystickPageState extends State<JoystickPage> {
  String esp32Ip = '192.168.4.1'; // Default ESP32 IP
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
  void sendSliderData(double val) async {
    final url = Uri.parse('http://$esp32Ip/rotation?val=$val');
    try {
      await http.get(url);
      print('Sent rotation data: val=$val');
    } catch (e) {
      print('Error sending rotation data: $e');
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
      body: Column(
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
                // Slider on left
                Expanded(
                  child: RotatedBox(
                    quarterTurns: -1,
                    child: Padding(
                      padding: const EdgeInsets.symmetric(vertical: 20, horizontal: 10),
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Slider(
                            value: netValue,
                            min: -1,
                            max: 1,
                            divisions: 100,
                            label: netValue.toStringAsFixed(2),
                            onChanged: (val) {
                              setState(() {
                                netValue = val;
                              });
                              sendSliderData(netValue);
                            },
                            onChangeEnd: (val) {
                              setState(() {
                                netValue = 0;
                              });
                              sendSliderData(0);
                            },
                          ),
                          const SizedBox(height: 19),
                        ],
                      ),
                    ),
                  ),
                ),
                
                // Buttons in the middle
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
                            const SizedBox(height: 20),
                            ElevatedButton(
                              onPressed: sendShootValue,
                              child: const Text('Shoot'),
                            ),
                          ],
                        ),
                        const SizedBox(height: 20),
                      ],
                    ),
                  ),
                ),

                // Slider on right
                Expanded(
                  flex: 2,
                  child: Padding(
                    padding: const EdgeInsets.symmetric(vertical: 20, horizontal: 10),
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: [ 
                        Slider(
                          value: sliderValue,
                          min: -1,
                          max: 1,
                          divisions: 100,
                          label: sliderValue.toStringAsFixed(2),
                          onChanged: (val) {
                            setState(() {
                              sliderValue = val;
                            });
                            sendSliderData(sliderValue);
                          },
                          onChangeEnd: (val) {
                            // Send final value when slider is released
                            setState(() {
                              sliderValue = 0; // Reset slider after sending         
                            });
                            sendSliderData(0);
                          },
                        ),
                        const SizedBox(height: 19),
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
