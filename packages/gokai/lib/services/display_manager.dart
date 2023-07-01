import 'dart:ui';
import 'package:gokai/service.dart';
import 'package:gokai/view/display.dart';

class GokaiDisplayManager extends GokaiService {
  GokaiDisplayManager() : super(serviceName: 'DisplayManager');

  List<VoidCallback> onChange = [];

  Future<List<String>> getNames() {
    throw UnimplementedError('getNames() has not been implemented');
  }

  Future<GokaiDisplay> get(String name) {
    throw UnimplementedError('get("$name") has not been implemented');
  }
}