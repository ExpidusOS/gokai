import 'dart:html';
import 'package:gokai/constants.dart';
import 'package:gokai/platforms/web/services.dart';
import 'package:gokai/platform_interface.dart';
import 'package:gokai/service.dart';
import 'package:path/path.dart' as path;

final _kPlatformMap = <String, GokaiService Function()>{
  'AccountManager': () => GokaiWebAccountManager(),
  'DisplayManager': () => GokaiWebDisplayManager(),
  'EngineManager': () => GokaiWebEngineManager(),
  'InputManager': () => GokaiWebInputManager(),
  'PowerManager': () => GokaiWebPowerManager(),
  'StorageManager': () => GokaiWebStorageManager(),
  'WindowManager': () => GokaiWebWindowManager(),
};

class GokaiWebPlatform extends GokaiPlatform {
  @override
  Future<GokaiService> getService(String name) async {
    if (_kPlatformMap.containsKey(name)) return _kPlatformMap[name]!();
    throw Exception('Service $name has not been mapped');
  }

  @override
  Future<List<String>> getServiceNames() async
    => _kPlatformMap.keys.toList();

  @override
  Future<GokaiContextMode> getMode() async
    => GokaiContextMode.client;

  @override
  Future<String> getPackageName() async
    => window.navigator.appName;

  @override
  Future<String> getPackageDir() async
    => '${window.location.origin}${path.dirname(window.location.pathname ?? '/')}';

  @override
  Future<String> getPackageConfigDir() async
    => path.join(await getPackageDir(), 'config');

  @override
  Future<String> getPackageDataDir() async
    => path.join(await getPackageDir(), 'data');

  @override
  Future<bool> quit() async => false;
}

typedef GokaiTargetPlatform = GokaiWebPlatform;
