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


from utils import GPG
import sys

gpg = GPG()
gpg.keyserver = 'hkp://pgp.mit.edu'

fedora_key = '8E1431D5'
expected_fingerprint = 'C527EA07A9349B589C35E1BF11ADC0948E1431D5'

print('Attempting to receive key %s' % fedora_key)
import_result = gpg.recv_keys('8E1431D5')

if not import_result:
    print('Error importing key %s. Cannot continue' % fedora_key)
    sys.exit(1)

# Ignore any other fingerprints (GRM: not sure when you would have more than
# one anwyay)
fp0 = import_result.fingerprints[0]

print('Received key %s with fingerprint %s' % (fp0[-8:], fp0))

print('Checking fingerprint matches expected')
if expected_fingerprint == fp0:
    print(' - OK')
else:
    print(' - ERROR: expected %s, got %s' % (expected_fingerprint, fp0))

# Display uids
uids = gpg.get_uids(fedora_key)
print('Key %s has uids:' % fedora_key)
for uid in uids:
    print(' - %s' % uid)

# Get the list of signatures of the fedora key.
sig_keyids = gpg.list_sigs_by_keyid(fedora_key)

# Now attempt to import each of those keys:
successful_imports = []
for keyid in sig_keyids:
    print('Importing key with ID %s' % keyid)
    import_result = gpg.recv_keys(keyid)
    if import_result:
        successful_imports.append(keyid)
    else:
        print('Key with ID %s received failed:' % keyid)
        print(import_result.stderr)

# Now let's suppose we want to explore the first key we received, to find out whose it is.
interesting_key = successful_imports[0]
import_result = gpg.recv_keys(interesting_key)

if not import_result:
    print('Error importing key %s. Cannot continue')
    sys.exit(1)

uids = gpg.get_uids(interesting_key)
print('Key %s has uids:')
for uid in uids:
    print(' - %s' % uid)
