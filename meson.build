project(
	'lisk-test',
	['c', 'cpp'],
	default_options: {
		'cpp_std': ['c++26', 'c++23', 'c++latest'],
		'b_vscrt': 'static_from_buildtype',
		'warning_level': '3',
		'werror': false,
	},
)

cxx = meson.get_compiler('cpp')

if cxx.get_id() == 'msvc'
	add_project_arguments(
		'-DUNICODE',
		'-DWIN32_LEAN_AND_MEAN',
		'-DNOMINMAX',
		'-wd4315',
		'-wd4366',
		'-wd4250',
		'-Wv:18',
		language: ['c', 'cpp'])

	add_project_arguments(
		'-Zc:__cplusplus',
		'-Zc:rvalueCast',
		'-Zc:wchar_t',
		'-Zc:ternary',
		'-Zc:preprocessor',
		language: ['cpp'])

	add_project_link_arguments(
		'Winmm.lib',
		'Setupapi.lib',
		'Version.lib',
		'Imm32.lib',
		language: ['c', 'cpp'])
else
	add_project_arguments(
		'-Wno-unknown-warning-option',
		'-Wno-gnu-zero-variadic-macro-arguments',
		'-Wno-language-extension-token',
		'-Wno-dangling-else',
		'-Wno-microsoft-cast',
		'-Wno-pragma-pack',
		'-Wno-deprecated-declarations',
		'-Wno-unused-private-field',
		'-Wno-unused-variable',
		'-Wno-init-list-lifetime',
		'-Wno-invalid-partial-specialization',
		'-Wno-missing-field-initializers',
		'-Wno-maybe-uninitialized',
		'-Wno-undefined-inline',
		'-Wfatal-errors',
		'-Wno-psabi',
		# '-mavx',
		language: ['c', 'cpp', 'objcpp'])

	if cxx.get_id() == 'gcc'
		if cxx.has_link_argument('-Wl,-lstdc++_libbacktrace')
			# GCC <14
			add_project_link_arguments(
				'-Wl,-lstdc++_libbacktrace',
				language: ['cpp'])
		elif cxx.has_link_argument('-Wl,-lstdc++exp')
			# GCC >=14
			add_project_link_arguments(
				'-Wl,-lstdc++exp',
				language: ['cpp'])
		else
			warning('Unable to determine backtrace library link argument')
		endif
	endif
endif

subdir('src')

executable(
	'lisktest',
	'example/main.cpp',
	dependencies: [
		dependency('threads'),
		lisk_dep,
	],
)
