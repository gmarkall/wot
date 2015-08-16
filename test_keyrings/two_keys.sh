#!/bin/bash
mkdir two_keys
chmod 700 two_keys
gpg2 --homedir=two_keys --list-keys
gpg2 --homedir=two_keys --keyserver pgp.mit.edu --recv-keys 8E1431D5
gpg2 --homedir=two_keys --keyserver pgp.mit.edu --recv-keys 67C6FAA2
