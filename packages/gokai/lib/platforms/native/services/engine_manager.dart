import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/constants.dart';
import 'package:gokai/flutter/engine.dart';
import 'package:gokai/services/engine_manager.dart';

class GokaiNativeEngineManager extends GokaiEngineManager {
  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Services::EngineManager', JSONMethodCodec());

  const GokaiNativeEngineManager() : super();

  @override
  Future<String> getEngineId() async
    => (await methodChannel.invokeMethod<String>('getEngineId'))!;

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