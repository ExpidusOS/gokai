enum GokaiUserIDType {
  guid,
  uid;

  static GokaiUserIDType from(dynamic value) {
    if (value is int) {
      return GokaiUserIDType.uid;
    } else if (value is String) {
      return GokaiUserIDType.guid;
    }

    throw Exception('Invalid type');
  }
}

class GokaiUserID {
  const GokaiUserID({
    required this.type,
    required this.value,
  });
  const GokaiUserID.guid(String value) : type = GokaiUserIDType.guid, value = value;
  const GokaiUserID.uid(int value) : type = GokaiUserIDType.uid, value = value;
  GokaiUserID.dynamic(dynamic value) : type = GokaiUserIDType.from(value), value = value;

  final GokaiUserIDType type;
  final dynamic value;

  @override
  String toString() => '${type.name}@${value.toString()}';
}
