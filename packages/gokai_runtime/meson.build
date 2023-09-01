project('gokai-runtime', license: 'GPL-3.0-only', version: '0.1.0-alpha',
  meson_version: '>=1.1.0')

conf_data = configuration_data()
conf_data.set('path', join_paths(meson.project_source_root(), '..', '..'))
conf_data.set('branch', get_option('branch'))

configure_file(
  input: 'com.expidusos.gokai.Sdk.json.in',
  output: 'com.expidusos.gokai.Sdk.json',
  configuration: conf_data)
