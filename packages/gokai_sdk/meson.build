project('gokai-sdk', 'c', 'cpp', license: 'GPL-3.0-only', version: '0.1.0-alpha',
  default_options: ['cpp_std=c++17'],
  meson_version: '>=1.1.0')

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

if target_machine.system() == 'linux' or target_machine.system() == 'android'
  libprefix = 'lib'
  libsuffix = '.so'
elif target_machine.system() == 'windows'
  libprefix = ''
  libsuffix = '.dll'
elif target_machine.system() == 'darwin'
  libprefix = ''
  libsuffix = '.dylib'
endif

install_headers(flutter_engine_dir / 'flutter_embedder.h')
install_data(flutter_engine_dir / 'icudtl.dat',
  install_dir: libdir / 'gokai')
install_data(flutter_engine_dir / '@0@flutter_engine@1@'.format(libprefix, libsuffix),
  install_mode: 'rwxr-xr-x',
  install_dir: libdir)
install_data(flutter_engine_dir / 'gen_snapshot',
  install_mode: 'rwxr-xr-x',
  install_dir: libexecdir / 'gokai')

spdlog = dependency('spdlog')
libuv = dependency('libuv')
yaml_cpp = dependency('yaml-cpp')
glm = dependency('glm')
fmt = dependency('fmt')
gles2 = dependency('glesv2')
egl = dependency('egl')
vulkan = dependency('vulkan')
cairo = dependency('cairo')
jsoncpp = dependency('jsoncpp')
crossguid = [dependency('crossguid')]

if target_machine.system() == 'linux'
  crossguid += dependency('uuid')
endif

subdir('core')
subdir('framework')
subdir('api')
