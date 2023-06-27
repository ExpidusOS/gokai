libgokai_framework_src = []
libgokai_framework_deps = [gokai_core]
libgokai_framework_inc = include_directories('include')
libgokai_framework_cppflags = []

if target_machine.system() == 'linux'
  libgokai_framework_src += [
    'lib/os/linux/input/wayland/server/base.cc',
    'lib/os/linux/input/wayland/server/keyboard.cc',
    'lib/os/linux/input/wayland/server/pointer.cc',
    'lib/os/linux/input/wayland/server/touch.cc',
    'lib/os/linux/services/wayland/server/compositor.cc',
    'lib/os/linux/services/wayland/server/display-manager.cc',
    'lib/os/linux/services/wayland/server/input-manager.cc',
    'lib/os/linux/services/wayland/server/window-manager.cc',
    'lib/os/linux/services/package-manager.cc',
    'lib/os/linux/view/wayland/server/display.cc',
    'lib/os/linux/context.cc',
    'lib/os/linux/logging.cc',
    'lib/os/linux/object-factory.cc',
  ]

  libgokai_framework_deps += [
    dependency('libxdg-basedir'),
    dependency('packagekit-glib2'),
    dependency('pixman-1'),
    dependency('wlroots'),
    dependency('wayland-server'),
    dependency('libsystemd'),
    dependency('xkbcommon'),
  ]

  libgokai_framework_cppflags += [
    '-DWLR_USE_UNSTABLE=1'
  ]
endif

libgokai_framework = shared_library('gokai-framework-v1', libgokai_framework_src,
  cpp_args: libgokai_framework_cppflags,
  name_prefix: '',
  include_directories: libgokai_framework_inc,
  dependencies: libgokai_framework_deps,
  install: true,
  install_dir: libdir / 'gokai' / 'frameworks')
gokai_framework = declare_dependency(
  link_with: libgokai_framework,
  include_directories: libgokai_framework_inc,
  dependencies: libgokai_framework_deps)