import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services/account_manager.dart';
import 'package:gokai/user/id.dart';

class GokaiNativeAccountManager extends GokaiAccountManager {
  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Services::AccountManager', JSONMethodCodec());

  GokaiNativeAccountManager() : super() {
    methodChannel.setMethodCallHandler((call) async {
      switch (call.method) {
        case 'changed':
          for (final func in onChange) {
            func();
          }
          break;
      }
    });
  }

  @override
  Future<List<GokaiUserID>> getIds() async
    => ((await methodChannel.invokeListMethod('getIds')) ?? []).map((e) => GokaiUserID.dynamic(e)).toList();
}
