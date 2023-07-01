import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services/engine_manager.dart';

class GokaiNativeEngineManager extends GokaiEngineManager {
  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Services::EngineManager', JSONMethodCodec());

  const GokaiNativeEngineManager() : super();

  @override
  Future<String> getEngineId() async
    => (await methodChannel.invokeMethod<String>('getEngineId'))!;
}