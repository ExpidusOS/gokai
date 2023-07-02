import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/constants.dart';
import 'package:gokai/platforms/native/services.dart';
import 'package:gokai/platform_interface.dart';
import 'package:gokai/service.dart';

class GokaiNativePlatform extends GokaiPlatform {
  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Context', JSONMethodCodec());

  @override
  Future<GokaiService> getService(String name) async {
    final channel = (await methodChannel.invokeMethod<String>('getService', name))!;
    final map = {
      'Gokai::Services::DisplayManager': () => GokaiNativeDisplayManager(),
      'Gokai::Services::EngineManager': () => GokaiNativeEngineManager(),
      'Gokai::Services::InputManager': () => GokaiNativeInputManager(),
      'Gokai::Services::WindowManager': () => GokaiNativeWindowManager(),
    };

    if (map.containsKey(channel)) return map[channel]!();
    throw Exception('Service $name on channel $channel has not been mapped');
  }

  @override
  Future<List<String>> getServiceNames() async
    => (await methodChannel.invokeListMethod<String>('getServiceNames'))!;

  @override
  Future<GokaiContextMode> getMode() async {
    final name = (await methodChannel.invokeMethod<String>('getMode'))!;
    return GokaiContextMode.values.firstWhere((e) => e.name == name);
  }

  @override
  Future<String> getPackageName() async
    => (await methodChannel.invokeMethod<String>('getPackageName'))!;

  @override
  Future<String> getPackageDir() async
    => (await methodChannel.invokeMethod<String>('getPackageDir'))!;

  @override
  Future<String> getPackageConfigDir() async
    => (await methodChannel.invokeMethod<String>('getPackageConfigDir'))!;

  @override
  Future<String> getPackageDataDir() async
    => (await methodChannel.invokeMethod<String>('getPackageDataDir'))!;
}