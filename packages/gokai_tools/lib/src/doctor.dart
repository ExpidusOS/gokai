import 'package:gokai_tools/src/base/user_messages.dart';
import 'package:gokai_tools/src/linux/linux_doctor.dart';
import 'package:gokai_tools/src/version.dart';
import 'package:process/process.dart';
import 'package:flutter_tools/src/base/context.dart';
import 'package:flutter_tools/src/base/file_system.dart';
import 'package:flutter_tools/src/base/io.dart';
import 'package:flutter_tools/src/base/os.dart';
import 'package:flutter_tools/src/base/net.dart';
import 'package:flutter_tools/src/base/platform.dart';
import 'package:flutter_tools/src/base/user_messages.dart';
import 'package:flutter_tools/src/linux/linux_workflow.dart';
import 'package:flutter_tools/src/android/android_studio_validator.dart';
import 'package:flutter_tools/src/android/android_workflow.dart';
import 'package:flutter_tools/src/custom_devices/custom_device_workflow.dart';
import 'package:flutter_tools/src/intellij/intellij_validator.dart';
import 'package:flutter_tools/src/vscode/vscode_validator.dart';
import 'package:flutter_tools/src/macos/xcode_validator.dart';
import 'package:flutter_tools/src/macos/macos_workflow.dart';
import 'package:flutter_tools/src/windows/visual_studio_validator.dart';
import 'package:flutter_tools/src/windows/windows_version_validator.dart';
import 'package:flutter_tools/src/windows/windows_workflow.dart';
import 'package:flutter_tools/src/web/chrome.dart';
import 'package:flutter_tools/src/web/web_validator.dart';
import 'package:flutter_tools/src/web/workflow.dart';
import 'package:flutter_tools/src/artifacts.dart';
import 'package:flutter_tools/src/cache.dart';
import 'package:flutter_tools/src/features.dart';
import 'package:flutter_tools/src/doctor.dart';
import 'package:flutter_tools/src/doctor_validator.dart';
import 'package:flutter_tools/src/globals.dart' as globals;
import 'package:flutter_tools/src/proxy_validator.dart';
import 'package:flutter_tools/src/http_host_validator.dart';
import 'package:flutter_tools/src/version.dart';

abstract class GokaiDoctorValidatorsProvider extends DoctorValidatorsProvider {
  // Allow tests to construct a [_DefaultDoctorValidatorsProvider] with explicit
  // [FeatureFlags].
  factory GokaiDoctorValidatorsProvider.test({
    Platform? platform,
    required FeatureFlags featureFlags,
  }) {
    return _DefaultDoctorValidatorsProvider(
      featureFlags: featureFlags,
      platform: platform ?? FakePlatform(),
    );
  }
  /// The singleton instance, pulled from the [AppContext].
  static DoctorValidatorsProvider get _instance => context.get<DoctorValidatorsProvider>()!;

  static final DoctorValidatorsProvider defaultInstance = _DefaultDoctorValidatorsProvider(
    platform: globals.platform,
    featureFlags: featureFlags,
  );

  List<DoctorValidator> get validators;
  List<Workflow> get workflows;
}

class _DefaultDoctorValidatorsProvider implements GokaiDoctorValidatorsProvider {
  _DefaultDoctorValidatorsProvider({
    required this.platform,
    required this.featureFlags,
  });

  List<DoctorValidator>? _validators;
  List<Workflow>? _workflows;
  final Platform platform;
  final FeatureFlags featureFlags;

  late final LinuxWorkflow linuxWorkflow = LinuxWorkflow(
    platform: platform,
    featureFlags: featureFlags,
  );

  late final WebWorkflow webWorkflow = WebWorkflow(
    platform: platform,
    featureFlags: featureFlags,
  );

  late final MacOSWorkflow macOSWorkflow = MacOSWorkflow(
    platform: platform,
    featureFlags: featureFlags,
  );

  late final CustomDeviceWorkflow customDeviceWorkflow = CustomDeviceWorkflow(
    featureFlags: featureFlags,
  );

