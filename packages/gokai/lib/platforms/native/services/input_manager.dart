import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services/input_manager.dart';

class GokaiNativeInputManager extends GokaiInputManager {
  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Services::InputManager', JSONMethodCodec());

  GokaiNativeInputManager() : super() {
    methodChannel.setMethodCallHandler((call) async {
      switch (call.method) {
        case 'changed':
          for (final func in onChange) {
            func();
          }
          break;
      }
    });
  }

  @override
  Future<List<String>> getNames() async
    => (await methodChannel.invokeListMethod<String>('getNames'))!;
}