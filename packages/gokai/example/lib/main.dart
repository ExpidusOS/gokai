import 'package:flutter/material.dart';
import 'package:gokai/gokai.dart';
import 'package:gokai/services.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  List<String> _displayNames = [];
  List<String> _inputNames = [];
  GokaiContext? gkContext;

  @override
  void initState() {
    super.initState();

    GokaiContext().init().then((ctx) async {
      final displayManager = ctx.services['DisplayManager'] as GokaiDisplayManager;
      displayManager.onChange.add(() {
        displayManager.getNames().then((value) => setState(() {
          _displayNames = value;
        }));
      });

      final displayNames = await displayManager.getNames();

      final inputManager = ctx.services['InputManager'] as GokaiInputManager;
      inputManager.onChange.add(() {
        inputManager.getNames().then((value) => setState(() {
          _inputNames = value;
        }));
      });

      final inputNames = await inputManager.getNames();

      setState(() {
        gkContext = ctx;
        _displayNames = displayNames;
        _inputNames = inputNames;
      });
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Gokai Example'),
        ),
        body: Center(
          child: Column(
            children: [
              Text('Service names: ${gkContext != null ? gkContext!.services.keys.join(', ') : ''}'),
              Text('Service channels: ${gkContext != null ? gkContext!.services.values.map((e) => e.channelName).join(', ') : ''}'),
              Text('Displays: ${_displayNames.join(', ')}'),
              Text('Inputs: ${_inputNames.join(', ')}'),
            ],
          ),
        ),
      ),
    );
  }
}