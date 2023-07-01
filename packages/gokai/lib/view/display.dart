import 'dart:ui';
import 'package:vector_math/vector_math.dart';

class GokaiDisplayMode {
  const GokaiDisplayMode({
    required this.resolution,
    required this.refresh,
  });

  final Rect resolution;
  final double refresh;
}

class GokaiDisplay {
  const GokaiDisplay({
    required this.name,
    required this.model,
    required this.isHDR,
    required this.physicalSize,
    required this.getMode,
    required this.getModes,
  });

  final String name;
  final String model;
  final bool isHDR;
  final Vector2 physicalSize;
  final Future<GokaiDisplayMode> Function() getMode;
  final Future<List<GokaiDisplayMode>> Function() getModes;
}