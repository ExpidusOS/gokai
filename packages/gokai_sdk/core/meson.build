libgokai_core_src = [
  'lib/application.cc',
  'lib/application-manifest.cc',
  'lib/context.cc',
  'lib/logging.cc',
  'lib/object.cc',
  'lib/object-arguments.cc',
  'lib/object-factory.cc',
  'lib/service.cc',
  'lib/graphics/rendering/egl/renderer.cc',
  'lib/graphics/rendering/gles2/renderer.cc',
  'lib/graphics/rendering/pixman/renderer.cc',
  'lib/graphics/rendering/vulkan/renderer.cc',
  'lib/graphics/renderer.cc',
  'lib/service-channel.cc',
  'lib/services/compositor.cc',
  'lib/services/display-manager.cc',
  'lib/services/package-manager.cc',
  'lib/services/window-manager.cc',
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
  requires: [
    'spdlog',
    'libuv',
    'yaml-cpp',
    'glm',
    'fmt',
    'glesv2',
    'egl',
  ])