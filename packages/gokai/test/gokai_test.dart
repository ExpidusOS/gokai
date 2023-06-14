import 'package:flutter_test/flutter_test.dart';
import 'package:gokai/gokai.dart';
import 'package:gokai/gokai_platform_interface.dart';
import 'package:gokai/gokai_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockGokaiPlatform
    with MockPlatformInterfaceMixin
    implements GokaiPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final GokaiPlatform initialPlatform = GokaiPlatform.instance;

  test('$MethodChannelGokai is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelGokai>());
  });

  test('getPlatformVersion', () async {
    Gokai gokaiPlugin = Gokai();
    MockGokaiPlatform fakePlatform = MockGokaiPlatform();
    GokaiPlatform.instance = fakePlatform;

    expect(await gokaiPlugin.getPlatformVersion(), '42');
  });
}
