import 'package:args/args.dart';
import 'package:args/command_runner.dart';
import 'package:flutter_tools/src/base/utils.dart';
import 'package:flutter_tools/src/cache.dart';
import 'package:flutter_tools/src/globals.dart' as globals;

/// Common gokai command line options.
abstract final class GokaiGlobalOptions {
  static const String kColorFlag = 'color';
  static const String kContinuousIntegrationFlag = 'ci';
  static const String kDeviceIdOption = 'device-id';
  static const String kDisableTelemetryFlag = 'disable-telemetry';
  static const String kEnableTelemetryFlag = 'enable-telemetry';
  static const String kLocalEngineOption = 'local-engine';
  static const String kLocalEngineSrcPathOption = 'local-engine-src-path';
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
            'If used with "flutter doctor", shows additional diagnostic information. '
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
        help: 'Disable telemetry reporting each time a flutter or dart '
            'command runs, until it is re-enabled.');
    argParser.addFlag(GokaiGlobalOptions.kEnableTelemetryFlag,
        negatable: false,
        help: 'Enable telemetry reporting each time a flutter or dart '
            'command runs.');
    argParser.addOption(GokaiGlobalOptions.kPackagesOption,
        hide: !verboseHelp,
        help: 'Path to your "package_config.json" file.');
    if (verboseHelp) {
      argParser.addSeparator('Local build selection options (not normally required):');
    }

    argParser.addOption(GokaiGlobalOptions.kLocalEngineSrcPathOption,
        hide: !verboseHelp,
        help: 'Path to your engine src directory, if you are building Flutter locally.\n'
            'Defaults to \$$kFlutterEngineEnvironmentVariableName if set, otherwise defaults to '
            'the path given in your pubspec.yaml dependency_overrides for $kFlutterEnginePackageName, '
            'if any.');

    argParser.addOption(GokaiGlobalOptions.kLocalEngineOption,
        hide: !verboseHelp,
        help: 'Name of a build output within the engine out directory, if you are building Flutter locally.\n'
            'Use this to select a specific version of the engine if you have built multiple engine targets.\n'
            'This path is relative to "--local-engine-src-path" (see above).');

    argParser.addOption(GokaiGlobalOptions.kLocalWebSDKOption,
        hide: !verboseHelp,
        help: 'Name of a build output within the engine out directory, if you are building Flutter locally.\n'
            'Use this to select a specific version of the web sdk if you have built multiple engine targets.\n'
            'This path is relative to "--local-engine-src-path" (see above).');

    if (verboseHelp) {
      argParser.addSeparator('Options for testing the "flutter" tool itself:');
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
}