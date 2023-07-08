import 'package:gokai/user/id.dart';
import 'package:gokai/service.dart';

class GokaiAccountManager extends GokaiService {
  GokaiAccountManager() : super(serviceName: 'AccountManager');

  Future<List<GokaiUserID>> getIds() {
    throw UnimplementedError('getIds() has not been implemented');
  }
}
