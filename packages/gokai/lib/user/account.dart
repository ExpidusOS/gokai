import 'id.dart';

class GokaiUserAccount {
  const GokaiUserAccount({
    required this.id,
    required this.language,
    required this.displayName,
    this.picture,
    required this.isSystem,
    required this.isAdministrator,
  });

  final GokaiUserID id;
  final String language;
  final String displayName;
  final String? picture;
  final bool isSystem;
  final bool isAdministrator;
}
