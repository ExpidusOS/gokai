libgokai_core_src = [
  'lib/application.cc',
  'lib/application-manifest.cc',
  'lib/context.cc',
  'lib/logging.cc',
  'lib/object.cc',
  'lib/object-arguments.cc',
  'lib/object-factory.cc',
  'lib/service.cc',
  'lib/service-channel.cc',
  'lib/flutter/codecs/json.cc',
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
  'lib/graphics/renderer.cc',
  'lib/input/base.cc',
  'lib/input/keyboard.cc',
  'lib/input/pointer.cc',
  'lib/input/touch.cc',
  'lib/services/compositor.cc',
  'lib/services/compositor-input-method.cc',
  'lib/services/display-manager.cc',
  'lib/services/engine-manager.cc',
  'lib/services/input-manager.cc',
  'lib/services/package-manager.cc',
  'lib/services/window-manager.cc',
  'lib/view/cairo/image.cc',
  'lib/view/display.cc',
  'lib/view/displayable.cc',
  'lib/view/rect.cc',
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