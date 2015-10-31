from . import _gpgme

_gpgme.gpgme_setup()
_gpgme_version = _gpgme.get_gpgme_version()

if _gpgme_version != '1.5.5':
    raise RuntimeError('GPGME version 1.5.5 required (got %s)' % _gpgme_version)



class Keyring(_gpgme.Keyring):
    def __init__(self, keyring_path):
        super().__init__(keyring_path)
        self._dict = {'keyid': [], 'signedby': [], uid: [], 'timestamp': []}

    def to_dict(self):
        '''
        Convert to a dict. The dict contains lists of the key IDs, the lists of
        key IDs signing the key, the uid, and the timestamp.

        FIXME: Signatures are not yet shown.
        FIXME: Since importing is not yet implemented, timestamps will all be
               the same (the time of the first call).
        '''
        keys = self.list_keys()
        new_keys = set(self._dict['keyid']
        ### Got this far then realised that the list_keys operation takes keyids
        ### and returns uids, which is not ideal for this function.



