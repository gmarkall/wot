from distutils.core import setup, Extension

packages = [ 'wot' ]

CFLAGS = []
LDFLAGS = []

ext_gpgme = Extension(name='wot._gpgme',
                      sources=['wot/_gpgme.c'],
                      extra_compile_args=CFLAGS,
                      extra_link_args=LDFLAGS)

ext_modules = [ ext_gpgme ]

setup(
    name='wot',
    description='Visualise the PGP web of trust',
    packages=packages,
    ext_modules=ext_modules,
    license='GPLv3',
    version='0.1'
)
