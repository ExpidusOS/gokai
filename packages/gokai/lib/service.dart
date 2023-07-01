class GokaiService {
  const GokaiService({
    required this.serviceName,
    String? channelName,
  }) : channelName = channelName ?? 'Gokai::Services::$serviceName';

  final String serviceName;
  final String channelName;
}