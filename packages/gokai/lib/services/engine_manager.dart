import 'package:gokai/flutter/engine.dart';
import 'package:gokai/service.dart';

class GokaiEngineManager extends GokaiService {
  const GokaiEngineManager() : super(serviceName: 'EngineManager');

  Future<GokaiFlutterEngine> getEngine() async =>
    await get(await getEngineId());

  Future<String> getEngineId() {
    throw UnimplementedError('getEngineId() has not been implemented');
  }

  Future<GokaiFlutterEngine> get(String id) {
    throw UnimplementedError('get("$id") has not been implemented');
  }
}