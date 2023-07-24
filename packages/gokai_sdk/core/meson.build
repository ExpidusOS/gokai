libgokai_core_src = [
  'lib/application.cc',
  'lib/application-manifest.cc',
  'lib/casting.cc',
  'lib/context.cc',
  'lib/logging.cc',
  'lib/object.cc',
  'lib/object-arguments.cc',
  'lib/object-factory.cc',
  'lib/plugin.cc',
  'lib/plugin-module.cc',
  'lib/service.cc',
  'lib/service-channel.cc',
  'lib/flutter/codecs/json.cc',
  'lib/flutter/codecs/std.cc',
  'lib/flutter/engine.cc',
  'lib/flutter/message-codec.cc',
  'lib/flutter/method-call.cc',
  'lib/flutter/method-channel.cc',
  'lib/flutter/method-codec.cc',
  'lib/flutter/text-input-model.cc',
  'lib/flutter/text-range.cc',
  'lib/graphics/rendering/cairo/renderer.cc',
  'lib/graphics/rendering/egl/renderer.cc',
  'lib/graphics/rendering/gles2/renderer.cc',
  'lib/graphics/rendering/vulkan/renderer.cc',
  'lib/graphics/compositor.cc',
  'lib/graphics/renderer.cc',
  'lib/graphics/swap-chain.cc',
  'lib/graphics/texture.cc',
  'lib/input/base.cc',
  'lib/input/keyboard.cc',
  'lib/input/pointer.cc',
  'lib/input/switch.cc',
  'lib/input/touch.cc',
  'lib/memory/buffer.cc',
  'lib/memory/read-buffer.cc',
  'lib/memory/write-buffer.cc',
  'lib/os/dynamic-library.cc',
  'lib/services/account-manager.cc',
  'lib/services/compositor.cc',
  'lib/services/compositor-input-method.cc',
  'lib/services/display-manager.cc',
  'lib/services/engine-manager.cc',
  'lib/services/input-manager.cc',
  'lib/services/package-manager.cc',
  'lib/services/plugin-manager.cc',
  'lib/services/power-manager.cc',
  'lib/services/texture-manager.cc',
  'lib/services/window-manager.cc',
  'lib/user/account.cc',
  'lib/user/id.cc',
  'lib/view/cairo/image.cc',
  'lib/view/display.cc',
  'lib/view/displayable.cc',
  'lib/view/rect.cc',
  'lib/view/window.cc',
]
libgokai_core_inc = [
  include_directories('include'),
  include_directories(flutter_engine_dir),
]
libgokai_core_deps = [
  libuv,
  spdlog,
  yaml_cpp,
  glm,
  fmt,
  flutter_engine,
  gles2,
  egl,
  vulkan,
  cairo,
  crossguid,
  jsoncpp,
]
libgokai_core_headers_exclude = []

if target_machine.system() == 'linux'
  libgokai_core_src += [
    'lib/os/linux/dynamic-library.cc',
    'lib/os/linux/paths.cc'
  ]

  libgokai_core_inc += [
    include_directories('include-linux')
  ]

  install_subdir('include-linux/gokai',
    strip_directory: true,
    install_dir: includedir / 'gokai')
endif

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

pkg.generate(libgokai_core,
  libraries: [
    flutter_engine,
  ],
  extra_cflags: [
    '-I'+flutter_engine_dir
  ],
  requires: [
    'spdlog',
    'libuv',
    'yaml-cpp',
    'glm',
    'fmt',
    'glesv2',
    'egl',
    'crossguid',
    'jsoncpp',
  ])
