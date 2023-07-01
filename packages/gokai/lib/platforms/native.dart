import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/constants.dart';
import 'package:gokai/platform_interface.dart';

class GokaiNativePlatform extends GokaiPlatform {
  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Context');

  @override
  Future<List<String>> getServiceNames() async
    => (await methodChannel.invokeListMethod<String>('getServiceNames'))!;

  @override
  Future<GokaiContextMode> getContextMode() async {
    final name = (await methodChannel.invokeMethod<String>('getContextMode'))!;
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