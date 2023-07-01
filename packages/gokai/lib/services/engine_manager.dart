import 'package:gokai/service.dart';

class GokaiEngineManager extends GokaiService {
  const GokaiEngineManager() : super(serviceName: 'EngineManager');

  Future<String> getEngineId() {
    throw UnimplementedError('getEngineId() has not been implemented');
  }
}