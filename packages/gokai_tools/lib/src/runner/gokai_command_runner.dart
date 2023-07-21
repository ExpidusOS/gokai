import 'dart:io';

import 'package:args/args.dart';
import 'package:args/command_runner.dart';
import 'package:completion/completion.dart';
import 'package:flutter_tools/src/base/common.dart';
import 'package:flutter_tools/src/base/context.dart';
import 'package:flutter_tools/src/base/file_system.dart';
import 'package:flutter_tools/src/base/terminal.dart';
import 'package:flutter_tools/src/base/user_messages.dart';
import 'package:flutter_tools/src/base/utils.dart';
import 'package:flutter_tools/src/artifacts.dart';
import 'package:flutter_tools/src/cache.dart';
import 'package:flutter_tools/src/convert.dart';
import 'package:flutter_tools/src/globals.dart' as globals;
import 'package:flutter_tools/src/version.dart';
import 'package:flutter_tools/src/tester/flutter_tester.dart';
import 'package:flutter_tools/src/web/web_device.dart';

/// Common gokai command line options.
abstract final class GokaiGlobalOptions {
  static const String kColorFlag = 'color';
  static const String kContinuousIntegrationFlag = 'ci';
  static const String kDeviceIdOption = 'device-id';
  static const String kDisableTelemetryFlag = 'disable-telemetry';
  static const String kEnableTelemetryFlag = 'enable-telemetry';
  static const String kLocalEngineOption = 'local-engine';
  static const String kLocalEngineSrcPathOption = 'local-engine-src-path';
  static const String kLocalGokaiOption = 'local-gokai';
  static const String kLocalWebSDKOption = 'local-web-sdk';
  static const String kMachineFlag = 'machine';
  static const String kPackagesOption = 'packages';
  static const String kPrefixedErrorsFlag = 'prefixed-errors';
  static const String kQuietFlag = 'quiet';
  static const String kShowTestDeviceFlag = 'show-test-device';
  static const String kShowWebServerDeviceFlag = 'show-web-server-device';
  static const String kSuppressAnalyticsFlag = 'suppress-analytics';
  static const String kVerboseFlag = 'verbose';
  static const String kVersionCheckFlag = 'version-check';
  static const String kVersionFlag = 'version';
  static const String kWrapColumnOption = 'wrap-column';
  static const String kWrapFlag = 'wrap';
}

