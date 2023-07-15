import 'dart:ui';
import 'package:gokai/view/window.dart';
import 'package:gokai/service.dart';

typedef GokaiWindowCallback = void Function(String id);

class GokaiWindowManager extends GokaiService {
  GokaiWindowManager() : super(serviceName: 'WindowManager');

  List<VoidCallback> onChange = [];
  List<GokaiWindowCallback> onCommit = [];
  List<GokaiWindowCallback> onMapped = [];

  Future<List<GokaiWindow>> getViewable() async {
    final windows = await getAll();
    var list = <GokaiWindow>[];
    for (final win in windows) {
      if (!win.isMapped) continue;
      if (!win.isToplevel) continue;
      list.add(win);
    }
    return list;
  }

  Future<List<GokaiWindow>> getAll() async {
    final ids = await getIds();
    var list = <GokaiWindow>[];
    for (final id in ids) {
      list.add(await get(id));
    }
    return list;
  }

  Future<List<String>> getIds() {
    throw UnimplementedError('getIds() has not been implemented');
  }

  Future<GokaiWindow> get(String id) {
    throw UnimplementedError('get("$id") has not been implemented');
  }
}