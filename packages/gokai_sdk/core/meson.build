libgokai_core_src = [
  'lib/context.cc',
  'lib/object.cc',
  'lib/object-arguments.cc',
  'lib/object-factory.cc',
  'lib/service.cc',
  'lib/service-channel.cc',
  'lib/value.cc',
  'lib/services/compositor.cc',
  'lib/services/display-manager.cc',
  'lib/services/package-manager.cc',
  'lib/services/window-manager.cc',
  'lib/values/bool.cc',
  'lib/values/int.cc',
  'lib/values/pointer.cc',
  'lib/values/string.cc',
]
libgokai_core_inc = include_directories('include')
libgokai_core_deps = [
  dependency('libuv')
]
libgokai_core_headers_exclude = []

libgokai_core = library('gokai-core', libgokai_core_src,
  include_directories: libgokai_core_inc,
  dependencies: libgokai_core_deps,
  install: true)
gokai_core = declare_dependency(
  link_with: libgokai_core,
  include_directories: libgokai_core_inc,
  dependencies: libgokai_core_deps)

install_subdir('include/gokai',
  exclude_directories: libgokai_core_headers_exclude,
  strip_directory: true,
  install_dir: includedir / 'gokai')

pkg.generate(libgokai_core)