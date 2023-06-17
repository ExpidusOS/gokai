libgokai_api_src = [
  'lib/binder.cc'
]
libgokai_api_deps = [gokai_core]
libgokai_api_inc = include_directories('include')

libgokai_api = library('gokai-api', libgokai_api_src,
  include_directories: libgokai_api_inc,
  dependencies: libgokai_api_deps,
  install: true)
gokai_api = declare_dependency(
  link_with: libgokai_api,
  include_directories: libgokai_api_inc,
  dependencies: libgokai_api_deps)