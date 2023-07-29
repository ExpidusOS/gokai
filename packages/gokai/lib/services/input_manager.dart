import 'package:event/event.dart';
import 'package:gokai/service.dart';

enum GokaiPointerEventType {
  hover,
  button,
  leave,
}

class GokaiPointerEvent {
  const GokaiPointerEvent({
    required this.type,
    required this.windowId,
    required this.x,
    required this.y,
    required this.button,
    required this.isReleased,
  });

  final GokaiPointerEventType type;
  final String windowId;
  final double x;
  final double y;
  final int button;
  final bool isReleased;

  Map<String, dynamic> toJson() => <String, dynamic>{
    'type': type.name,
    'windowId': windowId,
    'x': x,
    'y': y,
    'button': button,
    'isReleased': isReleased
  };

  static GokaiPointerEvent fromJson(Map<String, dynamic> json) =>
    GokaiPointerEvent(
      type: GokaiPointerEventType.values.firstWhere((e) => e == (json['type'] as String)),
      windowId: json['windowId'] as String,
      x: json['x'] as double,
      y: json['y'] as double,
      button: json['button'] as int,
      isReleased: json['isReleased'] as bool,
    );
}

enum GokaiTouchEventType {
  up,
  down,
  motion,
  cancel,
}

class GokaiTouchEvent {
  const GokaiTouchEvent({
    required this.type,
    required this.id,
    required this.windowId,
    required this.x,
    required this.y,
  });

  final GokaiTouchEventType type;
  final int id;
  final String windowId;
  final double x;
  final double y;

  Map<String, dynamic> toJson() => <String, dynamic>{
    'type': type.name,
    'id': id,
    'windowId': windowId,
    'x': x,
    'y': y,
  };

  static GokaiTouchEvent fromJson(Map<String, dynamic> json) =>
    GokaiTouchEvent(
      type: GokaiTouchEventType.values.firstWhere((e) => e == (json['type'] as String)),
      id: json['id'] as int,
      windowId: json['windowId'] as String,
      x: json['x'] as double,
      y: json['y'] as double,
    );
}

class GokaiInputManager extends GokaiService {
  GokaiInputManager() : super(serviceName: 'InputManager');

  Event<Value<String>> onChange = Event<Value<String>>();
  Event<Value<bool>> onLid = Event<Value<bool>>();
  Event<Value<bool>> onTablet = Event<Value<bool>>();

  Future<List<String>> getIds() {
    throw UnimplementedError('getIds() has not been implemented');
  }

  Future<bool> sendPointer(GokaiPointerEvent event) {
    throw UnimplementedError('sendPointer() has not been implemented');
  }

  Future<bool> sendTouch(GokaiTouchEvent event) {
    throw UnimplementedError('sendTouch() has not been implemented');
  }

  Future<bool> sendText(String windowId, String text) {
    throw UnimplementedError('sendText() has not been implemented');
  }

  Future<bool> sendKeyboard(String windowId, int keycode) {
    throw UnimplementedError('sendKeyboard() has not been implemented');
  }
}
