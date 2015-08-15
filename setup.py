from distutils.core import setup, Extension
import subprocess

packages = [ 'wot' ]

gpgme_cflags = subprocess.check_output(['gpgme-config', '--cflags'],
                                       universal_newlines=True)
gpgme_ldflags = subprocess.check_output(['gpgme-config', '--libs'],
                                        universal_newlines=True)

print(gpgme_cflags.strip())
print(gpgme_ldflags.strip())

CFLAGS = [ gpgme_cflags.strip() ]
LDFLAGS = [ gpgme_ldflags.strip() ]

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
