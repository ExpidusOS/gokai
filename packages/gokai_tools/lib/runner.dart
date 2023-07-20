// Based on https://github.com/flutter/flutter/blob/de26ad0a8a5373a64dae84fa2019821a2c984047/packages/flutter_tools/lib/runner.dart

import 'dart:async';

import 'package:args/command_runner.dart';
import 'package:intl/intl.dart' as intl;
import 'package:intl/intl_standalone.dart' as intl_standalone;

import 'package:flutter_tools/src/base/async_guard.dart';
import 'package:flutter_tools/src/base/common.dart';
import 'package:flutter_tools/src/base/context.dart';
import 'package:flutter_tools/src/base/error_handling_io.dart';
import 'package:flutter_tools/src/base/file_system.dart';
import 'package:flutter_tools/src/base/io.dart';
import 'package:flutter_tools/src/base/logger.dart';
import 'package:flutter_tools/src/base/process.dart';
import 'package:flutter_tools/src/context_runner.dart';
import 'package:flutter_tools/src/doctor.dart';
import 'package:flutter_tools/src/globals.dart' as globals;
import 'package:flutter_tools/src/reporting/crash_reporting.dart';
import 'package:flutter_tools/src/reporting/first_run.dart';
import 'package:flutter_tools/src/reporting/reporting.dart';
import 'package:flutter_tools/src/runner/flutter_command.dart';
import 'src/runner/gokai_command_runner.dart';

Future<int> run(
  List<String> args,
  List<FlutterCommand> Function() commands, {
    bool muteCommandLogging = false,
    bool verbose = false,
    bool verboseHelp = false,
    bool? reportCrashes,
    String? flutterVersion,
    Map<Type, Generator>? overrides,
    required ShutdownHooks shutdownHooks,
  }) async {
  if (muteCommandLogging) {
    args = List<String>.of(args);
    args.removeWhere((String option) => option == '-vv' || option == '-v' || option == '--verbose');
  }

  return runInContext<int>(() async {
    reportCrashes ??= !await globals.isRunningOnBot;
    final runner = GokaiCommandRunner(verboseHelp: verboseHelp);
    commands().forEach(runner.addCommand);

    final String systemLocale = await intl_standalone.findSystemLocale();
    intl.Intl.defaultLocale = intl.Intl.verifiedLocale(
      systemLocale, intl.NumberFormat.localeExists,
      onFailure: (String _) => 'en_US',
    );

    String getVersion() => flutterVersion ?? globals.flutterVersion.getVersionString(redactUnknownBranches: true);
    Object? firstError;
    StackTrace? firstStackTrace;

    return runZoned<Future<int>>(() async {
      try {
        if (args.contains('--disable-telemetry') && args.contains('--enable-telemetry')) {
          throwToolExit(
            'Both enable and disable telemetry commands were detected when only one can be supplied per invocation.',
            exitCode: 1
          );
        }

        if (args.contains('--disable-telemetry')) {
          AnalyticsConfigEvent(enabled: false).send();

          await globals.flutterUsage.ensureAnalyticsSent();

          globals.flutterUsage.enabled = false;
          globals.printStatus('Analytics reporting disabled.');
          await globals.analytics.setTelemetry(false);
        }

        if (args.contains('--enable-telemetry')) {
          AnalyticsConfigEvent(enabled: true).send();

          globals.flutterUsage.enabled = true;
          globals.printStatus('Analytics reporting enabled.');

          await globals.analytics.setTelemetry(true);
        }

        await runner.run(args);

        if (firstError == null) {
          return await _exit(0, shutdownHooks: shutdownHooks);
        }
        return 1;
      } catch (error, stackTrace) {
        firstError = error;
        firstStackTrace = stackTrace;
        return _handleToolError(error, stackTrace, verbose, args, reportCrashes!, getVersion, shutdownHooks);
      }
    }, onError: (error, stackTrace) async { // ignore: deprecated_member_use
      firstError ??= error;
      firstStackTrace ??= stackTrace;
      await _handleToolError(firstError!, firstStackTrace, verbose, args, reportCrashes!, getVersion, shutdownHooks);
    });
  }, overrides: overrides);
}

