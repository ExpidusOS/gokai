import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services/input_manager.dart';

class GokaiNativeInputManager extends GokaiInputManager {
  static final _singleton = GokaiNativeInputManager._();

  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Services::InputManager', JSONMethodCodec());

  factory GokaiNativeInputManager() => _singleton;

  GokaiNativeInputManager._() : super() {
    methodChannel.setMethodCallHandler((call) async {
      switch (call.method) {
        case 'changed':
          onChange.broadcast();
          break;
      }
    });
  }

  @override
  Future<List<String>> getNames() async
    => (await methodChannel.invokeListMethod<String>('getNames')) ?? [];
}
