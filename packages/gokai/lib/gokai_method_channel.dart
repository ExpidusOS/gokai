import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'gokai_platform_interface.dart';

/// An implementation of [GokaiPlatform] that uses method channels.
class MethodChannelGokai extends GokaiPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('gokai');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
