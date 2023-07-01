import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services/display_manager.dart';

class GokaiNativeDisplayManager extends GokaiDisplayManager {
  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Services::DisplayManager', JSONMethodCodec());

  GokaiNativeDisplayManager() : super() {
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