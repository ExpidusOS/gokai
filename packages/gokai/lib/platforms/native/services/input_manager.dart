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
        case 'lid':
          onLid.broadcast(call.arguments);
          break;
        case 'tablet':
          onTablet.broadcast(call.arguments);
          break;
      }
    });
  }

  @override
  Future<List<String>> getIds() async
    => (await methodChannel.invokeListMethod<String>('getIds')) ?? [];

  @override
  Future<bool> sendPointer(GokaiPointerEvent event) async
    => (await methodChannel.invokeMethod<bool>('sendPointer', event.toJson()))!;

  @override
  Future<bool> sendTouch(GokaiTouchEvent event) async
    => (await methodChannel.invokeMethod<bool>('sendTouch', event.toJson()))!;

  @override
  Future<bool> sendText(String windowId, String text) async
    => (await methodChannel.invokeMethod<bool>('sendText', {
      'windowId': windowId,
      'text': text,
    }))!;

  @override
  Future<bool> sendKeyboard(String windowId, int keycode) async
    => (await methodChannel.invokeMethod<bool>('sendKeyboard', {
      'windowId': windowId,
      'keycode': keycode,
    }))!;
}
