libgokai_framework_src = []
libgokai_framework_deps = [gokai_core]
libgokai_framework_inc = [include_directories('include')]
libgokai_framework_private_inc = []
libgokai_framework_cppflags = []

if target_machine.system() == 'linux'
  libgokai_framework_src += [
    'lib/os/linux/devices/power.cc',
    'lib/os/linux/graphics/rendering/egl/wayland/server/renderer.cc',
    'lib/os/linux/graphics/wayland/server/texture.cc',
    'lib/os/linux/input/wayland/server/base.cc',
    'lib/os/linux/input/wayland/server/keyboard.cc',
    'lib/os/linux/input/wayland/server/pointer.cc',
    'lib/os/linux/input/wayland/server/switch.cc',
    'lib/os/linux/input/wayland/server/touch.cc',
    'lib/os/linux/input/base.cc',
    'lib/os/linux/services/wayland/server/compositor.cc',
    'lib/os/linux/services/wayland/server/compositor-input-method.cc',
    'lib/os/linux/services/wayland/server/display-manager.cc',
    'lib/os/linux/services/wayland/server/input-manager.cc',
    'lib/os/linux/services/wayland/server/window-manager.cc',
    'lib/os/linux/services/account-manager.cc',
    'lib/os/linux/services/input-manager.cc',
    'lib/os/linux/services/package-manager.cc',
    'lib/os/linux/services/power-manager.cc',
    'lib/os/linux/user/account.cc',
    'lib/os/linux/view/wayland/server/egl/buffer.cc',
    'lib/os/linux/view/wayland/server/display.cc',
    'lib/os/linux/view/wayland/server/window.cc',
    'lib/os/linux/wm/wayland/server/xdg-decoration.cc',
    'lib/os/linux/wm/wayland/server/xdg-surface.cc',
    'lib/os/linux/context.cc',
    'lib/os/linux/logging.cc',
    'lib/os/linux/object-factory.cc',
  ]

  libgokai_framework_deps += [
    dependency('accountsservice'),
    dependency('libxdg-basedir'),
    dependency('packagekit-glib2'),
    dependency('pixman-1'),
    dependency('wlroots'),
    dependency('wayland-server'),
    dependency('libsystemd'),
    dependency('xkbcommon'),
    dependency('libdrm'),
    dependency('libevdev'),
    dependency('upower-glib'),
  ]

  libgokai_framework_cppflags += [
    '-DWLR_USE_UNSTABLE=1'
  ]

  libgokai_framework_private_inc += [
    include_directories('lib/os/linux/services/wayland/server')
  ]

  wayland = import('unstable-wayland')
  libgokai_framework_src += wayland.scan_xml(
    wayland.find_protocol('xdg-shell'),
    client: false,
    server: true,
    public: true
  )
endif

libgokai_framework = shared_library('gokai-framework-v1', libgokai_framework_src,
  cpp_args: libgokai_framework_cppflags,
  name_prefix: '',
  include_directories: libgokai_framework_inc + libgokai_framework_private_inc,
  dependencies: libgokai_framework_deps,
  install: true,
  install_dir: libdir / 'gokai' / 'frameworks')
gokai_framework = declare_dependency(
  link_with: libgokai_framework,
  include_directories: libgokai_framework_inc,
  dependencies: libgokai_framework_deps)
