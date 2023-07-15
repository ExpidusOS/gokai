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
        case 'mapped':
          for (final func in onMapped) {
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
    final role = await methodChannel.invokeMethod<String>('getRole', id);
    final isToplevel = (await methodChannel.invokeMethod<bool>('isToplevel', id))!;
    final isMapped = (await methodChannel.invokeMethod<bool>('isMapped', id))!;
    final hasDecorations = (await methodChannel.invokeMethod<bool>('hasDecorations', id))!;
    final childrenAbove = await methodChannel.invokeListMethod<String>('getChildrenAbove', id) ?? [];
    final childrenBelow = await methodChannel.invokeListMethod<String>('getChildrenBelow', id) ?? [];
    return GokaiWindow(
      id: id,
      rect: Rect.fromLTWH(rect['x']!.roundToDouble(), rect['y']!.roundToDouble(), rect['width']!.roundToDouble(), rect['height']!.roundToDouble()),
      title: title,
      texture: texture,
      role: role,
      isToplevel: isToplevel,
      isMapped: isMapped,
      hasDecorations: hasDecorations,
      childrenAbove: childrenAbove,
      childrenBelow: childrenBelow,
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