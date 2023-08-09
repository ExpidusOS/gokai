import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/devices/power.dart';
import 'package:gokai/services/power_manager.dart';

class GokaiWebPowerManager extends GokaiPowerManager {
  static final _singleton = GokaiWebPowerManager._();

  factory GokaiWebPowerManager() => _singleton;
  GokaiWebPowerManager._() : super();

  @override
  Future<List<String>> getIds() async
    => [];
}
