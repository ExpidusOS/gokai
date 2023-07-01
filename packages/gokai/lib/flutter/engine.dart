import 'package:gokai/constants.dart';

class GokaiFlutterEngine {
  const GokaiFlutterEngine({
    required this.id,
    required this.viewType,
    required this.viewName,
  });

  final String id;
  final GokaiFlutterEngineViewType viewType;
  final String viewName;
}