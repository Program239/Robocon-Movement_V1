import 'package:flutter/material.dart';
import 'package:flutter_joystick/flutter_joystick.dart';
import 'package:http/http.dart' as http;

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

  void sendCommand(String command) async {
    final url = Uri.parse('http://$esp32Ip/$command');
    try {
      await http.get(url);
      print('Sent command: $command');
    } catch (e) {
      print('Error sending command: $e');
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Robot Joystick')),
      body: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          // TextField for IP Address
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 16.0),
            child: TextField(
              controller: ipController,
              decoration: InputDecoration(
                labelText: 'Enter ESP32 IP Address',
                border: OutlineInputBorder(),
              ),
              onSubmitted: (value) {
                setState(() {
                  esp32Ip = value; // Update the IP address
                });
                print('IP Address updated to: $esp32Ip');
              },
            ),
          ),
          const SizedBox(height: 20),
          // Joystick
          Expanded(
            child: Center(
              child: Joystick(
                mode: JoystickMode.all,
                listener: (details) {
                  double x = details.x;
                  double y = details.y;

                  // Deadzone
                  if (x.abs() < 0.2 && y.abs() < 0.2) {
                    sendCommand('stop');
                    return;
                  }

                  // Prioritize major direction
                  if (y < -0.5 && x.abs() < 0.5) {
                    sendCommand('up');
                  } else if (y > 0.5 && x.abs() < 0.5) {
                    sendCommand('down');
                  } else if (x < -0.5 && y.abs() < 0.5) {
                    sendCommand('left');
                  } else if (x > 0.5 && y.abs() < 0.5) {
                    sendCommand('right');
                  }
                  // Diagonals (optional):
                  // Top-right, Bottom-left, etc.
                },
              ),
            ),
          ),
        ],
      ),
    );
  }
}


