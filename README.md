Web of trust exploration
========================

To execute:

```
$ cd wot
$ python app.py
```

Output should match (roughly) the example given in output.txt.


Notes
-----

The utils module contains a subclass of `gnupg.GPG` which has a couple of extra
methods to make the API a little more convenient for exploring a web of trust.

The app.py file is an example that uses the GPG class to first receive the
fedora key from a key server, and show the uids. It then lists the signatures
and attempts to import all the keys that signed the fedora key.

The first key that successfully imported is printed out along with its uid.

