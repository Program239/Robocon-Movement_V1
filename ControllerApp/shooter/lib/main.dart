import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:async';
import 'dart:convert';

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
  String esp32Ip = '192.168.4.1'; // Default ESP32 IP
  final TextEditingController ipController = TextEditingController();
  double sliderValue = 0;
  double netValue = 0;
  int lastX = 0;
  int lastY = 0;
  int _elapsedSeconds = 0;
  bool motorValue = false;
  bool _timerRunning = false;

  Timer? _gameTimer;
  Timer? _cameraTimer;

  @override
  void initState() {
    super.initState();
    _cameraTimer = Timer.periodic(Duration(seconds: 1), (timer) {
      receiveCameraData();
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

  void receiveCameraData() async {
    final url = Uri.parse('http://$esp32Ip/camera');
    try {
      final response = await http.get(url);
      if (response.statusCode == 200) {
        // Example response: "/camera?x=123&y=456"
        final body = response.body;
        final match = RegExp(r'x=(\d+)&y=(\d+)').firstMatch(body);
        if (match != null) {
          setState(() {
            lastX = int.parse(match.group(1)!);
            lastY = int.parse(match.group(2)!);
          });
          print('Received camera coordinates: x=$lastX, y=$lastY');
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

  void stopGameTimer() async {
    final url = Uri.parse('http://$esp32Ip/gameTimer/stop');
    try {
      await http.get(url);
      print('Game timer stopped');
    } catch (e) {
      print('Error stopping game timer: $e');
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
          
          // Main control row: joystick + slider + buttons
          Expanded(
            child: Row(
              children: [ // Add some space on the left
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
                        //Shooter Rotation Buttons
                        Row(
                        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
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
                          mainAxisAlignment: MainAxisAlignment.spaceEvenly,
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
                      ],
                    ),
                  ),
                ),

              ],
            ),
          ),
        ],
      ),
      floatingActionButton: FloatingActionButton(
        child: Icon(Icons.settings),
        onPressed: () {
          Navigator.push(
            context,
            MaterialPageRoute(builder: (context) => ParameterPage(esp32Ip: esp32Ip)),
          );
        },
      ),
    );
  }
}

// New ParameterPage widget
class ParameterPage extends StatefulWidget {
  final String esp32Ip;
  const ParameterPage({super.key, required this.esp32Ip});

  @override
  State<ParameterPage> createState() => _ParameterPageState();
}

class _ParameterPageState extends State<ParameterPage> {
  final TextEditingController heightMinController = TextEditingController();
  final TextEditingController heightMaxController = TextEditingController();
  final TextEditingController widthMinController = TextEditingController();
  final TextEditingController widthMaxController = TextEditingController();
  final TextEditingController xMinController = TextEditingController();
  final TextEditingController xMaxController = TextEditingController();
  final TextEditingController yMinController = TextEditingController();
  final TextEditingController yMaxController = TextEditingController();

  void sendParameters() async {
    final params = {
      'heightMin': heightMinController.text,
      'heightMax': heightMaxController.text,
      'widthMin': widthMinController.text,
      'widthMax': widthMaxController.text,
      'xMin': xMinController.text,
      'xMax': xMaxController.text,
      'yMin': yMinController.text,
      'yMax': yMaxController.text,
    };
    final uri = Uri.http(
      widget.esp32Ip,
      '/set_params',
      params,
    );
    try {
      final response = await http.get(uri);
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Sent! Status: ${response.statusCode}')),
      );
    } catch (e) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Error sending parameters')),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Set Parameters'),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: ListView(
          children: [
            TextField(
              controller: heightMinController,
              decoration: InputDecoration(labelText: 'heightMin'),
              keyboardType: TextInputType.number,
            ),
            TextField(
              controller: heightMaxController,
              decoration: InputDecoration(labelText: 'heightMax'),
              keyboardType: TextInputType.number,
            ),
            TextField(
              controller: widthMinController,
              decoration: InputDecoration(labelText: 'widthMin'),
              keyboardType: TextInputType.number,
            ),
            TextField(
              controller: widthMaxController,
              decoration: InputDecoration(labelText: 'widthMax'),
              keyboardType: TextInputType.number,
            ),
            TextField(
              controller: xMinController,
              decoration: InputDecoration(labelText: 'xMin'),
              keyboardType: TextInputType.number,
            ),
            TextField(
              controller: xMaxController,
              decoration: InputDecoration(labelText: 'xMax'),
              keyboardType: TextInputType.number,
            ),
            TextField(
              controller: yMinController,
              decoration: InputDecoration(labelText: 'yMin'),
              keyboardType: TextInputType.number,
            ),
            TextField(
              controller: yMaxController,
              decoration: InputDecoration(labelText: 'yMax'),
              keyboardType: TextInputType.number,
            ),
            SizedBox(height: 24),
            ElevatedButton(
              onPressed: sendParameters,
              child: Text('Send Parameters'),
            ),
          ],
        ),
      ),
    );
  }
}
