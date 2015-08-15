from wot.gpgme import Keyring
import pytest, shutil, tempfile

@pytest.fixture
def keyring_folder(request):
    tmpdir = tempfile.mkdtemp()
    def rm_tmpdir():
        shutil.rmtree(tmpdir)
    request.addfinalizer(rm_tmpdir)
    return tmpdir

def test_keyring_ctor_noarg():
    with pytest.raises(TypeError):
        Keyring()

def test_keyring_ctor(keyring_folder):
    print(keyring_folder)
    Keyring(keyring_folder)
