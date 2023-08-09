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
    => [];
}
