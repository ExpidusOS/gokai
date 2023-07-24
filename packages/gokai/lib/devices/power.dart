class GokaiPowerDevice {
  const GokaiPowerDevice({
    required this.id,
    required this.getCycleCount,
    required this.getLevel,
    required this.getIsCharging,
    required this.isIntegrated,
  });

  final String id;
  final Future<int> Function() getCycleCount;
  final Future<double> Function() getLevel;
  final Future<bool> Function() getIsCharging;
  final bool isIntegrated;

  Future<int> get cycleCount => getCycleCount();
  Future<double> get level => getLevel();
  Future<bool> get isCharging => getIsCharging();
}
