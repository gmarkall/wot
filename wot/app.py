from utils import GPG

gpg = GPG()
gpg.keyserver = 'hkp://pgp.mit.edu'

fedora_key = '8E1431D5'
expected_fingerprint = 'C527EA07A9349B589C35E1BF11ADC0948E1431D5'

print('Attempting to receive key %s' % fedora_key)
import_result = gpg.recv_keys('8E1431D5')

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

# Now let's suppose we want to explore the first key we received

