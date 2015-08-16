# Tests for the initialisation of the _gpgme module. These are kept separate as
# it's not really possible to uninitialise and reinitialise the module in order
# to test initialisation alongside other tests

from wot import _gpgme
import pytest


def test_initialisation():
    with pytest.raises(RuntimeError):
        _gpgme.get_gpgme_version()

    with pytest.raises(RuntimeError):
        _gpgme.get_protocol_name()

    _gpgme.gpgme_setup()

    assert _gpgme.get_gpgme_version() == '1.5.5'
    assert _gpgme.get_protocol_name() == 'OpenPGP'

    with pytest.raises(RuntimeError):
        _gpgme.gpgme_setup()

if __name__ == '__main__':
    pytest.main()
