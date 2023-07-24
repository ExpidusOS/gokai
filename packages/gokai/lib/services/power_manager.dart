import 'package:event/event.dart';
import 'package:gokai/devices/power.dart';
import 'package:gokai/service.dart';

class GokaiPowerManager extends GokaiService {
  GokaiPowerManager() : super(serviceName: 'PowerManager');

  Event onChange = Event();

  Future<List<GokaiPowerDevice>> getAll() async {
    final ids = await getIds();
    var devices = <GokaiPowerDevice>[];
    for (final id in ids) {
      devices.add(await get(id));
    }
    return devices;
  }

  Future<List<String>> getIds() {
    throw UnimplementedError('getIds() has not been implemented');
  }

  Future<GokaiPowerDevice> get(String id) {
    throw UnimplementedError('get("$id") has not been implemented');
  }
}
