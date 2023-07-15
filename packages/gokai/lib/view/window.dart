import 'dart:ui';

class GokaiWindow {
  const GokaiWindow({
    required this.id,
    this.texture,
    required this.enter,
    required this.leave,
  });

  final String id;
  final int? texture;
  final VoidCallback enter;
  final VoidCallback leave;
}