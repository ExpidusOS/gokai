import 'id.dart';

class GokaiUserAccount {
  const GokaiUserAccount({
    required this.id,
    required this.language,
    required this.displayName,
    this.picture,
    this.home,
    required this.isSystem,
    required this.isAdministrator,
  });

  final GokaiUserID id;
  final String language;
  final String displayName;
  final String? picture;
  final String? home;
  final bool isSystem;
  final bool isAdministrator;
}
