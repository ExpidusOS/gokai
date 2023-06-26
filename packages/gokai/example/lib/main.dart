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
  static const methodChannel = MethodChannel('Gokai::Services::EngineManager', JSONMethodCodec());
  String _engineId = 'Unknown';

  @override
  void initState() {
    super.initState();
    initEngineId();
  }

  Future<void> initEngineId() async {
    String value = await methodChannel.invokeMethod('getEngineId');
    setState(() {
      _engineId = value;
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Text('Engine ID: $_engineId'),
        ),
      ),
    );
  }
}
