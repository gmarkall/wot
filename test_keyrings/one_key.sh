#!/bin/bash
mkdir one_key
chmod 700 one_key
gpg2 --homedir=one_key --list-keys
gpg2 --homedir=one_key --keyserver pgp.mit.edu --recv-keys 8E1431D5
