libgokai_api_conf = configuration_data()
libgokai_api_conf.merge_from(conf_data)

libgokai_api_src = [
  'lib/binder.cc',
  'lib/binder-manager.cc'
]
libgokai_api_deps = [gokai_core]
libgokai_api_inc = include_directories('include')

if target_machine.system() == 'linux'
  libgokai_api_src += [
    'lib/os/linux/binder.cc',
    'lib/os/linux/binder-manager.cc'
  ]
endif

libgokai_api_build_h = configure_file(
  input: 'lib/build.h.in',
  output: 'gokai-api-build.h',
  configuration: libgokai_api_conf)

libgokai_api = library('gokai-api', libgokai_api_src,
  include_directories: libgokai_api_inc,
  dependencies: libgokai_api_deps,
  install: true)
gokai_api = declare_dependency(
  link_with: libgokai_api,
  include_directories: libgokai_api_inc,
  dependencies: libgokai_api_deps)

pkg.generate(libgokai_api,
  requires: ['gokai-core', 'libuv'])