  @override
  List<DoctorValidator> get validators {
    if (_validators != null) {
      return _validators!;
    }

    final List<DoctorValidator> ideValidators = <DoctorValidator>[
      if (androidWorkflow!.appliesToHostPlatform)
        ...AndroidStudioValidator.allValidators(globals.config, platform, globals.fs, globals.userMessages),
      ...IntelliJValidator.installedValidators(
        fileSystem: globals.fs,
        platform: platform,
        userMessages: userMessages,
        plistParser: globals.plistParser,
        processManager: globals.processManager,
      ),
      ...VsCodeValidator.installedValidators(globals.fs, platform, globals.processManager),
    ];
    final ProxyValidator proxyValidator = ProxyValidator(platform: platform);
    _validators = <DoctorValidator>[
      GokaiValidator(
        fileSystem: globals.fs,
        platform: globals.platform,
        flutterVersion: () => globals.flutterVersion,
        devToolsVersion: () => globals.cache.devToolsVersion,
        processManager: globals.processManager,
        userMessages: userMessages,
        artifacts: globals.artifacts!,
        flutterRoot: () => Cache.flutterRoot!,
        operatingSystemUtils: globals.os,
      ),
      if (platform.isWindows)
        WindowsVersionValidator(
          operatingSystemUtils: globals.os,
        ),
      if (androidWorkflow!.appliesToHostPlatform)
        GroupedValidator(<DoctorValidator>[androidValidator!, androidLicenseValidator!]),
      if (globals.iosWorkflow!.appliesToHostPlatform || macOSWorkflow.appliesToHostPlatform)
        GroupedValidator(<DoctorValidator>[XcodeValidator(xcode: globals.xcode!, userMessages: userMessages), globals.cocoapodsValidator!]),
      if (webWorkflow.appliesToHostPlatform)
        ChromeValidator(
          chromiumLauncher: ChromiumLauncher(
            browserFinder: findChromeExecutable,
            fileSystem: globals.fs,
            operatingSystemUtils: globals.os,
            platform:  globals.platform,
            processManager: globals.processManager,
            logger: globals.logger,
          ),
          platform: globals.platform,
        ),
      if (linuxWorkflow.appliesToHostPlatform)
        LinuxDoctorValidator(
          processManager: globals.processManager,
          userMessages: userMessages,
        ),
      if (windowsWorkflow!.appliesToHostPlatform)
        visualStudioValidator!,
      if (ideValidators.isNotEmpty)
        ...ideValidators
      else
        NoIdeValidator(),
      if (proxyValidator.shouldShow)
        proxyValidator,
      if (globals.deviceManager?.canListAnything ?? false)
        DeviceValidator(
          deviceManager: globals.deviceManager,
          userMessages: globals.userMessages,
        ),
      HttpHostValidator(
        platform: globals.platform,
        featureFlags: featureFlags,
        httpClient: globals.httpClientFactory?.call() ?? HttpClient(),
      ),
    ];
    return _validators!;
  }

  @override
  List<Workflow> get workflows {
    if (_workflows == null) {
      _workflows = <Workflow>[];

      if (globals.iosWorkflow!.appliesToHostPlatform) {
        _workflows!.add(globals.iosWorkflow!);
      }

      if (androidWorkflow?.appliesToHostPlatform ?? false) {
        _workflows!.add(androidWorkflow!);
      }

      if (linuxWorkflow.appliesToHostPlatform) {
        _workflows!.add(linuxWorkflow);
      }

      if (macOSWorkflow.appliesToHostPlatform) {
        _workflows!.add(macOSWorkflow);
      }

      if (windowsWorkflow?.appliesToHostPlatform ?? false) {
        _workflows!.add(windowsWorkflow!);
      }

      if (webWorkflow.appliesToHostPlatform) {
        _workflows!.add(webWorkflow);
      }

      if (customDeviceWorkflow.appliesToHostPlatform) {
        _workflows!.add(customDeviceWorkflow);
      }
    }
    return _workflows!;
  }
}

class GokaiValidator extends DoctorValidator implements FlutterValidator {
  GokaiValidator({
    required Platform platform,
    required FlutterVersion Function() flutterVersion,
    required String Function() devToolsVersion,
    required UserMessages userMessages,
    required FileSystem fileSystem,
    required Artifacts artifacts,
    required ProcessManager processManager,
    required String Function() flutterRoot,
    required OperatingSystemUtils operatingSystemUtils,
  }) : _flutterVersion = flutterVersion,
       _devToolsVersion = devToolsVersion,
       _platform = platform,
       _userMessages = userMessages,
       _fileSystem = fileSystem,
       _artifacts = artifacts,
       _processManager = processManager,
       _flutterRoot = flutterRoot,
       _operatingSystemUtils = operatingSystemUtils,
       super('Gokai');

