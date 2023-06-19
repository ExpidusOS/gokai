project('gokai-sdk', 'cpp', license: 'GPL-3.0-only', version: '0.1.0-alpha',
  default_options: ['cpp_std=c++17'])

pkg = import('pkgconfig')

cpp = meson.get_compiler('cpp')

prefix = get_option('prefix')
bindir = prefix / get_option('bindir')
datadir = prefix / get_option('datadir')
libdir = prefix / get_option('libdir')
libexecdir = prefix / get_option('libexecdir')
includedir = prefix / get_option('includedir')

conf_data = configuration_data()
conf_data.set('LIBDIR', libdir)

flutter_engine_dir = get_option('flutter-engine')
if flutter_engine_dir == ''
  error('Must specify the Flutter Engine build with the "flutter-engine" option.')
endif

flutter_engine = cpp.find_library('flutter_engine',
  dirs: [flutter_engine_dir],
  has_headers: ['flutter_embedder.h'],
  header_include_directories: [include_directories(flutter_engine_dir)],
  required: true)

spdlog = dependency('spdlog')
libuv = dependency('libuv')
yaml_cpp = dependency('yaml-cpp')

subdir('core')
subdir('framework')
subdir('api')