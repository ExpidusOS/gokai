
import 'gokai_platform_interface.dart';

class Gokai {
  Future<String?> getPlatformVersion() {
    return GokaiPlatform.instance.getPlatformVersion();
  }
}