Future<int> _handleToolError(
  Object error,
  StackTrace? stackTrace,
  bool verbose,
  List<String> args,
  bool reportCrashes,
  String Function() getFlutterVersion,
  ShutdownHooks shutdownHooks,
) async {
  if (error is UsageException) {
    globals.printError('${error.message}\n');
    globals.printError("Run 'gokai -h' (or 'gokai <command> -h') for available flutter commands and options.");
    // Argument error exit code.
    return _exit(64, shutdownHooks: shutdownHooks);
  } else if (error is ToolExit) {
    if (error.message != null) {
      globals.printError(error.message!);
    }
    if (verbose) {
      globals.printError('\n$stackTrace\n');
    }
    return _exit(error.exitCode ?? 1, shutdownHooks: shutdownHooks);
  } else if (error is ProcessExit) {
    if (error.immediate) {
      exit(error.exitCode);
      return error.exitCode;
    } else {
      return _exit(error.exitCode, shutdownHooks: shutdownHooks);
    }
  } else {
    globals.stdio.stderrWrite('\n');

    if (!reportCrashes) {
      globals.stdio.stderrWrite('$error\n$stackTrace\n');
      return _exit(1, shutdownHooks: shutdownHooks);
    }

    globals.flutterUsage.sendException(error);

    await asyncGuard(() async {
      final crashReportSender = CrashReportSender(
        usage: globals.flutterUsage,
        platform: globals.platform,
        logger: globals.logger,
        operatingSystemUtils: globals.os,
      );

      await crashReportSender.sendReport(
        error: error,
        stackTrace: stackTrace!,
        getFlutterVersion: getFlutterVersion,
        command: args.join(' '),
      );
    }, onError: (error) {
      globals.printError('Error sending crash report: $error');
    });

    globals.printError('Oops; gokai has exited unexpectedly: "$error".');

    try {
      final logger = BufferLogger(
        terminal: globals.terminal,
        outputPreferences: globals.outputPreferences,
      );

      final doctorText = DoctorText(logger);

      final details = CrashDetails(
        command: _crashCommand(args),
        error: error,
        stackTrace: stackTrace!,
        doctorText: doctorText
      );

      final file = await _createLocalCrashReport(details);
      await globals.crashReporter!.informUser(details, file);
      return _exit(1, shutdownHooks: shutdownHooks);
    } catch (error, st) {
      globals.stdio.stderrWrite(
        'Unable to generate crash report due to secondary error: $error\n$st\n'
        '${globals.userMessages.flutterToolBugInstructions}\n',
      );

      throw ProcessExit(1, immediate: true);
    }
  }
}

String _crashCommand(List<String> args) => 'gokai ${args.join(' ')}';

String _crashException(dynamic error) => '${error.runtimeType}: $error';

Future<File> _createLocalCrashReport(CrashDetails details) async {
  final buffer = StringBuffer();

  buffer.writeln('Gokai crash report.');
  buffer.writeln('${globals.userMessages.flutterToolBugInstructions}\n');

  buffer.writeln('## command\n');
  buffer.writeln('${details.command}\n');

  buffer.writeln('## exception\n');
  buffer.writeln('${_crashException(details.error)}\n');
  buffer.writeln('```\n${details.stackTrace}```\n');

  buffer.writeln('## gokai doctor\n');
  buffer.writeln('```\n${await details.doctorText.text}```');

  late File crashFile;
  ErrorHandlingFileSystem.noExitOnFailure(() {
    try {
      crashFile = globals.fsUtils.getUniqueFile(
        globals.fs.currentDirectory,
        'gokai',
        'log',
      );
      crashFile.writeAsStringSync(buffer.toString());
    } on FileSystemException catch (_) {
      // Fallback to the system temporary directory.
      try {
        crashFile = globals.fsUtils.getUniqueFile(
          globals.fs.systemTempDirectory,
          'gokai',
          'log',
        );
        crashFile.writeAsStringSync(buffer.toString());
      } on FileSystemException catch (e) {
        globals.printError('Could not write crash report to disk: $e');
        globals.printError(buffer.toString());

        rethrow;
      }
    }
  });
  return crashFile;
}

Future<int> _exit(int code, {required ShutdownHooks shutdownHooks}) async {
  final messenger = FirstRunMessenger(persistentToolState: globals.persistentToolState!);
  final legacyAnalyticsMessageShown = messenger.shouldDisplayLicenseTerms();

  globals.flutterUsage.printWelcome();

  if (globals.analytics.shouldShowMessage) {
    globals.logger.printStatus(globals.analytics.getConsentMessage);
    if (!globals.flutterUsage.enabled) {
      globals.printStatus(
        'Please note that analytics reporting was already disabled, '
        'and will continue to be disabled.\n');
    }

    if (legacyAnalyticsMessageShown) {
      globals.logger.printStatus('You have received two consent messages because '
        'the gokai tool is migrating to a new analytics system. '
        'Disabling analytics collection will disable both the legacy '
        'and new analytics collection systems. '
        'You can disable analytics reporting by running `gokai --disable-telemetry`\n');
    }

    globals.analytics.clientShowedMessage();
  }

  if (globals.flutterUsage.enabled) {
    final Stopwatch stopwatch = Stopwatch()..start();
    await globals.flutterUsage.ensureAnalyticsSent();
    globals.printTrace('ensureAnalyticsSent: ${stopwatch.elapsedMilliseconds}ms');
  }

  await shutdownHooks.runShutdownHooks(globals.logger);

  final completer = Completer<void>();

  Timer.run(() {
    try {
      globals.printTrace('exiting with code $code');
      exit(code);
      completer.complete();
    } catch (error, stackTrace) { // ignore: avoid_catches_without_on_clauses
      completer.completeError(error, stackTrace);
    }
  });

  await completer.future;
  return code;
}