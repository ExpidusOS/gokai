import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services/display_manager.dart';
import 'package:gokai/view/display.dart';
import 'package:vector_math/vector_math.dart';

class GokaiNativeDisplayManager extends GokaiDisplayManager {
  static final _singleton = GokaiNativeDisplayManager._();

  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Services::DisplayManager', JSONMethodCodec());

  factory GokaiNativeDisplayManager() => _singleton;

  GokaiNativeDisplayManager._() : super() {
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

  Future<GokaiDisplay> get(String name) async {
    final model = (await methodChannel.invokeMethod<String>('getModel', name))!;
    final isHDR = (await methodChannel.invokeMethod<bool>('isHDR', name))!;
    final physicalSize = (await methodChannel.invokeListMethod<int>('getPhysicalSize', name))!;
    return GokaiDisplay(
      name: name,
      model: model,
      isHDR: isHDR,
      physicalSize: Vector2(physicalSize[0] * 1.0, physicalSize[1] * 1.0),
      getMode: () async {
        final value = (await methodChannel.invokeMapMethod('getMode', name))!;
        final res = value['resolution'] as Map;
        return GokaiDisplayMode(
          resolution: Rect.fromLTRB(
            res['x'], res['y'],
            res['width'], res['height']
          ),
          refresh: value['refresh'],
        );
      },
      getModes: () async {
        final values = (await methodChannel.invokeListMethod('getModes', name))!;
        var list = <GokaiDisplayMode>[];
        for (final value in values) {
          final res = value['resolution'] as Map;

          list.add(GokaiDisplayMode(
            resolution: Rect.fromLTRB(
              res['x'], res['y'],
              res['width'], res['height']
            ),
            refresh: value['refresh'],
          ));
        }
        return list;
      },
    );
  }
}
