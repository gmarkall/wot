from . import _gpgme

_gpgme_version = _gpgme.get_gpgme_version()

if _gpgme_version != '1.5.5':
    raise RuntimeError('GPGME version 1.5.5 required (got %s)' % _gpgme_version)


class Keyring(_gpgme.Keyring):
    def __init__(self, keyring_path):
        super().__init__(keyring_path)
