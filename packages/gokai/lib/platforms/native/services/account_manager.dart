import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services/account_manager.dart';
import 'package:gokai/user/account.dart';
import 'package:gokai/user/id.dart';

class GokaiNativeAccountManager extends GokaiAccountManager {
  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Services::AccountManager', JSONMethodCodec());

  GokaiNativeAccountManager() : super() {
    methodChannel.setMethodCallHandler((call) async {
      switch (call.method) {
        case 'changed':
          onChange.broadcast();
          break;
      }
    });
  }

  @override
  Future<GokaiUserID> getCurrentId() async
    => GokaiUserID.dynamic(await methodChannel.invokeMethod('getCurrentId'));

  @override
  Future<List<GokaiUserID>> getIds() async
    => ((await methodChannel.invokeListMethod('getIds')) ?? []).map((e) => GokaiUserID.dynamic(e)).toList();

  @override
  Future<GokaiUserAccount> get(GokaiUserID id) async {
    final language = (await methodChannel.invokeMethod<String>('getLanguage', id.value))!;
    final displayName = (await methodChannel.invokeMethod<String>('getDisplayName', id.value))!;
    final picture = await methodChannel.invokeMethod<String>('getPicture', id.value);
    final isSystem = (await methodChannel.invokeMethod<bool>('isSystem', id.value))!;
    final isAdministrator = (await methodChannel.invokeMethod<bool>('isAdministrator', id.value))!;

    return GokaiUserAccount(
      id: id,
      language: language,
      displayName: displayName,
      picture: picture,
      isSystem: isSystem,
      isAdministrator: isAdministrator,
    );
  }
}
