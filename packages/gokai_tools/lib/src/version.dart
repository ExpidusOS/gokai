import 'package:flutter_tools/src/base/process.dart';
import 'package:flutter_tools/src/cache.dart';
import 'package:flutter_tools/src/globals.dart' as globals;
import 'package:flutter_tools/src/version.dart';

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