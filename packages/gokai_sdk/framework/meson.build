libgokai_framework_src = []
libgokai_framework_deps = [gokai_core]
libgokai_framework_inc = include_directories('include')

if target_machine.system() == 'linux'
  libgokai_framework_src += [
    'lib/os/linux/services/package-manager.cc',
    'lib/os/linux/context.cc'
  ]

  libgokai_framework_deps += [
    dependency('appstream'),
    dependency('libxdg-basedir'),
    dependency('packagekit-glib2'),
  ]
endif

libgokai_framework = library('gokai-framework', libgokai_framework_src,
  include_directories: libgokai_framework_inc,
  dependencies: libgokai_framework_deps,
  install: true)
gokai_framework = declare_dependency(
  link_with: libgokai_framework,
  include_directories: libgokai_framework_inc,
  dependencies: libgokai_framework_deps)