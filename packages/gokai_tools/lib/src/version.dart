import 'package:flutter_tools/src/base/process.dart';
import 'package:flutter_tools/src/base/platform.dart';
import 'package:flutter_tools/src/cache.dart';
import 'package:flutter_tools/src/globals.dart' as globals;
import 'package:flutter_tools/src/version.dart';

class GokaiVersionUpstreamValidator {
  GokaiVersionUpstreamValidator({
    required this.version,
    required this.platform,
  });

  final Platform platform;
  final FlutterVersion version;

  /// Performs the validation against the tracking remote of the [version].
  ///
  /// Returns [VersionCheckError] if the tracking remote is not standard.
  VersionCheckError? run(){
    final String? flutterGit = platform.environment['FLUTTER_GIT_URL'];
    final String? repositoryUrl = version.repositoryUrl;

    if (repositoryUrl == null) {
      return VersionCheckError(
          'The tool could not determine the remote upstream which is being '
              'tracked by the SDK.'
      );
    }

    // Strip `.git` suffix before comparing the remotes
    final List<String> sanitizedStandardRemotes = <String>[
      // If `FLUTTER_GIT_URL` is set, use that as standard remote.
      if (flutterGit != null) flutterGit
      // Else use the predefined standard remotes.
      else ..._standardRemotes,
    ].map((String remote) => stripDotGit(remote)).toList();

    final String sanitizedRepositoryUrl = stripDotGit(repositoryUrl);

    if (!sanitizedStandardRemotes.contains(sanitizedRepositoryUrl)) {
      if (flutterGit != null) {
        // If `FLUTTER_GIT_URL` is set, inform to either remove the
        // `FLUTTER_GIT_URL` environment variable or set it to the current
        // tracking remote.
        return VersionCheckError(
            'The Flutter SDK is tracking "$repositoryUrl" ($sanitizedRepositoryUrl, $sanitizedStandardRemotes) but "FLUTTER_GIT_URL" '
                'is set to "$flutterGit".\n'
                'Either remove "FLUTTER_GIT_URL" from the environment or set it to '
                '"$repositoryUrl". '
                'If this is intentional, it is recommended to use "git" directly to '
                'manage the SDK.'
        );
      }
      // If `FLUTTER_GIT_URL` is unset, inform to set the environment variable.
      return VersionCheckError(
          'The Flutter SDK is tracking a non-standard remote "$repositoryUrl".\n'
              'Set the environment variable "FLUTTER_GIT_URL" to '
              '"$repositoryUrl". '
              'If this is intentional, it is recommended to use "git" directly to '
              'manage the SDK.'
      );
    }
    return null;
  }

  // The predefined list of remotes that are considered to be standard.
  static final List<String> _standardRemotes = <String>[
    'https://github.com/ExpidusOS/gokai.git',
    'git@github.com:ExpidusOS/gokai.git',
  ];

  // Strips ".git" suffix from a given string, preferably an url.
  // For example, changes 'https://github.com/flutter/flutter.git' to 'https://github.com/flutter/flutter'.
  // URLs without ".git" suffix will remain unaffected.
  static final RegExp _patternUrlDotGit = RegExp(r'(.*)(\.git)$');
  static String stripDotGit(String url) {
    final RegExpMatch? match = _patternUrlDotGit.firstMatch(url);
    if (match == null) {
      return url;
    }
    return match.group(1)!;
  }
}

class GokaiVersion extends FlutterVersion {
  GokaiVersion({
    super.frameworkRevision,
    String? workingDirectory,
    String? gokaiRevision,
  }) : super(workingDirectory: workingDirectory) {
    _gokaiRevision = gokaiRevision ?? _runGit(
      FlutterVersion.gitLog(<String>['-n', '1', '--pretty=format:%H']).join(' '),
      globals.processUtils,
      workingDirectory,
    );
  }

  @override
  String get channel => 'stable';

  @override
  String get repositoryUrl => 'https://github.com/ExpidusOS/gokai';

  @override
  String get frameworkAge => 'unknown';

  @override
  String get frameworkCommitDate => 'unknown';

  String? _gokaiRevision;
  String? get gokaiRevision => _gokaiRevision;

  String get gokaiRevisionShort => _shortGitRevision(gokaiRevision);

  @override
  String toString() {
    final baseText = super.toString();
    final gokaiText = 'Gokai • revision $gokaiRevisionShort';

    return '$baseText\n$gokaiText';
  }

  @override
  Map<String, Object> toJson() {
    final base = super.toJson();
    base['gokaiRevision'] = gokaiRevision ?? 'unknown';
    return base;
  }
}

String _shortGitRevision(String? revision) {
  if (revision == null) {
    return '';
  }
  return revision.length > 10 ? revision.substring(0, 10) : revision;
}

String _runGit(String command, ProcessUtils processUtils, [String? workingDirectory]) {
  return processUtils.runSync(
    command.split(' '),
    workingDirectory: workingDirectory ?? Cache.flutterRoot,
  ).stdout.trim();
}