class GokaiCommandRunner extends CommandRunner<void> {
  GokaiCommandRunner({
    bool verboseHelp = false
  }) : super(
    'gokai',
    'Manage your Gokai based Flutter app development.'
  ) {
    argParser.addFlag(GokaiGlobalOptions.kVerboseFlag,
        abbr: 'v',
        negatable: false,
        help: 'Noisy logging, including all shell commands executed.\n'
            'If used with "--help", shows hidden options. '
            'If used with "gokai doctor", shows additional diagnostic information. '
            '(Use "-vv" to force verbose logging in those cases.)');
    argParser.addFlag(GokaiGlobalOptions.kPrefixedErrorsFlag,
        negatable: false,
        help: 'Causes lines sent to stderr to be prefixed with "ERROR:".',
        hide: !verboseHelp);
    argParser.addFlag(GokaiGlobalOptions.kQuietFlag,
        negatable: false,
        hide: !verboseHelp,
        help: 'Reduce the amount of output from some commands.');
    argParser.addFlag(GokaiGlobalOptions.kWrapFlag,
        hide: !verboseHelp,
        help: 'Toggles output word wrapping, regardless of whether or not the output is a terminal.',
        defaultsTo: true);
    argParser.addOption(GokaiGlobalOptions.kWrapColumnOption,
        hide: !verboseHelp,
        help: 'Sets the output wrap column. If not set, uses the width of the terminal. No '
            'wrapping occurs if not writing to a terminal. Use "--no-wrap" to turn off wrapping '
            'when connected to a terminal.');
    argParser.addOption(GokaiGlobalOptions.kDeviceIdOption,
        abbr: 'd',
        help: 'Target device id or name (prefixes allowed).');
    argParser.addFlag(GokaiGlobalOptions.kVersionFlag,
        negatable: false,
        help: 'Reports the version of this tool.');
    argParser.addFlag(GokaiGlobalOptions.kMachineFlag,
        negatable: false,
        hide: !verboseHelp,
        help: 'When used with the "--version" flag, outputs the information using JSON.');
    argParser.addFlag(GokaiGlobalOptions.kColorFlag,
        hide: !verboseHelp,
        help: 'Whether to use terminal colors (requires support for ANSI escape sequences).',
        defaultsTo: true);
    argParser.addFlag(GokaiGlobalOptions.kVersionCheckFlag,
        defaultsTo: true,
        hide: !verboseHelp,
        help: 'Allow Flutter to check for updates when this command runs.');
    argParser.addFlag(GokaiGlobalOptions.kSuppressAnalyticsFlag,
        negatable: false,
        help: 'Suppress analytics reporting for the current CLI invocation.');
    argParser.addFlag(GokaiGlobalOptions.kDisableTelemetryFlag,
        negatable: false,
        help: 'Disable telemetry reporting each time a gokai or dart '
            'command runs, until it is re-enabled.');
    argParser.addFlag(GokaiGlobalOptions.kEnableTelemetryFlag,
        negatable: false,
        help: 'Enable telemetry reporting each time a gokai or dart '
            'command runs.');
    argParser.addOption(GokaiGlobalOptions.kPackagesOption,
        hide: !verboseHelp,
        help: 'Path to your "package_config.json" file.');
    if (verboseHelp) {
      argParser.addSeparator('Local build selection options (not normally required):');
    }

    argParser.addOption(GokaiGlobalOptions.kLocalEngineSrcPathOption,
        hide: !verboseHelp,
        help: 'Path to your engine src directory, if you are building Flutter or Gokai locally.\n'
            'Defaults to \$$kFlutterEngineEnvironmentVariableName if set, otherwise defaults to '
            'the path given in your pubspec.yaml dependency_overrides for $kFlutterEnginePackageName, '
            'if any.');

    argParser.addOption(GokaiGlobalOptions.kLocalEngineOption,
        hide: !verboseHelp,
        help: 'Name of a build output within the engine out directory, if you are building Flutter or Gokai locally.\n'
            'Use this to select a specific version of the engine if you have built multiple engine targets.\n'
            'This path is relative to "--local-engine-src-path" (see above).');

    argParser.addOption(GokaiGlobalOptions.kLocalGokaiOption,
        hide: !verboseHelp,
        help: 'Path to the build of Gokai to use, if you are using a local engine then this should be specified.');

    argParser.addOption(GokaiGlobalOptions.kLocalWebSDKOption,
        hide: !verboseHelp,
        help: 'Name of a build output within the engine out directory, if you are building Flutter or Gokai locally.\n'
            'Use this to select a specific version of the web sdk if you have built multiple engine targets.\n'
            'This path is relative to "--local-engine-src-path" (see above).');

    if (verboseHelp) {
      argParser.addSeparator('Options for testing the "gokai" tool itself:');
    }
    argParser.addFlag(GokaiGlobalOptions.kShowTestDeviceFlag,
        negatable: false,
        hide: !verboseHelp,
        help: 'List the special "flutter-tester" device in device listings. '
            'This headless device is used to test Flutter tooling.');
    argParser.addFlag(GokaiGlobalOptions.kShowWebServerDeviceFlag,
      negatable: false,
      hide: !verboseHelp,
      help: 'List the special "web-server" device in device listings.',
    );
    argParser.addFlag(
      GokaiGlobalOptions.kContinuousIntegrationFlag,
      negatable: false,
      help: 'Enable a set of CI-specific test debug settings.',
      hide: !verboseHelp,
    );
  }

  @override
  ArgParser get argParser => _argParser;
  final ArgParser _argParser = ArgParser(
    allowTrailingOptions: false,
    usageLineLength: globals.outputPreferences.wrapText ? globals.outputPreferences.wrapColumn : null,
  );

