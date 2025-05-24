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
  String esp32Ip = '10.108.134.140'; // Default ESP32 IP
  final TextEditingController ipController = TextEditingController();

  void sendJoystickData(double x, double y) async {
    final url = Uri.parse('http://$esp32Ip/joystick?x=$x&y=$y');
    try {
      await http.get(url);
      print('Sent joystick data: x=$x, y=$y');
    } catch (e) {
      print('Error sending joystick data: $e');
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

                // Show a SnackBar confirmation
                ScaffoldMessenger.of(context).showSnackBar(
                  SnackBar(
                    content: Text('ESP32 IP updated to $esp32Ip'),
                    duration: Duration(seconds: 2),
                  ),
                );
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
                    sendJoystickData(0,0);
                    return;
                  }
 
                  // Send joystick data to ESP32
                  sendJoystickData(x, y);
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


