lak_subprj = subproject('lak')
lak_dep = lak_subprj.get_variable('lak_dep')

lisk = static_library(
	'lisk',
	[
		'atom.cpp',
		'callable.cpp',
		'environment.cpp',
		'eval.cpp',
		'expression.cpp',
		'functor.cpp',
		'lambda.cpp',
		'lisk.cpp',
		'number.cpp',
		'pointer.cpp',
		'string.cpp',
	],
	override_options: 'cpp_std=' + version,
	include_directories: include_directories('../include'),
	dependencies: lak_dep,
)

lisk_dep = declare_dependency(
	link_with: lisk,
	include_directories: include_directories('../include'),
	dependencies: lak_dep,
)
