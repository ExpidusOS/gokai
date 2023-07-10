import 'dart:ui';
import 'package:gokai/user/account.dart';
import 'package:gokai/user/id.dart';
import 'package:gokai/service.dart';

class GokaiAccountManager extends GokaiService {
  GokaiAccountManager() : super(serviceName: 'AccountManager');

  List<VoidCallback> onChange = [];

  Future<List<GokaiUserAccount>> getAll() async {
    final ids = await getIds();
    var accounts = <GokaiUserAccount>[];
    for (final id in ids) {
      accounts.add(await get(id));
    }
    return accounts;
  }

  Future<GokaiUserAccount> getCurrent() async =>
    await get(await getCurrentId());

  Future<GokaiUserID> getCurrentId() {
    throw UnimplementedError('getCurrentId() has not been implemented');
  }

  Future<List<GokaiUserID>> getIds() {
    throw UnimplementedError('getIds() has not been implemented');
  }

  Future<GokaiUserAccount> get(GokaiUserID id) {
    throw UnimplementedError('get("$id") has not been implemented');
  }
}
