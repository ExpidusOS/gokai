import 'package:flutter_tools/src/base/platform.dart';
import 'package:flutter_tools/src/base/user_messages.dart';
import 'package:flutter_tools/src/cache.dart';
import 'package:flutter_tools/src/doctor.dart';
import 'package:flutter_tools/src/globals.dart' as globals;
import 'package:flutter_tools/src/pre_run_validator.dart';
import 'package:flutter_tools/src/version.dart';
import 'package:flutter_tools/src/commands/doctor.dart';
import 'package:flutter_tools/src/commands/shell_completion.dart';
import 'package:gokai_tools/src/base/user_messages.dart';
import 'package:gokai_tools/src/doctor.dart';
import 'package:gokai_tools/src/version.dart';

import 'runner.dart' as runner;

Future<void> main(List<String> args) async {
  final bool veryVerbose = args.contains('-vv');
  final bool verbose = args.contains('-v') || args.contains('--verbose') || veryVerbose;
  final bool prefixedErrors = args.contains('--prefixed-errors');
  final int powershellHelpIndex = args.indexOf('-?');
  if (powershellHelpIndex != -1) {
    args[powershellHelpIndex] = '-h';
  }

  final bool doctor = (args.isNotEmpty && args.first == 'doctor') ||
      (args.length == 2 && verbose && args.last == 'doctor');
  final bool help = args.contains('-h') || args.contains('--help') ||
      (args.isNotEmpty && args.first == 'help') || (args.length == 1 && verbose);
  final bool muteCommandLogging = (help || doctor) && !veryVerbose;
  final bool verboseHelp = help && verbose;
  final bool daemon = args.contains('daemon');
  final bool runMachine = (args.contains('--machine') && args.contains('run')) ||
      (args.contains('--machine') && args.contains('attach'));

  Cache.flutterRoot = Cache.defaultFlutterRoot(
    platform: const LocalPlatform(),
    fileSystem: globals.localFileSystem,
    userMessages: UserMessages(),
  );

  final flutterVersion = 'f468f3366c26a5092eb964a230ce7892fda8f2f8';

  await runner.run(
    args,
    () => [
      DoctorCommand(verbose: verbose),
      ShellCompletionCommand(),
    ],
    verbose: verbose,
    muteCommandLogging: muteCommandLogging,
    verboseHelp: verboseHelp,
    overrides: {
      DoctorValidatorsProvider: () => GokaiDoctorValidatorsProvider.defaultInstance,
      PreRunValidator: () => PreRunValidator(fileSystem: globals.fs),
      FlutterVersion: () => GokaiVersion(
        frameworkRevision: flutterVersion,
        gokaiRevision: globals.platform.environment['GOKAI_VERSION'],
      ),
      UserMessages: () => GokaiUserMessages(),
    },
    flutterVersion: flutterVersion,
    shutdownHooks: globals.shutdownHooks
  );
}
