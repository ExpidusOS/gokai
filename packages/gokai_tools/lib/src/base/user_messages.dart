import 'package:flutter_tools/src/base/user_messages.dart';

class GokaiUserMessages extends UserMessages {
  String gokaiVersion(String version, String channel, String flutterRoot) =>
      'Gokai version $version on channel $channel at $flutterRoot';

  String get gokaiLibrariesMissing => 'Gokai\'s native SDK is required for Linux development.\n'
    'Please look at the README.md in "package/gokai_sdk" in the Gokai source code for instructions to compile from source.';
}