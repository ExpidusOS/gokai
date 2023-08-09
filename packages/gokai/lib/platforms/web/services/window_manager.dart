import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services/window_manager.dart';
import 'package:gokai/view/window.dart';

class GokaiWebWindowManager extends GokaiWindowManager {
  static final _singleton = GokaiWebWindowManager._();

  factory GokaiWebWindowManager() => _singleton;
  GokaiWebWindowManager._() : super();

  @override
  Future<List<String>> getIds() async
    => [];
}
