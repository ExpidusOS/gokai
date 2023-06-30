import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:gokai/gokai.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  static const displayManager = MethodChannel('Gokai::Services::DisplayManager', JSONMethodCodec());
  static const engineManager = MethodChannel('Gokai::Services::EngineManager', JSONMethodCodec());
  static const inputManager = MethodChannel('Gokai::Services::InputManager', JSONMethodCodec());

  late Future<List<String>?> _displayNamesFuture;
  late Future<List<String>?> _inputNamesFuture;

  @override
  void initState() {
    super.initState();

    _displayNamesFuture = displayManager.invokeListMethod<String>('getNames');
    _inputNamesFuture = inputManager.invokeListMethod<String>('getNames');

    displayManager.setMethodCallHandler((call) async {
      switch (call.method) {
        case "changed":
          setState(() {
            _displayNamesFuture = displayManager.invokeListMethod<String>('getNames');
          });
          break;
      }
    });

    inputManager.setMethodCallHandler((call) async {
      switch (call.method) {
        case "changed":
          setState(() {
            _inputNamesFuture = inputManager.invokeListMethod<String>('getNames');
          });
          break;
      }
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
              FutureBuilder(
                future: engineManager.invokeMethod<String>('getEngineId'),
                builder: (context, snapshot) {
                  if (snapshot.hasData) {
                    return Text('Engine ID: ${snapshot.data!}');
                  }

                  if (snapshot.hasError) {
                    return Text('Failed to get engine ID: ${snapshot.error!.toString()}');
                  }
                  return const Text('Engine ID: Unknown');
                },
              ),
              FutureBuilder(
                future: _displayNamesFuture,
                builder: (context, snapshot) {
                  if (snapshot.hasData) {
                    return Text(
                      'Displays: ${snapshot.data!.join(', ')}'
                    );
                  }

                  if (snapshot.hasError) {
                    return Text('Failed to get displays: ${snapshot.error!.toString()}');
                  }

                  return const Text('');
                },
              ),
              FutureBuilder(
                future: _inputNamesFuture,
                builder: (context, snapshot) {
                  if (snapshot.hasData) {
                    return Text(
                      'Inputs: ${snapshot.data!.join(', ')}'
                    );
                  }

                  if (snapshot.hasError) {
                    return Text('Failed to get inputs: ${snapshot.error!.toString()}');
                  }

                  return const Text('');
                },
              ),
              TextButton(
                onPressed: () {
                  print('Hello, world');
                },
                child: const Text('Test Log'),
              ),
              const TextField(),
            ],
          ),
        ),
      ),
    );
  }
}