import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services/input_manager.dart';

class GokaiWebInputManager extends GokaiInputManager {
  static final _singleton = GokaiWebInputManager._();

  factory GokaiWebInputManager() => _singleton;
  GokaiWebInputManager._() : super();

  @override
  Future<List<String>> getIds() async
    => [];

  @override
  Future<bool> sendPointer(GokaiPointerEvent event) async
    => false;

  @override
  Future<bool> sendTouch(GokaiTouchEvent event) async
    => false;

  @override
  Future<bool> sendText(String windowId, String text) async
    => false;

  @override
  Future<bool> sendKeyboard(String windowId, int keycode) async
    => false;
}
