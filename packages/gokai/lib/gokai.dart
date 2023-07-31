import 'constants.dart';
import 'platform_interface.dart';
import 'service.dart';
export 'service.dart';

class GokaiContext {
  GokaiContext() : _instance = GokaiPlatform.instance;
  GokaiContext.raw(this._instance);

  final GokaiPlatform _instance;
  bool _hasInitialized = false;

  Map<String, GokaiService> services = {};
  GokaiContextMode mode = GokaiContextMode.invalid;
  late String packageName;
  late String packageDir;
  late String packageConfigDir;
  late String packageDataDir;

  Future<GokaiContext> init() async {
    if (_hasInitialized) {
      throw Exception('GokaiContext has already been initialized');
    }

    final serviceNames = await _instance.getServiceNames();
    for (final serviceName in serviceNames) {
      try {
        services[serviceName] = await _instance.getService(serviceName);
      } catch(ex) {
        // ignore
      }
    }

    mode = await _instance.getMode();
    packageName = await _instance.getPackageName();
    packageDir = await _instance.getPackageDir();
    packageConfigDir = await _instance.getPackageConfigDir();
    packageDataDir = await _instance.getPackageDataDir();
    _hasInitialized = true;
    return this;
  }

  Future<bool> quit() async => _instance.quit();
}
