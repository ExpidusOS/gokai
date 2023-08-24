import 'dart:io' as fs;
import 'package:flutter/foundation.dart';

import 'dir.dart';

class FileStat implements fs.FileStat {
  FileStat(this.value);

  final GokaiFileSystemEntryStat value;

  @override
  DateTime get accessed => value.atime;

  @override
  DateTime get changed => value.ctime;

  @override
  DateTime get modified => value.mtime;

  @override
  int get mode => value.mode;

  @override
  int get size => value.size;

  @override
  String modeString() => mode.toString();

  @override
  fs.FileSystemEntityType get type {
    switch (value.type) {
      case 'regular': return fs.FileSystemEntityType.file;
      case 'directory': return fs.FileSystemEntityType.directory;
      case 'symlink': return fs.FileSystemEntityType.link;
      case 'socket': return fs.FileSystemEntityType.unixDomainSock;
    }
    return fs.FileSystemEntityType.notFound;
  }
}

class GokaiFileSystemEntryStat {
  const GokaiFileSystemEntryStat({
    required this.path,
    required this.type,
    required this.dev,
    required this.mode,
    required this.nlink,
    required this.uid,
    required this.gid,
    required this.rdev,
    required this.ino,
    required this.size,
    required this.blksize,
    required this.blocks,
    required this.flags,
    required this.gen,
    required this.atime,
    required this.mtime,
    required this.ctime,
    required this.btime,
  });

  final String path;
  final String type;

  final int dev;
  final int mode;
  final int nlink;
  final int uid;
  final int gid;
  final int rdev;
  final int ino;
  final int size;
  final int blksize;
  final int blocks;
  final int flags;
  final int gen;

  final DateTime atime;
  final DateTime mtime;
  final DateTime ctime;
  final DateTime btime;

  fs.FileStat toDart() => FileStat(this);

  static GokaiFileSystemEntryStat fromJson(Map<String, dynamic> json)
    => GokaiFileSystemEntryStat(
      path: json['path'] as String,
      type: json['path'] as String,
      dev: json['dev'] as int,
      mode: json['mode'] as int,
      nlink: json['nlink'] as int,
      uid: json['uid'] as int,
      gid: json['gid'] as int,
      rdev: json['rdev'] as int,
      ino: json['ino'] as int,
      size: json['size'] as int,
      blksize: json['blksize'] as int,
      blocks: json['blocks'] as int,
      flags: json['flags'] as int,
      gen: json['gen'] as int,
      atime: DateTime.fromMillisecondsSinceEpoch(json['atime'] as int),
      mtime: DateTime.fromMillisecondsSinceEpoch(json['mtime'] as int),
      ctime: DateTime.fromMillisecondsSinceEpoch(json['ctime'] as int),
      btime: DateTime.fromMillisecondsSinceEpoch(json['btime'] as int),
    );
}

class FileSystemEntity extends fs.FileSystemEntity {
  FileSystemEntity(this.value);

  final GokaiFileSystemEntry value;

  @override
  fs.FileSystemEntity get absolute => isAbsolute ? this : throw UnimplementedError();

  @override
  fs.Directory get parent => (value as GokaiFileSystemDirectory).getParent().toDart() as fs.Directory;

  @override
  String get path => value.path;

  void _reportError(dynamic error, StackTrace stackTrace) {
    FlutterError.reportError(FlutterErrorDetails(
      exception: error,
      stack: stackTrace,
      library: 'gokai',
    ));
  }

  @override
  Future<fs.FileSystemEntity> delete({ bool recursive = false }) async {
    if (await value.delete()) return this;
    throw Exception('Failed to delete $path');
  }

  @override
  void deleteSync({ bool recursive = false }) {
    delete(recursive: recursive).catchError(_reportError);
  }

  @override
  Future<bool> exists() => value.exists();

  @override
  bool existsSync() {
    throw UnimplementedError('Cannot check if $path exists synchronously');
  }

  @override
  Future<fs.FileSystemEntity> rename(String p) async => (await value.rename(p)).toDart();

  @override
  fs.FileSystemEntity renameSync(String p) {
    throw UnimplementedError('Cannot rename($p) synchronously');
  }

  @override
  Future<String> resolveSymbolicLinks() {
    throw UnimplementedError('resolveSymbolicLinks() has not been implemented');
  }

  @override
  String resolveSymbolicLinksSync() {
    throw UnimplementedError('Cannot resolveSymbolicLinks() synchronously');
  }

  @override
  Future<fs.FileStat> stat() async => (await value.stat()).toDart();

  @override
  fs.FileStat statSync() {
    throw UnimplementedError('Cannot stat() synchronously');
  }
}

class GokaiFileSystemEntry {
  const GokaiFileSystemEntry(this.path);

  final String path;

  Future<GokaiFileSystemEntry> rename(String p) {
    throw UnimplementedError('rename("$p") has not been implemented');
  }

  Future<bool> delete() {
    throw UnimplementedError('delete("$path") has not been implemented');
  }

  Future<bool> exists() {
    throw UnimplementedError('exists("$path") has not been implemented');
  }

  Future<GokaiFileSystemEntry> copy(String p) {
    throw UnimplementedError('copy("$p") has not been implemented');
  }

  Future<GokaiFileSystemEntryStat> stat() {
    throw UnimplementedError('stat("$path") has not been implemented');
  }

  fs.FileSystemEntity toDart() => FileSystemEntity(this);
}
