import 'package:event/event.dart';
import 'package:gokai/flutter/engine.dart';
import 'package:gokai/service.dart';

class GokaiEngineManager extends GokaiService {
  GokaiEngineManager() : super(serviceName: 'EngineManager');

  Event onChange = Event();

  Future<GokaiFlutterEngine> getEngine() async
    => await get(await getEngineId());

  Future<List<GokaiFlutterEngine>> getAll() async {
    final ids = await getIds();
    var engines = <GokaiFlutterEngine>[];
    for (final id in ids) {
      engines.add(await get(id));
    }
    return engines;
  }

  Future<String> getEngineId() {
    throw UnimplementedError('getEngineId() has not been implemented');
  }

  Future<List<String>> getIds() {
    throw UnimplementedError('getIds() has not been implemented');
  }

  Future<GokaiFlutterEngine> get(String id) {
    throw UnimplementedError('get("$id") has not been implemented');
  }
}