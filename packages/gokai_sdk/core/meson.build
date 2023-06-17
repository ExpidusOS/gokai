libgokai_core = library('gokai-core', [
    'lib/context.cc',
    'lib/service.cc',
    'lib/services/display-manager.cc',
    'lib/services/window-manager.cc',
  ],
  include_directories: include_directories('include'),
  install: true)
gokai_core = declare_dependency(
  link_with: libgokai_core,
  include_directories: include_directories('include'))