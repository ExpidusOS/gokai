import 'package:flutter/material.dart';
import 'package:gokai/flutter/engine.dart';
import 'package:gokai/user/account.dart';
import 'package:gokai/view/window.dart';
import 'package:gokai/gokai.dart';
import 'package:gokai/services.dart';
import 'package:gokai/widgets.dart';

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
  List<GokaiUserAccount> _accounts = [];

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
        windowManager.getViewable().then((value) => setState(() {
          _windows = value;
        }));
      });

      windowManager.onMapped.add((id) {
        windowManager.getViewable().then((value) => setState(() {
          _windows = value;
        }));
      });

      windowManager.onCommit.add((id) {
        windowManager.getViewable().then((value) => setState(() {
          _windows = value;
        }));
      });

      final windows = await windowManager.getViewable();

      final accountManager = ctx.services['AccountManager'] as GokaiAccountManager;
      accountManager.onChange.add(() {
        accountManager.getAll().then((value) => setState(() {
          _accounts = value;
        }));
      });

      final accounts = await accountManager.getAll();

      setState(() {
        gkContext = ctx;
        _displayNames = displayNames;
        _inputNames = inputNames;
        _engine = engine;
        _engines = engines;
        _windows = windows;
        _accounts = accounts;
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
              Text('Accounts: ${_accounts.map((acc) => '${acc.id} ${acc.displayName} (${acc.picture ?? 'No picture'}): ${acc.language}').join(', ')}'),
              Expanded(
                child: ListView(
                  padding: const EdgeInsets.all(8),
                  children: _windows.map(
                    (e) => GokaiWindowView(
                      id: e.id,
                      windowManager: gkContext!.services['WindowManager'] as GokaiWindowManager,
                      decorationBuilder: (context, child, win) => Column(
                        children: [
                          SizedBox(
                            width: win.rect.width,
                            child: Card(
                              child: Row(
                                children: [
                                  Text(win.title ?? 'Untitled Window'),
                                ],
                              ),
                            ),
                          ),
                          child,
                        ],
                      ),
                    ),
                  ).toList(),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
