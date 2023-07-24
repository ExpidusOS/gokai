import 'package:event/event.dart';
import 'package:gokai/service.dart';

class GokaiInputManager extends GokaiService {
  GokaiInputManager() : super(serviceName: 'InputManager');

  Event<Value<String>> onChange = Event<Value<String>>();
  Event<Value<bool>> onLid = Event<Value<bool>>();
  Event<Value<bool>> onTablet = Event<Value<bool>>();

  Future<List<String>> getIds() {
    throw UnimplementedError('getIds() has not been implemented');
  }
}
