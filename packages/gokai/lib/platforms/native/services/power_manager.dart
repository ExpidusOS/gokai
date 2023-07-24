import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/devices/power.dart';
import 'package:gokai/services/power_manager.dart';

class GokaiNativePowerManager extends GokaiPowerManager {
  static final _singleton = GokaiNativePowerManager._();

  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Services::PowerManager', JSONMethodCodec());

  factory GokaiNativePowerManager() => _singleton;

  GokaiNativePowerManager._() : super() {
    methodChannel.setMethodCallHandler((call) async {
      switch (call.method) {
        case 'changed':
          onChange.broadcast(call.arguments);
          break;
      }
    });
  }

  @override
  Future<List<String>> getIds() async
    => (await methodChannel.invokeListMethod<String>('getIds')) ?? [];

  @override
  Future<GokaiPowerDevice> get(String id) async {
    final isIntegrated = (await methodChannel.invokeMethod<bool>('isIntegrated', id))!;
    return GokaiPowerDevice(
      id: id,
      getCycleCount: () async => (await methodChannel.invokeMethod<int>('getCycleCount', id))!,
      getLevel: () async => (await methodChannel.invokeMethod<double>('getLevel', id))!,
      getIsCharging: () async => (await methodChannel.invokeMethod<bool>('isCharging', id))!,
      isIntegrated: isIntegrated,
    );
  }
}