  @override
  String get usageFooter {
    return wrapText('Run "gokai help -v" for verbose help output, including less commonly used options.',
      columnWidth: globals.outputPreferences.wrapColumn,
      shouldWrap: globals.outputPreferences.wrapText,
    );
  }

  @override
  String get usage {
    final String usageWithoutDescription = super.usage.substring(description.length + 2);
    final String prefix = wrapText(description,
      shouldWrap: globals.outputPreferences.wrapText,
      columnWidth: globals.outputPreferences.wrapColumn,
    );
    return '$prefix\n\n$usageWithoutDescription';
  }

  @override
  ArgResults parse(Iterable<String> args) {
    try {
      return tryArgsCompletion(args.toList(), argParser);
    } on ArgParserException catch (error) {
      if (error.commands.isEmpty) {
        usageException(error.message);
      }

      Command<void>? command = commands[error.commands.first];
      for (final String commandName in error.commands.skip(1)) {
        command = command?.subcommands[commandName];
      }

      command!.usageException(error.message);
    }
  }

  @override
  Future<void> run(Iterable<String> args) {
    if (args.length == 1) {
      if (args.first == 'build') {
        args = <String>['build', '-h'];
      } else if (args.first == 'custom-devices') {
        args = <String>['custom-devices', '-h'];
      } else if (args.first == 'pub') {
        args = <String>['pub', '-h'];
      }
    }

    return super.run(args);
  }

