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

# This was supposed to work but the GPG interface does something odd that I didn't
# understand, so every set of signatures was the same.

#    def to_dict(self):
#        keys = self.list_keys()
#        keyid = []
#        signedby = []
#        uid = []
#        trust = []
#        for key in keys:
#            keyidstr = key['keyid']
#            keyid.append(keyidstr)
#            signedby.append(self.list_sigs_by_keyid(keyidstr))
#            uidlist = self.get_uids(keyidstr)
#            uidstr = "\n".join(uidlist)
#            uid.append(uidstr)
#            trust.append(0)
#
#        return {'keyid': keyid, 'signedby': signedby, 'uid': uid, 'trust': trust}


# This appears to work, but is a monster hack.

    def to_dict(self):
        # we want the key ids of all keys that are signed, but we get
        # the uids of all keys that are signed.
        # try to turn those signed uids back into keyids...
        res = self.list_sigs()
        signed_uids = list(res.sigs)
        keys = self.list_keys()
        uid_to_keyid_map = {}
        for suid in signed_uids:
            for key in keys:
                uids = key['uids']
                for uid in uids:
                    if uid == suid:
                        signed_keyid = key['keyid']
                        uid_to_keyid_map.update({uid: signed_keyid})

        # Now we have the mapping between uids and keyids, make a keyid-only dict of
        # signatures
        sig_keyid_dict = {}
        for k, v in res.sigs.items():
            sig_keyid_dict[uid_to_keyid_map[k]] = (k, v)

        # Now we have a dict of signatures with all keyids, create a list of all
        # unique keyids
        keyid_set = set()
        for k, v in sig_keyid_dict.items():
            keyid_set.add(k)
            for skeyid in v[1]:
                keyid_set.add(skeyid)

        all_keyids = list(keyid_set)

        # Now we have all the keyids, we should be able to compute who
        # signed each key
        signedby = []
        for keyid in all_keyids:
            this_signedby = []
            try:
                sig_keyids = sig_keyid_dict[keyid][1]
                for sig_key in sig_keyids:
                    this_signedby.append(all_keyids.index(sig_key))
            except KeyError:
                # Not everything is known to be signed by anything, so we will
                # get key errors for them
                pass
            signedby.append(this_signedby)

        uids = []
        for keyid in all_keyids:
            try:
                uidlist = self.get_uids(keyid)
                uidstr = "\n".join(uidlist)
                uids.append(uidstr)
            except KeyError:
                # Not every key is downloaded, so we will get key errors.
                # We have no uid for these
                uids.append(None)

        return {'keyid': all_keyids, 'signedby': signedby, 'uid': uids}

