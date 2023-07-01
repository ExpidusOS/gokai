import 'constants.dart';
import 'platform_interface.dart';

class GokaiContext {
  GokaiContext() : _instance = GokaiPlatform.instance;
  GokaiContext.raw(this._instance);

  final GokaiPlatform _instance;
  bool _hasInitialized = false;

  List<String> serviceNames = [];
  GokaiContextMode mode = GokaiContextMode.invalid;
  late String packageName;
  late String packageDir;
  late String packageConfigDir;
  late String packageDataDir;

  Future<GokaiContext> init() async {
    if (_hasInitialized) {
      throw Exception('GokaiContext has already been initialized');
    }

    serviceNames = await _instance.getServiceNames();
    mode = await _instance.getContextMode();
    packageName = await _instance.getPackageName();
    packageDir = await _instance.getPackageDir();
    packageConfigDir = await _instance.getPackageConfigDir();
    packageDataDir = await _instance.getPackageDataDir();
    _hasInitialized = true;
    return this;
  }
}