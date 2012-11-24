# Woah a scons build file
# Checkout scons.sourceforge.net for details
common_sources = [
					'common.c',
					'linkproto_stub.c',
					'linkproto_core.c'
				]

common_objects = [
					'common.o',
					'linkproto_stub.o',
					'linkproto_core.o'
				]
	
batch_io_sources = [
					'batch.c'
				]

pksh_sources = [
				'rl_common.c',
				'pksh.c',
				'netfsproto_core.c'
				]
pksh_libs = [
				'readline',
				'curses',
				'ncurses'
			]

Object(common_sources)

# pksh
Program('pksh', common_objects + pksh_sources, LIBS = pksh_libs, LIBPATH = '/usr/local/lib')
# eeexec
Program('eeexec', common_objects + batch_io_sources + ['eeexec.c'])
# iopexec
Program('iopexec', common_objects + batch_io_sources + ['iopexec.c'])
# reset
Program('reset', common_objects + batch_io_sources + ['reset.c'])
# dumpmem
Program('dumpmem', common_objects + batch_io_sources + ['dumpmem.c'])
# dumpreg
Program('dumpreg', common_objects + batch_io_sources + ['dumpreg.c'])
# viewmem
Program('viewmem', common_objects + batch_io_sources + ['viewmem.c'])
# gsexec
Program('gsexec', common_objects + batch_io_sources + ['gsexec.c'])
