project('gokai-sdk', 'cpp', license: 'GPL-3.0-only', version: '0.1.0-alpha',
  default_options: ['cpp_std=c++17'])

pkg = import('pkgconfig')

prefix = get_option('prefix')
bindir = prefix / get_option('bindir')
datadir = prefix / get_option('datadir')
libdir = prefix / get_option('libdir')
libexecdir = prefix / get_option('libexecdir')
includedir = prefix / get_option('includedir')

conf_data = configuration_data()
conf_data.set('LIBDIR', libdir)

spdlog = dependency('spdlog')
libuv = dependency('libuv')

subdir('core')
subdir('framework')
subdir('api')