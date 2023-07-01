enum GokaiContextMode {
  invalid(-1),
  client(0),
  compositor(1);

  const GokaiContextMode(this.id);

  final int id;
}