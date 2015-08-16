from wot._gpgme import Keyring
import pytest, shutil, tempfile

@pytest.fixture
def keyring_folder(request):
    tmpdir = tempfile.mkdtemp()
    def rm_tmpdir():
        shutil.rmtree(tmpdir)
    request.addfinalizer(rm_tmpdir)
    return tmpdir

# A way to get a distinct temp folder... Maybe there is a better way to get two
# of the same fixture?
@pytest.fixture
def keyring_folder_2(request):
    tmpdir = tempfile.mkdtemp()
    def rm_tmpdir():
        shutil.rmtree(tmpdir)
    request.addfinalizer(rm_tmpdir)
    return tmpdir


def test_keyring_ctor_noarg():
    with pytest.raises(TypeError):
        Keyring()

def test_keyring_ctor(keyring_folder):
    k = Keyring(keyring_folder)
    # Ensure the keyring path is the one we specified
    assert k.get_path() == keyring_folder
    # Best sanity check we can do for the engine file is to make sure it's
    # something that actually exists
    assert os.path.isfile(k.get_engine_file_name())

def test_two_keyrings_independence(keyring_folder, keyring_folder_2):
    k1 = Keyring(keyring_folder)
    k2 = Keyring(keyring_folder_2)
    assert k1.get_path() == keyring_folder
    assert k2.get_path() == keyring_folder_2
