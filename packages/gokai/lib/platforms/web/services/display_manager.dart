import 'dart:html';
import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services/display_manager.dart';
import 'package:gokai/view/display.dart';
import 'package:vector_math/vector_math.dart';

class GokaiWebDisplayManager extends GokaiDisplayManager {
  static final _singleton = GokaiWebDisplayManager._();

  factory GokaiWebDisplayManager() => _singleton;
  GokaiWebDisplayManager._() : super();

  @override
  Future<List<String>> getNames() async
    => [ window.name! ];

  Future<GokaiDisplay> get(String name) async {
    if (name != window.name) throw Exception('Display does not exist');
    return GokaiDisplay(
      name: window.name!,
      model: window.navigator.userAgent,
      isHDR: false,
      physicalSize: Vector2(0, 0),
      getMode: () async {
        return GokaiDisplayMode(
          resolution: Rect.fromLTRB(
            0, 0,
            window.screen!.width! * 1.0, window.screen!.height! * 1.0
          ),
          refresh: 0,
        );
      },
      getModes: () async => [],
    );
  }
}
