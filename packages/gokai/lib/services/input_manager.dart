import 'dart:ui';
import 'package:gokai/service.dart';

class GokaiInputManager extends GokaiService {
  GokaiInputManager() : super(serviceName: 'InputManager');

  List<VoidCallback> onChange = [];

  Future<List<String>> getNames() {
    throw UnimplementedError('getNames() has not been implemented');
  }
}