import 'dart:convert';
import 'dart:io' as fs;
import 'dart:typed_data';
import 'entry.dart';

class File extends FileSystemEntity implements fs.File {
  File(super.value);

  @override
  fs.File get absolute => isAbsolute ? this : throw UnimplementedError();

  @override
  Future<fs.File> rename(String p) async => (await value.rename(p)).toDart() as File;

  @override
  fs.File renameSync(String p) {
    throw UnimplementedError('Cannot rename($p) synchronously');
  }

  @override
  Future<fs.File> copy(String newPath) async => (await value.copy(newPath)).toDart() as fs.File;

  @override
  fs.File copySync(String newPath) {
    throw UnimplementedError('Cannot copy() synchronously');
  }

  @override
  Future<fs.File> create({bool recursive = false, bool exclusive = false}) async {
    if (await (value as GokaiFileSystemFile).create(recursive, exclusive)) return this;
    throw Exception('Failed to create $path');
  }

  @override
  void createSync({bool recursive = false, bool exclusive = false}) {
    throw UnimplementedError('Cannot create() synchronously');
  }

  @override
  Future<DateTime> lastAccessed() async => (await value.stat()).atime;

  @override
  DateTime lastAccessedSync() {
    throw UnimplementedError();
  }

  @override
  Future<DateTime> lastModified() async => (await value.stat()).mtime;

  @override
  DateTime lastModifiedSync() {
    throw UnimplementedError();
  }

  @override
  Future<int> length() async => (await value.stat()).size;

  @override
  int lengthSync() {
    throw UnimplementedError();
  }

  @override
  Future<fs.RandomAccessFile> open({fs.FileMode mode = fs.FileMode.read}) {
    throw UnimplementedError();
  }

  @override
  Stream<List<int>> openRead([int? start, int? end]) {
    throw UnimplementedError();
  }

  @override
  fs.RandomAccessFile openSync({fs.FileMode mode = fs.FileMode.read}) {
    throw UnimplementedError();
  }

  @override
  fs.IOSink openWrite({fs.FileMode mode = fs.FileMode.write, Encoding encoding = utf8}) {
    throw UnimplementedError();
  }

  @override
  Future<Uint8List> readAsBytes() {
    // TODO: implement readAsBytes
    throw UnimplementedError();
  }

  @override
  Uint8List readAsBytesSync() {
    throw UnimplementedError();
  }

  @override
  Future<List<String>> readAsLines({Encoding encoding = utf8}) {
    // TODO: implement readAsLines
    throw UnimplementedError();
  }

  @override
  List<String> readAsLinesSync({Encoding encoding = utf8}) {
    throw UnimplementedError();
  }

  @override
  Future<String> readAsString({Encoding encoding = utf8}) {
    // TODO: implement readAsString
    throw UnimplementedError();
  }

  @override
  String readAsStringSync({Encoding encoding = utf8}) {
    // TODO: implement readAsStringSync
    throw UnimplementedError();
  }

  @override
  Future setLastAccessed(DateTime time) {
    throw UnimplementedError();
  }

  @override
  void setLastAccessedSync(DateTime time) {
    throw UnimplementedError();
  }

  @override
  Future setLastModified(DateTime time) {
    throw UnimplementedError();
  }

  @override
  void setLastModifiedSync(DateTime time) {
    throw UnimplementedError();
  }

  @override
  Future<fs.File> writeAsBytes(List<int> bytes, {fs.FileMode mode = fs.FileMode.write, bool flush = false}) async {
    await (value as GokaiFileSystemFile).write(bytes);
    return this;
  }

  @override
  void writeAsBytesSync(List<int> bytes, {fs.FileMode mode = fs.FileMode.write, bool flush = false}) {
    throw UnimplementedError();
  }

  @override
  Future<fs.File> writeAsString(String contents, {fs.FileMode mode = fs.FileMode.write, Encoding encoding = utf8, bool flush = false}) =>
    writeAsBytes(
      List.generate(contents.length, (index) => contents.codeUnitAt(index)),
      mode: mode,
      flush: flush,
    );

  @override
  void writeAsStringSync(String contents, {fs.FileMode mode = fs.FileMode.write, Encoding encoding = utf8, bool flush = false}) {
    throw UnimplementedError();
  }
}

class GokaiFileSystemFile extends GokaiFileSystemEntry {
  const GokaiFileSystemFile(super.path);

  Future<bool> create(bool recursive, bool exclusive) {
    throw UnimplementedError('create() has not been implemented');
  }

  Future<List<int>> read() {
    throw UnimplementedError('read() has not been implemented');
  }

  Future<void> write(List<int> value) {
    throw UnimplementedError('write() has not been implemented');
  }

  @override
  fs.FileSystemEntity toDart() => File(this);
}