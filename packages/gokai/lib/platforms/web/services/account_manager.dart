import 'dart:html';
import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services/account_manager.dart';
import 'package:gokai/user/account.dart';
import 'package:gokai/user/id.dart';

class GokaiWebAccountManager extends GokaiAccountManager {
  static final _singleton = GokaiWebAccountManager._();

  factory GokaiWebAccountManager() => _singleton;

  GokaiWebAccountManager._() : super();

  @override
  Future<GokaiUserID> getCurrentId() async
    => GokaiUserID.uid(0);

  @override
  Future<List<GokaiUserID>> getIds() async
    => [ await getCurrentId() ];

  @override
  Future<GokaiUserAccount> get(GokaiUserID id) async {
    final curr = await getCurrentId();
    if (id.type != curr.type || id.value != curr.value) throw Exception('User does not exist');
    return GokaiUserAccount(
      id: id,
      language: window.navigator.language,
      displayName: 'Web',
      picture: null,
      isSystem: false,
      isAdministrator: false,
    );
  }
}
