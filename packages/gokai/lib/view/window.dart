import 'dart:ui';

class GokaiWindow {
  const GokaiWindow({
    required this.id,
    this.texture,
    this.title,
    required this.hasDecorations,
    required this.rect,
    required this.enter,
    required this.leave,
    required this.setRect,
  });

  final String id;
  final int? texture;
  final String? title;
  final Rect rect;
  final bool hasDecorations;
  final VoidCallback enter;
  final VoidCallback leave;
  final void Function(Rect rect) setRect;
}