  final Platform _platform;
  final FlutterVersion Function() _flutterVersion;
  final String Function() _devToolsVersion;
  final String Function() _flutterRoot;
  final UserMessages _userMessages;
  final FileSystem _fileSystem;
  final Artifacts _artifacts;
  final ProcessManager _processManager;
  final OperatingSystemUtils _operatingSystemUtils;

  @override
  Future<ValidationResult> validate() async {
    final List<ValidationMessage> messages = <ValidationMessage>[];
    String? versionChannel;
    String? frameworkVersion;

    try {
      final version = _flutterVersion() as GokaiVersion;
      final String? gitUrl = _platform.environment['FLUTTER_GIT_URL'];
      versionChannel = version.channel;
      frameworkVersion = version.gokaiRevision ?? '0.0.0-unknown';

      final String flutterRoot = _flutterRoot();
      messages.add(_getGokaiVersionMessage(frameworkVersion, versionChannel, flutterRoot));

      _validateRequiredBinaries(flutterRoot).forEach(messages.add);
      messages.add(_getGokaiUpstreamMessage(version));
      if (gitUrl != null) {
        messages.add(ValidationMessage(_userMessages.flutterGitUrl(gitUrl)));
      }
      messages.add(ValidationMessage(_userMessages.flutterRevision(
        version.frameworkRevisionShort,
        version.frameworkAge,
        version.frameworkCommitDate,
      )));
      messages.add(ValidationMessage(_userMessages.engineRevision(version.engineRevisionShort)));
      messages.add(ValidationMessage(_userMessages.dartRevision(version.dartSdkVersion)));
      messages.add(ValidationMessage(_userMessages.devToolsVersion(_devToolsVersion())));
      final String? pubUrl = _platform.environment[kPubDevOverride];
      if (pubUrl != null) {
        messages.add(ValidationMessage(_userMessages.pubMirrorURL(pubUrl)));
      }
      final String? storageBaseUrl = _platform.environment[kFlutterStorageBaseUrl];
      if (storageBaseUrl != null) {
        messages.add(ValidationMessage(_userMessages.flutterMirrorURL(storageBaseUrl)));
      }
    } on VersionCheckError catch (e) {
      messages.add(ValidationMessage.error(e.message));
    }

    // Check that the binaries we downloaded for this platform actually run on it.
    // If the binaries are not downloaded (because android is not enabled), then do
    // not run this check.
    final String genSnapshotPath = _artifacts.getArtifactPath(Artifact.genSnapshot);
    if (_fileSystem.file(genSnapshotPath).existsSync() && !_genSnapshotRuns(genSnapshotPath)) {
      final StringBuffer buffer = StringBuffer();
      buffer.writeln(_userMessages.flutterBinariesDoNotRun);
      if (_platform.isLinux) {
        buffer.writeln(_userMessages.flutterBinariesLinuxRepairCommands);
      } else if (_platform.isMacOS && _operatingSystemUtils.hostPlatform == HostPlatform.darwin_arm64) {
        buffer.writeln('Flutter requires the Rosetta translation environment on ARM Macs. Try running:');
        buffer.writeln('  sudo softwareupdate --install-rosetta --agree-to-license');
      }
      messages.add(ValidationMessage.error(buffer.toString()));
    }

    ValidationType valid;
    if (messages.every((ValidationMessage message) => message.isInformation)) {
      valid = ValidationType.success;
    } else {
      // The issues for this validator stem from broken git configuration of the local install;
      // in that case, make it clear that it is fine to continue, but freshness check/upgrades
      // won't be supported.
      valid = ValidationType.partial;
      messages.add(
        ValidationMessage(_userMessages.flutterValidatorErrorIntentional),
      );
    }

    return ValidationResult(
      valid,
      messages,
      statusInfo: _userMessages.flutterStatusInfo(
        versionChannel,
        frameworkVersion,
        _operatingSystemUtils.name,
        _platform.localeName,
      ),
    );
  }