  @override
  Future<void> runCommand(ArgResults topLevelResults) async {
    final contextOverrides = <Type, Object?>{};

    if (topLevelResults.wasParsed(GokaiGlobalOptions.kDisableTelemetryFlag) ||
      topLevelResults.wasParsed(GokaiGlobalOptions.kEnableTelemetryFlag)) {
      return;
    }

    int? wrapColumn;
    if (topLevelResults.wasParsed(GokaiGlobalOptions.kWrapColumnOption)) {
      try {
        wrapColumn = int.parse(topLevelResults[GokaiGlobalOptions.kWrapColumnOption] as String);
        if (wrapColumn < 0) {
          throwToolExit(userMessages.runnerWrapColumnInvalid(topLevelResults[GokaiGlobalOptions.kWrapColumnOption]));
        }
      } on FormatException {
        throwToolExit(userMessages.runnerWrapColumnParseError(topLevelResults[GokaiGlobalOptions.kWrapColumnOption]));
      }
    }

    final bool useWrapping = topLevelResults.wasParsed(GokaiGlobalOptions.kWrapFlag)
      ? topLevelResults[GokaiGlobalOptions.kWrapFlag] as bool
      : globals.stdio.terminalColumns != null && topLevelResults[GokaiGlobalOptions.kWrapFlag] as bool;
    contextOverrides[OutputPreferences] = OutputPreferences(
      wrapText: useWrapping,
      showColor: topLevelResults[GokaiGlobalOptions.kColorFlag] as bool?,
      wrapColumn: wrapColumn,
    );

    if (((topLevelResults[GokaiGlobalOptions.kShowTestDeviceFlag] as bool?) ?? false)
        || topLevelResults[GokaiGlobalOptions.kDeviceIdOption] == FlutterTesterDevices.kTesterDeviceId) {
      FlutterTesterDevices.showFlutterTesterDevice = true;
    }
    if (((topLevelResults[GokaiGlobalOptions.kShowWebServerDeviceFlag] as bool?) ?? false)
        || topLevelResults[GokaiGlobalOptions.kDeviceIdOption] == WebServerDevice.kWebServerDeviceId) {
      WebServerDevice.showWebServerDevice = true;
    }

    // Set up the tooling configuration.
    final EngineBuildPaths? engineBuildPaths = await globals.localEngineLocator?.findEnginePath(
      engineSourcePath: topLevelResults[GokaiGlobalOptions.kLocalEngineSrcPathOption] as String?,
      localEngine: topLevelResults[GokaiGlobalOptions.kLocalEngineOption] as String?,
      localWebSdk: topLevelResults[GokaiGlobalOptions.kLocalWebSDKOption] as String?,
      packagePath: topLevelResults[GokaiGlobalOptions.kPackagesOption] as String?,
    );
    if (engineBuildPaths != null) {
      contextOverrides.addAll(<Type, Object?>{
        Artifacts: Artifacts.getLocalEngine(engineBuildPaths),
      });
    }

    await context.run(
      overrides: contextOverrides.map((type, value) => MapEntry(type, () => value)),
      body: () async {
        globals.logger.quiet = (topLevelResults['quiet'] as bool?) ?? false;

        if (globals.platform.environment['FLUTTER_ALREADY_LOCKED'] != 'true') {
          await globals.cache.lock();
        }

        if ((topLevelResults['suppress-analytics'] as bool?) ?? false) {
          globals.flutterUsage.suppressAnalytics = true;
        }

        if (globals.platform.environment['GOKAI_VERSION_EXISTS'] != 'true') {
          globals.flutterVersion.ensureVersionFile();
        }

        final bool machineFlag = topLevelResults['machine'] as bool? ?? false;
        final bool ci = await globals.botDetector.isRunningOnBot;
        final bool redirectedCompletion = !globals.stdio.hasTerminal &&
            (topLevelResults.command?.name ?? '').endsWith('-completion');
        final bool isMachine = machineFlag || ci || redirectedCompletion;
        final bool versionCheckFlag = topLevelResults['version-check'] as bool? ?? false;
        final bool explicitVersionCheckPassed = topLevelResults.wasParsed('version-check') && versionCheckFlag;

        if (topLevelResults.command?.name != 'upgrade' &&
            (explicitVersionCheckPassed || (versionCheckFlag && !isMachine))) {
          await globals.flutterVersion.checkFlutterVersionFreshness();
        }

        // See if the user specified a specific device.
        final String? specifiedDeviceId = topLevelResults['device-id'] as String?;
        if (specifiedDeviceId != null) {
          globals.deviceManager?.specifiedDeviceId = specifiedDeviceId;
        }

        if ((topLevelResults['version'] as bool?) ?? false) {
          globals.flutterUsage.sendCommand('version');

          if (globals.platform.environment['GOKAI_DONT_UPDATE'] != 'true') {
            globals.flutterVersion.fetchTagsAndUpdate();
          }

          String status;
          if (machineFlag) {
            final Map<String, Object> jsonOut = globals.flutterVersion.toJson();
            jsonOut['flutterRoot'] = Cache.flutterRoot!;
            status = const JsonEncoder.withIndent('  ').convert(jsonOut);
          } else {
            status = globals.flutterVersion.toString();
          }
          globals.printStatus(status);
          return;
        }

        if (machineFlag && topLevelResults.command?.name != 'analyze') {
          throwToolExit('The "--machine" flag is only valid with the "--version" flag or the "analyze --suggestions" command.', exitCode: 2);
        }

        await super.runCommand(topLevelResults);
      },
    );
  }

  List<String> getRepoRoots() {
    final String root = globals.fs.path.absolute(Cache.flutterRoot!);
    return <String>['dev', 'examples', 'packages'].map<String>((String item) {
      return globals.fs.path.join(root, item);
    }).toList();
  }

  List<Directory> getRepoPackages() =>
    getRepoRoots()
      .expand<String>((String root) => _gatherProjectPaths(root))
      .map<Directory>((String dir) => globals.fs.directory(dir))
      .toList();

  static List<String> _gatherProjectPaths(String rootPath) {
    if (globals.fs.isFileSync(globals.fs.path.join(rootPath, '.dartignore'))) {
      return <String>[];
    }

    final List<String> projectPaths = globals.fs.directory(rootPath)
      .listSync(followLinks: false)
      .expand((FileSystemEntity entity) {
        if (entity is Directory && !globals.fs.path.split(entity.path).contains('.dart_tool')) {
          return _gatherProjectPaths(entity.path);
        }
        return <String>[];
      }).toList();

    if (globals.fs.isFileSync(globals.fs.path.join(rootPath, 'pubspec.yaml'))) {
      projectPaths.add(rootPath);
    }

    return projectPaths;
  }
}
