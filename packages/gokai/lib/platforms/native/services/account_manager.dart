import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services/account_manager.dart';

class GokaiNativeAccountManager extends GokaiAccountManager {
  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Services::AccountManager', JSONMethodCodec());

  GokaiNativeAccountManager() : super();
}
