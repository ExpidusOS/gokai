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
        case 'commit':
          for (final func in onCommit) {
            func(call.arguments);
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
    final rect = (await methodChannel.invokeMapMethod<String, int>('getRect', id))!;
    final title = await methodChannel.invokeMethod<String>('getTitle', id);
    final texture = await methodChannel.invokeMethod<int>('getTexture', id);
    final hasDecorations = (await methodChannel.invokeMethod<bool>('hasDecorations', id))!;
    return GokaiWindow(
      id: id,
      rect: Rect.fromLTWH(rect['x']!.roundToDouble(), rect['y']!.roundToDouble(), rect['width']!.roundToDouble(), rect['height']!.roundToDouble()),
      title: title,
      texture: texture,
      hasDecorations: hasDecorations,
      enter: () => methodChannel.invokeMethod('enter', id),
      leave: () => methodChannel.invokeMethod('leave', id),
      setRect: (rect) => methodChannel.invokeMethod('setRect', {
        'id': id,
        'x': rect.left.round(),
        'y': rect.top.round(),
        'width': rect.width,
        'height': rect.height,
      }),
    );
  }
}