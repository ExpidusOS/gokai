class GokaiPowerDevice {
  const GokaiPowerDevice({
    required this.id,
    required this.getCycleCount,
    required this.getLevel,
    required this.isIntegrated,
  });

  final String id;
  final Future<int> Function() getCycleCount;
  final Future<double> Function() getLevel;
  final bool isIntegrated;

  Future<int> get cycleCount => getCycleCount();
  Future<double> get level => getLevel();
}
