import 'package:plugin_platform_interface/plugin_platform_interface.dart';
import 'package:gokai/platforms/native.dart';
import 'package:gokai/constants.dart';

abstract class GokaiPlatform extends PlatformInterface {
  GokaiPlatform() : super(token: _token);

  static final Object _token = Object();
  static GokaiPlatform _instance = GokaiNativePlatform();
  static GokaiPlatform get instance => _instance;

  static set instance(GokaiPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<List<String>> getServiceNames() {
    throw UnimplementedError('getServiceNames() has not been implemented.');
  }

  Future<GokaiContextMode> getContextMode() {
    throw UnimplementedError('getContextMode() has not been implemented.');
  }

  Future<String> getPackageName() {
    throw UnimplementedError('getPackageName() has not been implemented.');
  }

  Future<String> getPackageDir() {
    throw UnimplementedError('getPackageDir() has not been implemented.');
  }

  Future<String> getPackageConfigDir() {
    throw UnimplementedError('getPackageConfigDir() has not been implemented.');
  }

  Future<String> getPackageDataDir() {
    throw UnimplementedError('getPackageDataDir() has not been implemented.');
  }
}