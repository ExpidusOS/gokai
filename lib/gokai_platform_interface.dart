import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'gokai_method_channel.dart';

abstract class GokaiPlatform extends PlatformInterface {
  /// Constructs a GokaiPlatform.
  GokaiPlatform() : super(token: _token);

  static final Object _token = Object();

  static GokaiPlatform _instance = MethodChannelGokai();

  /// The default instance of [GokaiPlatform] to use.
  ///
  /// Defaults to [MethodChannelGokai].
  static GokaiPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [GokaiPlatform] when
  /// they register themselves.
  static set instance(GokaiPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
