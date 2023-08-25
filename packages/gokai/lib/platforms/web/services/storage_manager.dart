import 'dart:io';
import 'package:file/memory.dart';
import 'package:path/path.dart' as Path;
import 'package:flutter/foundation.dart';
import 'package:gokai/fs/dir.dart';
import 'package:gokai/fs/entry.dart';
import 'package:gokai/fs/file.dart';
import 'package:gokai/services/storage_manager.dart';

class GokaiWebFileSystemEntry extends GokaiFileSystemEntry {
  GokaiWebFileSystemEntry(super.path, this.instance);

  final GokaiWebStorageManager instance;

  @override
  Future<GokaiFileSystemEntry> rename(String p) async {
    switch (await instance.fs.type(path)) {
      case FileSystemEntityType.directory:
        await instance.fs.directory(path).rename(p);
        return GokaiWebFileSystemDirectory(p, instance);
      case FileSystemEntityType.link:
        await instance.fs.link(path).rename(p);
        return GokaiWebFileSystemEntry(p, instance);
      case FileSystemEntityType.file:
      case FileSystemEntityType.pipe:
      case FileSystemEntityType.unixDomainSock:
        await instance.fs.file(path).rename(p);
        return GokaiWebFileSystemEntry(p, instance);
      default:
        throw UnimplementedError('rename("$p") has not been implemented');
    }
  }

  @override
  Future<bool> delete() async {
    switch (await instance.fs.type(path)) {
      case FileSystemEntityType.directory:
        await instance.fs.directory(path).delete();
        return true;
      case FileSystemEntityType.link:
        await instance.fs.link(path).delete();
        return true;
      case FileSystemEntityType.file:
      case FileSystemEntityType.pipe:
      case FileSystemEntityType.unixDomainSock:
        await instance.fs.file(path).delete();
        return true;
      default:
        throw UnimplementedError('delete("$path") has not been implemented');
    }
  }

  @override
  Future<bool> exists() async => await instance.fs.type(path) != FileSystemEntityType.notFound;

  @override
  Future<GokaiFileSystemEntry> copy(String p) async {
    throw UnimplementedError('copy("$p") has not been implemented');
  }

  @override
  Future<GokaiFileSystemEntryStat> stat() {
    throw UnimplementedError('stat("$path") has not been implemented');
  }

  @override
  GokaiFileSystemDirectory getParent() => GokaiWebFileSystemDirectory(Path.dirname(path), instance);

  @override
  GokaiFileSystemDirectory toDirectory() => GokaiWebFileSystemDirectory(path, instance);

  @override
  GokaiFileSystemFile toFile() => GokaiWebFileSystemFile(path, instance);
}

class GokaiWebFileSystemDirectory extends GokaiFileSystemDirectory {
  GokaiWebFileSystemDirectory(super.path, this.instance);

  final GokaiWebStorageManager instance;

  @override
  Future<GokaiFileSystemEntry> rename(String p) async {
    await instance.fs.directory(path).rename(p);
    return GokaiWebFileSystemEntry(p, instance);
  }

  @override
  Future<bool> delete() async {
    await instance.fs.directory(path).delete();
    return true;
  }

  @override
  Future<bool> exists() async => await instance.fs.type(path) == FileSystemEntityType.directory;

  @override
  Future<void> link(String p) async {
    await instance.fs.link(p).create(path);
  }

  @override
  Future<bool> create(bool recursive) async {
    await instance.fs.directory(path).create(recursive: recursive);
    return true;
  }

  @override
  Future<List<GokaiFileSystemEntry>> list() async
    => instance.fs.directory(path).list()
      .map((e) => GokaiWebFileSystemEntry(e.path, instance))
      .toList();
}

class GokaiWebFileSystemFile extends GokaiFileSystemFile {
  GokaiWebFileSystemFile(super.path, this.instance);

  final GokaiWebStorageManager instance;

  @override
  Future<GokaiFileSystemEntry> rename(String p) async {
    await instance.fs.file(path).rename(p);
    return GokaiWebFileSystemEntry(p, instance);
  }

  @override
  Future<bool> delete() async {
    await instance.fs.file(path).delete();
    return true;
  }

  @override
  Future<bool> exists() async => await instance.fs.type(path) == FileSystemEntityType.file;

  @override
  Future<bool> create(bool recursive, bool exclusive) async {
    await instance.fs.file(path).create(recursive: recursive, exclusive: exclusive);
    return true;
  }

  @override
  Future<void> write(List<int> value) async {
    await instance.fs.file(path).writeAsBytes(value);
  }

  @override
  Future<List<int>> read() async => (await instance.fs.file(path).readAsBytes()).toList();
}

class GokaiWebStorageManager extends GokaiStorageManager {
  static final _singleton = GokaiWebStorageManager._();

  factory GokaiWebStorageManager() => _singleton;
  GokaiWebStorageManager._() : super();

  @visibleForTesting
  final fs = MemoryFileSystem();

  @override
  GokaiFileSystemEntry open(String path) => GokaiWebFileSystemEntry(path, this);
}