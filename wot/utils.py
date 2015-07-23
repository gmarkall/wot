# This file is part of "wot"
#
# Copyright (C) 2015 Graham Markall
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the included LICENSE file for details.

import gnupg

class GPG(gnupg.GPG):
    '''
    GPG class with additional methods to be slightly more convenient for
    exploring a web of trust.
    '''

    def list_keys(self, *, secret=False, keyid=None):
        '''
        List the keys currently in the keyring. Returns a list of the keys,
        or a single key if keyid is specified.
        '''

        keys = super(GPG, self).list_keys(secret=secret)

        if keyid is not None:
            klen = len(keyid)
            for k in keys:
                if k['keyid'][-klen:] == keyid:
                    return k
            raise KeyError('Key with ID %s not found' % keyid)

        return keys

    def list_sigs_by_keyid(self, keyid):
        '''
        Return a list of the keyids of the keys that a key with
        keyid is signed by.
        '''
        # list_sigs takes multiple keys to return signatures for, so we need
        # to take the first result.
        sig_info = self.list_sigs(keyid)[0]
        sigs = [ v for _, v in sig_info['sigs'].items() ][0]
        sig_keyids = [ s['keyid'] for s in sigs ]
        return sig_keyids

    def get_uids(self, keyid):
        '''
        Return a list of the uids of the key with keyid
        '''
        key = self.list_keys(keyid=keyid)
        return key['uids']
