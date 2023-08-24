import 'dart:io' as fs;
import 'entry.dart';

class Directory extends FileSystemEntity implements fs.Directory {
  Directory(super.value);

  @override
  fs.Directory get absolute => isAbsolute ? this : throw UnimplementedError();

  @override
  Future<fs.Directory> rename(String p) async => (await value.rename(p)).toDart() as Directory;

  @override
  fs.Directory renameSync(String p) {
    throw UnimplementedError('Cannot rename($p) synchronously');
  }

  @override
  Future<fs.Directory> create({bool recursive = false}) async {
    if (await (value as GokaiFileSystemDirectory).create(recursive)) {
      return this;
    }
    throw Exception('Failed to create $path');
  }

  @override
  void createSync({bool recursive = false}) {
    throw UnimplementedError('Cannot create() synchronously');
  }

  @override
  Future<fs.Directory> createTemp([String? prefix]) {
    throw UnimplementedError();
  }

  @override
  fs.Directory createTempSync([String? prefix]) {
    throw UnimplementedError('Cannot createTemp() synchronously');
  }

  @override
  Stream<fs.FileSystemEntity> list({bool recursive = false, bool followLinks = true}) async* {
    for (final val in await (value as GokaiFileSystemDirectory).list()) {
      yield val.toDart();
    }
  }

  @override
  List<fs.FileSystemEntity> listSync({bool recursive = false, bool followLinks = true}) {
    throw UnimplementedError('Cannot list() synchronously');
  }
}

class GokaiFileSystemDirectory extends GokaiFileSystemEntry {
  const GokaiFileSystemDirectory(super.path);

  Future<void> link(String p) {
    throw UnimplementedError('link("$p") has not been implemented');
  }

  Future<bool> create(bool recursive) {
    throw UnimplementedError('create() has not been implemented');
  }

  Future<List<GokaiFileSystemEntry>> list() {
    throw UnimplementedError('list() has not been implemented');
  }

  GokaiFileSystemDirectory getParent() {
    throw UnimplementedError('getParent() has not been implemented');
  }

  @override
  fs.FileSystemEntity toDart() => Directory(this);
}
