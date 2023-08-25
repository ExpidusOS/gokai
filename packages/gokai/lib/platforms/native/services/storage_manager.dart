import 'package:path/path.dart' as Path;
import 'package:flutter/foundation.dart';
import 'package:gokai/fs/dir.dart';
import 'package:gokai/fs/entry.dart';
import 'package:gokai/fs/file.dart';
import 'package:flutter/services.dart';
import 'package:gokai/services.dart';

class GokaiNativeFileSystemEntry extends GokaiFileSystemEntry {
  GokaiNativeFileSystemEntry(super.path, this.instance);

  final GokaiNativeStorageManager instance;

  @override
  Future<GokaiFileSystemEntry> rename(String p) async {
    await instance.methodChannel.invokeMethod('rename', {
      'source': path,
      'dest': p,
    });
    return GokaiNativeFileSystemEntry(p, instance);
  }

  @override
  Future<bool> delete() async => await instance.methodChannel.invokeMethod('delete', path);

  @override
  Future<bool> exists() async => await instance.methodChannel.invokeMethod('exists', path);

  @override
  Future<GokaiFileSystemEntry> copy(String p) async {
    await instance.methodChannel.invokeMethod('copy', {
      'source': path,
      'dest': p,
    });
    return GokaiNativeFileSystemEntry(p, instance);
  }

  @override
  Future<GokaiFileSystemEntryStat> stat() async => GokaiFileSystemEntryStat.fromJson(await instance.methodChannel.invokeMethod('stat', path));

  @override
  GokaiFileSystemDirectory getParent() => GokaiNativeFileSystemDirectory(Path.dirname(path), instance);

  @override
  GokaiFileSystemDirectory toDirectory() => GokaiNativeFileSystemDirectory(path, instance);

  @override
  GokaiFileSystemFile toFile() => GokaiNativeFileSystemFile(path, instance);
}

class GokaiNativeFileSystemDirectory extends GokaiFileSystemDirectory {
  GokaiNativeFileSystemDirectory(super.path, this.instance);

  final GokaiNativeStorageManager instance;

  @override
  Future<GokaiFileSystemEntry> rename(String p) async {
    await instance.methodChannel.invokeMethod('rename', {
      'source': path,
      'dest': p,
    });
    return GokaiNativeFileSystemEntry(p, instance);
  }

  @override
  Future<bool> delete() async => await instance.methodChannel.invokeMethod('delete', path);

  @override
  Future<bool> exists() async => await instance.methodChannel.invokeMethod('exists', path);

  @override
  Future<GokaiFileSystemEntryStat> stat() async => GokaiFileSystemEntryStat.fromJson(await instance.methodChannel.invokeMethod('stat', path));

  @override
  Future<GokaiFileSystemEntry> copy(String p) async {
    await instance.methodChannel.invokeMethod('copy', {
      'source': path,
      'dest': p,
    });
    return GokaiNativeFileSystemEntry(p, instance);
  }

  @override
  Future<void> link(String p) async {
    await instance.methodChannel.invokeMethod('link', {
      'source': path,
      'dest': p,
      'type': 'dir',
    });
  }

  @override
  Future<bool> create(bool recursive) async =>
    await instance.methodChannel.invokeMethod('create', {
      'path': path,
      'recursive': recursive,
      'type': 'dir',
    });

  @override
  Future<List<GokaiFileSystemEntry>> list() async
    => (await instance.methodChannel.invokeListMethod<String>('list', path))!
      .map((e) => GokaiNativeFileSystemEntry(e, instance))
      .toList();
}

class GokaiNativeFileSystemFile extends GokaiFileSystemFile {
  GokaiNativeFileSystemFile(super.path, this.instance);

  final GokaiNativeStorageManager instance;

  @override
  Future<GokaiFileSystemEntry> rename(String p) async {
    await instance.methodChannel.invokeMethod('rename', {
      'source': path,
      'dest': p,
    });
    return GokaiNativeFileSystemEntry(p, instance);
  }

  @override
  Future<bool> delete() async => await instance.methodChannel.invokeMethod('delete', path);

  @override
  Future<bool> exists() async => await instance.methodChannel.invokeMethod('exists', path);

  @override
  Future<GokaiFileSystemEntryStat> stat() async => GokaiFileSystemEntryStat.fromJson(await instance.methodChannel.invokeMethod('stat', path));

  @override
  Future<GokaiFileSystemEntry> copy(String p) async {
    await instance.methodChannel.invokeMethod('copy', {
      'source': path,
      'dest': p,
    });
    return GokaiNativeFileSystemEntry(p, instance);
  }

  @override
  Future<bool> create(bool recursive, bool exclusive) async =>
    await instance.methodChannel.invokeMethod('create', {
      'path': path,
      'recursive': recursive,
      'exclusive': exclusive,
      'type': 'file',
    });

  @override
  Future<void> write(List<int> value) async =>
    await instance.methodChannel.invokeMethod('write', {
      'path': path,
      'buffer': value,
    });

  @override
  Future<List<int>> read() async =>
    (await instance.methodChannel.invokeListMethod<int>('read', path))!;
}

class GokaiNativeStorageManager extends GokaiStorageManager {
  static final _singleton = GokaiNativeStorageManager._();

  @visibleForTesting
  final methodChannel = const MethodChannel('Gokai::Services::StorageManager', JSONMethodCodec());

  factory GokaiNativeStorageManager() => _singleton;

  GokaiNativeStorageManager._() : super();

  @override
  GokaiFileSystemEntry open(String path) => GokaiNativeFileSystemEntry(path, this);
}