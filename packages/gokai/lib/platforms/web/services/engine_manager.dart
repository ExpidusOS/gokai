import 'dart:html';
import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/constants.dart';
import 'package:gokai/flutter/engine.dart';
import 'package:gokai/services/engine_manager.dart';

const _kDefaultUUID = '00000000-0000-0000-0000-000000000000';

class GokaiWebEngineManager extends GokaiEngineManager {
  static final _singleton = GokaiWebEngineManager._();

  factory GokaiWebEngineManager() => _singleton;
  GokaiWebEngineManager._() : super();

  @override
  Future<String> getEngineId() async
    => _kDefaultUUID;

  @override
  Future<List<String>> getIds() async
    => [ _kDefaultUUID ];

  @override
  Future<GokaiFlutterEngine> get(String id) async {
    if (id != _kDefaultUUID) throw Exception('Engine $id does not exist');

    return GokaiFlutterEngine(
      id: _kDefaultUUID,
      viewType: GokaiFlutterEngineViewType.display,
      viewName: window.name!
    );
  }
}
