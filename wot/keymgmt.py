'''
Data structures used to represent individual keys, subkeys, uids, etc. These are
modelled on the key management data structures in GPGME, but only the relevant
fields for the WOT application are implemented at present. See Section 7.5 of
the GPGME manual, "Key Management":

https://www.gnupg.org/documentation/manuals/gpgme/Key-Management.html
'''


class Key(object):
    '''
    A key, roughly analogous to gpgme_key_t.
    '''
    def __init__(self, keyid, uids, subkeys):
        self._keyid = keyid
        self._uids = uid
        self._subkeys = subkeys

    @property
    def keyid(self):
        return self._keyid

    @property
    def uids(self):
        return self._uids

    @property
    def subkeys(self):
        return self._subkeys


class SubKey(object):
    '''
    A subkey, roughly analogous to gpgme_subkey_t.
    '''
    def __init__(self, keyid):
        self._keyid = keyid

    @property
    def keyid(self):
        return self._keyid


class UserID(object):
    '''
    A user ID, roughly analogous to gpgme_user_id_t.
    '''
    def __init__(self, uid, name, email, signatures):
        self._uid = uid
        self._name = name
        self._email = email
        self._signatures = signatures

    @property
    def uid(self):
        return self._uid

    @property
    def name(self):
        return self._name

    @property
    def email(self):
        return self._email

    @property
    def signatures(self):
        return self._signatures


class Signature(object):
    '''
    A signature on a user ID, roughly analagous to a gpgme_key_sig_t
    '''
    def __init__(self, keyid):
        self._keyid = keyid

    @property
    def keyid(self):
        return self._keyid
