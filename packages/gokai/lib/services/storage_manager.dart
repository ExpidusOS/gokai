import 'package:gokai/fs/entry.dart';
import 'package:gokai/service.dart';

class GokaiStorageManager extends GokaiService {
  GokaiStorageManager() : super(serviceName: 'StorageManager');

  GokaiFileSystemEntry open(String path) => GokaiFileSystemEntry(path);
}