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

lak_subprj = subproject('lak')
c_args = lak_subprj.get_variable('c_args')
cpp_args = lak_subprj.get_variable('cpp_args')
c_cpp_args = lak_subprj.get_variable('c_cpp_args')
version = lak_subprj.get_variable('version')

add_project_arguments(c_cpp_args + cpp_args, language: ['cpp'])
add_project_arguments(c_cpp_args + c_args, language: ['c'])

subdir('src')

executable(
	'lisktest',
	'example/main.cpp',
	override_options: 'cpp_std=' + version,
	dependencies: [
		dependency('threads'),
		lisk_dep,
	],
)