  ValidationMessage _getGokaiVersionMessage(String frameworkVersion, String versionChannel, String flutterRoot) {
    String flutterVersionMessage = (_userMessages as GokaiUserMessages).gokaiVersion(frameworkVersion, versionChannel, flutterRoot);

    // The tool sets the channel as "unknown", if the current branch is on a
    // "detached HEAD" state or doesn't have an upstream, and sets the
    // frameworkVersion as "0.0.0-unknown" if  "git describe" on HEAD doesn't
    // produce an expected format to be parsed for the frameworkVersion.
    if (versionChannel != 'unknown' && frameworkVersion != '0.0.0-unknown') {
      return ValidationMessage(flutterVersionMessage);
    }
    if (versionChannel == 'unknown') {
      flutterVersionMessage = '$flutterVersionMessage\n${_userMessages.flutterUnknownChannel}';
    }
    if (frameworkVersion == '0.0.0-unknown') {
      flutterVersionMessage = '$flutterVersionMessage\n${_userMessages.flutterUnknownVersion}';
    }
    return ValidationMessage.hint(flutterVersionMessage);
  }

  List<ValidationMessage> _validateRequiredBinaries(String flutterRoot) {
    final ValidationMessage? gokaiWarning = _validateSdkBinary('gokai', flutterRoot);
    final ValidationMessage? dartWarning = _validateSdkBinary('dart', flutterRoot);
    return <ValidationMessage>[
      if (gokaiWarning != null) gokaiWarning,
      if (dartWarning != null) dartWarning,
    ];
  }

  /// Return a warning if the provided [binary] on the user's path does not
  /// resolve within the Flutter SDK.
  ValidationMessage? _validateSdkBinary(String binary, String flutterRoot) {
    final String flutterBinDir = _fileSystem.path.join(flutterRoot, 'bin');

    final File? flutterBin = _operatingSystemUtils.which(binary);
    if (flutterBin == null) {
      return ValidationMessage.hint(
        'The $binary binary is not on your path. Consider adding '
            '$flutterBinDir to your path.',
      );
    }
    final String resolvedFlutterPath = flutterBin.resolveSymbolicLinksSync();
    if (!_filePathContainsDirPath(flutterRoot, resolvedFlutterPath)) {
      final String hint = 'Warning: `$binary` on your path resolves to '
        '$resolvedFlutterPath, which is not inside your current Flutter '
        'SDK checkout at $flutterRoot. Consider adding $flutterBinDir to '
        'the front of your path.';
      return ValidationMessage.hint(hint);
    }
    return null;
  }

  bool _filePathContainsDirPath(String directory, String file) {
    // calling .canonicalize() will normalize for alphabetic case and path
    // separators
    return _fileSystem.path.canonicalize(file)
      .startsWith(_fileSystem.path.canonicalize(directory) + _fileSystem.path.separator);
  }

  ValidationMessage _getGokaiUpstreamMessage(FlutterVersion version) {
    final String? repositoryUrl = version.repositoryUrl;
    final VersionCheckError? upstreamValidationError = GokaiVersionUpstreamValidator(version: version, platform: _platform).run();

    // VersionUpstreamValidator can produce an error if repositoryUrl is null
    if (upstreamValidationError != null) {
      final String errorMessage = upstreamValidationError.message;
      if (errorMessage.contains('could not determine the remote upstream which is being tracked')) {
        return ValidationMessage.hint(_userMessages.flutterUpstreamRepositoryUnknown);
      }
      // At this point, repositoryUrl must not be null
      if (errorMessage.contains('Gokai SDK is tracking a non-standard remote')) {
        return ValidationMessage.hint(_userMessages.flutterUpstreamRepositoryUrlNonStandard(repositoryUrl!));
      }
      if (errorMessage.contains('Either remove "FLUTTER_GIT_URL" from the environment or set it to')){
        return ValidationMessage.hint(_userMessages.flutterUpstreamRepositoryUrlEnvMismatch(repositoryUrl!));
      }
    }
    return ValidationMessage(_userMessages.flutterUpstreamRepositoryUrl(repositoryUrl!));
  }

  bool _genSnapshotRuns(String genSnapshotPath) {
    const int kExpectedExitCode = 255;
    try {
      return _processManager.runSync(<String>[genSnapshotPath]).exitCode == kExpectedExitCode;
    } on Exception {
      return false;
    }
  }
}