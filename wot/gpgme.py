from . import _gpgme

class Keyring(_gpgme.Keyring):
    def __init__(self, keyring_path):
        super().__init__(keyring_path)
