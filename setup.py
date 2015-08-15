from distutils.core import setup, Extension
import subprocess

packages = [ 'wot' ]

gpgme_cflags = subprocess.check_output(['gpgme-config', '--cflags'],
                                       universal_newlines=True)

gpgme_ldflags = subprocess.check_output(['gpgme-config', '--libs'],
                                        universal_newlines=True)
split_ldflags = gpgme_ldflags.strip().split()
library_dirs = [ arg[2:] for arg in split_ldflags if arg[:2] == '-L' ]
libraries = [ arg[2:] for arg in split_ldflags if arg[:2] == '-l' ]

CFLAGS = [ gpgme_cflags.strip() ]

ext_gpgme = Extension(name='wot._gpgme',
                      sources=['wot/_gpgme.c'],
                      extra_compile_args=CFLAGS,
                      libraries=libraries,
                      library_dirs=library_dirs)

ext_modules = [ ext_gpgme ]

setup(
    name='wot',
    description='Visualise the PGP web of trust',
    packages=packages,
    ext_modules=ext_modules,
    license='GPLv3',
    version='0.1'
)
