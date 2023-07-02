import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services/window_manager.dart';
import 'package:gokai/view/window.dart';

class GokaiNativeWindowManager extends GokaiWindowManager {
  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Services::WindowManager', JSONMethodCodec());

  GokaiNativeWindowManager() : super() {
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
  Future<List<String>> getIds() async
    => (await methodChannel.invokeListMethod<String>('getIds')) ?? [];

  @override
  Future<GokaiWindow> get(String id) async {
    return GokaiWindow(
      id: id,
    );
  }
}