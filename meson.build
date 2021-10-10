project(
  'lisk-test',
  ['c', 'cpp'],
  default_options: [
    'warning_level=3',
    'werror=true',
    'b_vscrt=static_from_buildtype',
    # 'b_sanitize=address'
  ],
)

cxx = meson.get_compiler('cpp')

if cxx.get_id() == 'msvc'

  version = 'c++latest'

  project_args = [
    '-Zc:__cplusplus',
    '-Zc:rvalueCast',
    '-Zc:wchar_t',
    '-Zc:ternary',
    '-DUNICODE',
    '-DWIN32_LEAN_AND_MEAN',
    '-DNOMINMAX',
    '-wd4315',
    '-wd4366',
  ]

  cplusplus = cxx.get_define(
    '__cplusplus',
    args: project_args + ['-std:' + version],
  )
else
  version = 'c++20'

  project_args = [
    '-Wfatal-errors',
  ]

  libs = []

  cplusplus = cxx.get_define(
    '__cplusplus',
    args: project_args + ['-std=' + version],
  )
endif

if cxx.compute_int(cplusplus) < 202002
  error('C++ version insufficient, expected 202002L got ' + cplusplus)
endif

add_project_arguments(project_args, language: ['c', 'cpp'])

subdir('src')

executable(
  'lisktest',
  'example/main.cpp',
  override_options: 'cpp_std=' + version,
  include_directories: include_directories('inc'),
  dependencies: dependency('threads'),
  link_with: lisk,
)
