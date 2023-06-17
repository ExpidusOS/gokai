libgokai_framework = library('gokai-framework', [
    'lib/context.cc',
  ],
  include_directories: include_directories('include'),
  dependencies: [gokai_core],
  install: true)
gokai_framework = declare_dependency(
  link_with: libgokai_framework,
  dependencies: [gokai_core],
  include_directories: include_directories('include'))