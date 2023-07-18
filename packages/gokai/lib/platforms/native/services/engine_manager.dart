import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/constants.dart';
import 'package:gokai/flutter/engine.dart';
import 'package:gokai/services/engine_manager.dart';

class GokaiNativeEngineManager extends GokaiEngineManager {
  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Services::EngineManager', JSONMethodCodec());

  GokaiNativeEngineManager() : super() {
    methodChannel.setMethodCallHandler((call) async {
      switch (call.method) {
        case 'changed':
          onChange.broadcast();
          break;
      }
    });
  }

  @override
  Future<String> getEngineId() async
    => (await methodChannel.invokeMethod<String>('getEngineId'))!;

  @override
  Future<List<String>> getIds() async
    => (await methodChannel.invokeListMethod<String>('getIds')) ?? [];

  @override
  Future<GokaiFlutterEngine> get(String id) async {
    final viewTypeRaw = await methodChannel.invokeMethod<String>('getViewType', id);
    final viewType = GokaiFlutterEngineViewType.values.firstWhere((e) => e.name == viewTypeRaw);

    final viewName = (await methodChannel.invokeMethod<String>('getViewName', id))!;
    return GokaiFlutterEngine(
      id: id,
      viewType: viewType,
      viewName: viewName
    );
  }
}