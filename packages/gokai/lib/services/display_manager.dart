import 'dart:ui';
import 'package:gokai/service.dart';

class GokaiDisplayManager extends GokaiService {
  GokaiDisplayManager() : super(serviceName: 'DisplayManager');

  List<VoidCallback> onChange = [];

  Future<List<String>> getNames() {
    throw UnimplementedError('getNames() has not been implemented');
  }
}