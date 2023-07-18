import 'package:event/event.dart';
import 'package:gokai/service.dart';
import 'package:gokai/view/display.dart';

class GokaiDisplayManager extends GokaiService {
  GokaiDisplayManager() : super(serviceName: 'DisplayManager');

  Event onChange = Event();

  Future<List<String>> getNames() {
    throw UnimplementedError('getNames() has not been implemented');
  }

  Future<GokaiDisplay> get(String name) {
    throw UnimplementedError('get("$name") has not been implemented');
  }
}