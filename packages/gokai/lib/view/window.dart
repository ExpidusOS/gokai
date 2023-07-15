import 'dart:ui';

class GokaiWindow {
  const GokaiWindow({
    required this.id,
    this.texture,
    this.title,
    this.role,
    required this.isToplevel,
    required this.isMapped,
    required this.childrenAbove,
    required this.childrenBelow,
    required this.hasDecorations,
    required this.rect,
    required this.enter,
    required this.leave,
    required this.setRect,
  });

  final String id;
  final int? texture;
  final String? title;
  final String? role;
  final bool isToplevel;
  final bool isMapped;
  final List<String> childrenAbove;
  final List<String> childrenBelow;
  final Rect rect;
  final bool hasDecorations;
  final VoidCallback enter;
  final VoidCallback leave;
  final void Function(Rect rect) setRect;
}