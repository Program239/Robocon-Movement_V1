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
  String esp32Ip = '192.168.4.1'; // Default ESP32 IP
  final TextEditingController ipController = TextEditingController();
  double sliderValue = 0;



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

  void sendButton1Hold() async {
    final url = Uri.parse('http://$esp32Ip/right');
    try {
      await http.get(url);
      print('Right sent');
    } catch (e) {
      print('Error sending button1 hold: $e');
    }
  }

  void sendButtonRelease() async {
    final url = Uri.parse('http://$esp32Ip/stop');
    try {
      await http.get(url);
      print('Stop sent');
    } catch (e) {
      print('Error sending button1 release: $e');
    }
  }

  void sendButton2Hold() async {
    final url = Uri.parse('http://$esp32Ip/left');
    try {
      await http.get(url);
      print('Left sent');
    } catch (e) {
      print('Error sending button1 hold: $e');
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Robot Controller')),
      body: Column(
        children: [
          // IP address input on top
          Padding(
            padding: const EdgeInsets.all(12.0),
            child: TextField(
              controller: ipController,
              decoration: InputDecoration(
                labelText: 'Enter ESP32 IP Address',
                border: OutlineInputBorder(),
              ),
              onSubmitted: (value) {
                setState(() {
                  esp32Ip = value;
                });
                print('IP Address updated to: $esp32Ip');

                ScaffoldMessenger.of(context).showSnackBar(
                  SnackBar(
                    content: Text('ESP32 IP updated to $esp32Ip'),
                    duration: Duration(seconds: 2),
                  ),
                );
              },
            ),
          ),

          // Main control row: joystick + slider + buttons
          Expanded(
            child: Row(
              children: [
                // Joystick on left
                Expanded(
                  flex: 3,
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

                        sendJoystickData(x, y);
                      },
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
                        // Two buttons for future functions
                        GestureDetector(
                          onLongPressStart: (_) => sendButton1Hold(),
                          onLongPressEnd: (_) => sendButtonRelease(),
                          child: ElevatedButton(
                            onPressed: sendButtonRelease,
                            child: const Text('Right'),
                          ),
                        ),
                        const SizedBox(height: 20),
                          GestureDetector(
                          onLongPressStart: (_) => sendButton2Hold(),
                          onLongPressEnd: (_) => sendButtonRelease(),
                          child: ElevatedButton(
                            onPressed: sendButtonRelease,
                            child: const Text('Left'),
                          ),
                        ),
                        
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
                        // Vertical slider rotated
                        RotatedBox(
                          quarterTurns: -1,
                          child: Slider(
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
