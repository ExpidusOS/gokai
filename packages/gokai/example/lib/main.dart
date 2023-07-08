import 'package:flutter/material.dart';
import 'package:gokai/flutter/engine.dart';
import 'package:gokai/view/window.dart';
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
  GokaiFlutterEngine? _engine;
  List<GokaiFlutterEngine> _engines = [];
  List<GokaiWindow> _windows = [];

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

      final engineManager = ctx.services['EngineManager'] as GokaiEngineManager;
      engineManager.onChange.add(() {
        engineManager.getAll().then((value) => setState(() {
          _engines = value;
        }));
      });

      final engine = await engineManager.getEngine();
      final engines = await engineManager.getAll();

      final windowManager = ctx.services['WindowManager'] as GokaiWindowManager;
      windowManager.onChange.add(() {
        windowManager.getAll().then((value) => setState(() {
          _windows = value;
        }));
      });

      final windows = await windowManager.getAll();

      final accountManager = ctx.services['AccountManager'] as GokaiAccountManager;
      print(await accountManager.getIds());

      setState(() {
        gkContext = ctx;
        _displayNames = displayNames;
        _inputNames = inputNames;
        _engine = engine;
        _engines = engines;
        _windows = windows;
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
              Text(_engine == null ? '' : 'Current engine: ${_engine!.id}'),
              ..._engines.map((engine) => Text('Engine ${engine.id}: ${engine.viewType.name}, ${engine.viewName}')),
              Text('Windows: ${_windows.map((window) => window.id).join(', ')}'),
            ],
          ),
        ),
      ),
    );
  }
}
