#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <structmember.h>
#include <gpgme.h>
#include <locale.h>
#include <stdbool.h>

/* Module-level variables */
static const char* gpgme_version;
static const char* protocol_name;
static gpgme_engine_info_t default_engine_info;
static bool setup = false;
static bool key_classes_set = false;
PyObject* key_class;
PyObject* subkey_class;
PyObject* uid_class;
PyObject* sig_class;

bool
setup_check(void) {
    if (!setup) {
        PyErr_SetString(PyExc_RuntimeError, "GPGME not set up yet");
        return false;
    }
    if (!key_classes_set) {
        PyErr_SetString(PyExc_RuntimeError, "Key classes not yet set");
        return false;
    }
    return true;
}

typedef struct KeyringObject {
    PyObject_HEAD
    gpgme_ctx_t ctx;
} KeyringObject;

static int
Keyring_traverse(KeyringObject* self, visitproc visit, void* arg) {
    return 0;
}

static void
Keyring_dealloc(KeyringObject* self) {
    gpgme_release(self->ctx);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static int
Keyring_init(KeyringObject* self, PyObject* args, PyObject* kwds) {
    if (!setup_check())
        return -1;

    PyObject *bytes;
    if (!PyArg_ParseTuple(args, "O&", PyUnicode_FSConverter, &bytes))
        return -1;

    char* keyring_path;
    int converr = PyBytes_AsStringAndSize(bytes, &keyring_path, NULL);
    if (converr == -1) {
        Py_DECREF(bytes);
        return -1;
    }

    /* Create and configure GPGME context */
    gpgme_error_t err;
    err = gpgme_new(&(self->ctx));
    if (err != GPG_ERR_NO_ERROR) {
        PyErr_SetString(PyExc_RuntimeError, "Error creating GPGME context");
        Py_DECREF(bytes);
        return -1;
    }

    err = gpgme_set_protocol(self->ctx, GPGME_PROTOCOL_OpenPGP);
    if (err != GPG_ERR_NO_ERROR) {
        PyErr_SetString(PyExc_RuntimeError, "Error setting GPGME protocol");
        Py_DECREF(bytes);
        return -1;
    }

    err = gpgme_ctx_set_engine_info(self->ctx, GPGME_PROTOCOL_OpenPGP,
              default_engine_info->file_name, keyring_path);
    if (err != GPG_ERR_NO_ERROR) {
        PyErr_SetString(PyExc_RuntimeError,
            "Error setting GPGME context engine info");
        Py_DECREF(bytes);
        return -1;
    }

    Py_DECREF(bytes);
    return 0;
}

PyObject*
Keyring_get_engine_file_name(KeyringObject* self, PyObject* args) {
    gpgme_engine_info_t info = gpgme_ctx_get_engine_info(self->ctx);
    return PyUnicode_FromString(info->file_name);
}

PyObject*
Keyring_get_path(KeyringObject* self, PyObject* args) {
    gpgme_engine_info_t info = gpgme_ctx_get_engine_info(self->ctx);
    return PyUnicode_FromString(info->home_dir);
}

PyObject*
Keyring_list_keys(KeyringObject* self, PyObject* args) {
    const char* keyid = NULL;
    if (!PyArg_ParseTuple(args, "|s", &keyid))
        return NULL;

    /* Change context's mode to list local keys only */
    gpgme_error_t err;
    err = gpgme_set_keylist_mode(self->ctx, GPGME_KEYLIST_MODE_LOCAL);
    if (err != GPG_ERR_NO_ERROR) {
        PyErr_SetString(PyExc_RuntimeError, "Unable to set keylist mode");
        return NULL;
    }

    /* List keys and iterate over them */
    err = gpgme_op_keylist_start(self->ctx, keyid, 0);
    if (err != GPG_ERR_NO_ERROR) {
        PyErr_SetString(PyExc_RuntimeError, "Error starting keylist operation");
        return NULL;
    }

    PyObject *keylist = PyList_New(0);
    if (!keylist)
        return NULL;

    gpgme_key_t key;
    while ((err = gpgme_op_keylist_next(self->ctx, &key)) == GPG_ERR_NO_ERROR) {
        PyObject* name = PyUnicode_FromString(key->uids->name);
        gpgme_key_release(key);

        if (PyList_Append(keylist, name) != 0) {
            Py_DECREF(keylist);
            Py_DECREF(name);
            return NULL;
        }
        Py_DECREF(name);
    }

    if (gpg_err_code(err) != GPG_ERR_EOF) {
        PyErr_SetString(PyExc_RuntimeError, "Error during keylist operation");
        Py_DECREF(keylist);
        return NULL;
    }

    return keylist;
}

static PyMethodDef Keyring_methods[] = {
    { "get_engine_file_name", (PyCFunction)Keyring_get_engine_file_name,
      METH_NOARGS, NULL },
    { "get_path", (PyCFunction)Keyring_get_path,
      METH_NOARGS, NULL },
    { "list_keys", (PyCFunction)Keyring_list_keys, METH_VARARGS, NULL },
    { NULL }
};

static PyMemberDef Keyring_members[] = {
    { NULL }
};

static PyTypeObject KeyringType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_gpgme.Keyring",                            /* tp_name */
    sizeof(KeyringObject),                       /* tp_basicsize */
    0,                                           /* tp_itemsize */
    (destructor)Keyring_dealloc,                 /* tp_dealloc */
    0,                                           /* tp_print */
    0,                                           /* tp_getattr */
    0,                                           /* tp_setattr */
    0,                                           /* tp_compare */
    0,                                           /* tp_repr */
    0,                                           /* tp_as_number */
    0,                                           /* tp_as_sequence */
    0,                                           /* tp_as_mapping */
    0,                                           /* tp_hash */
    0,                                           /* tp_call*/
    0,                                           /* tp_str*/
    0,                                           /* tp_getattro*/
    0,                                           /* tp_setattro*/
    0,                                           /* tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE
                       | Py_TPFLAGS_HAVE_GC,     /* tp_flags*/
    "Keyring object",                            /* tp_doc */
    (traverseproc)Keyring_traverse,              /* tp_traverse */
    0,                                           /* tp_clear */
    0,                                           /* tp_richcompare */
    0,                                           /* tp_weaklistoffset */
    0,                                           /* tp_iter */
    0,                                           /* tp_iternext */
    Keyring_methods,                             /* tp_methods */
    Keyring_members,                             /* tp_members */
    0,                                           /* tp_getset */
    0,                                           /* tp_base */
    0,                                           /* tp_dict */
    0,                                           /* tp_descr_get */
    0,                                           /* tp_descr_set */
    0,                                           /* tp_dictoffset */
    (initproc)Keyring_init,                      /* tp_init */
    0,                                           /* tp_alloc */
    0,                                           /* tp_new */
};

PyObject*
get_gpgme_version(void) {
    if (!setup_check())
        return NULL;
    return PyUnicode_FromString(gpgme_version);
}

PyObject*
get_protocol_name(void) {
    if (!setup_check())
        return NULL;
    return PyUnicode_FromString(protocol_name);
}

PyObject*
gpgme_setup(void) {
    if (setup) {
        PyErr_SetString(PyExc_RuntimeError, "GPGME already set up");
        return NULL;
    }
    gpgme_error_t err;

    /* Retrieve GPGME version */
    gpgme_version = gpgme_check_version(NULL);

    /* Set locale */
    gpgme_set_locale(NULL, LC_CTYPE, setlocale (LC_CTYPE, NULL));

    /* Check for OpenPGP support */
    err = gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP);
    if(err != GPG_ERR_NO_ERROR) {
        PyErr_SetString(PyExc_RuntimeError,
                        "gpgme_engine_check_version failed");
        return NULL;
    }

    protocol_name = gpgme_get_protocol_name(GPGME_PROTOCOL_OpenPGP);

    /* Get engine info defaults */
    err = gpgme_get_engine_info(&default_engine_info);
    if(err != GPG_ERR_NO_ERROR) {
        PyErr_SetString(PyExc_RuntimeError,
                        "gpgme_get_engine_info failed");
        return NULL;
    }

    setup = true;
    Py_RETURN_NONE;
}

PyObject*
set_key_classes(PyObject* self, PyObject* args) {
    if (key_classes_set) {
        PyErr_SetString(PyExc_RuntimeError, "Key classes already set");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, "OOOO", &key_class, &subkey_class, &uid_class,
            &sig_class))
        return NULL;

    Py_INCREF(key_class);
    Py_INCREF(subkey_class);
    Py_INCREF(uid_class);
    Py_INCREF(sig_class);

    key_classes_set = true;
    Py_RETURN_NONE;
}

static PyMethodDef ext_methods[] = {
    { "get_gpgme_version", (PyCFunction)get_gpgme_version, METH_NOARGS, NULL },
    { "get_protocol_name", (PyCFunction)get_protocol_name, METH_NOARGS, NULL },
    { "gpgme_setup", (PyCFunction)gpgme_setup, METH_NOARGS, NULL },
    { "set_key_classes", (PyCFunction)set_key_classes, METH_VARARGS, NULL },
    { NULL }
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT, "_gpgme", "No docs", -1, ext_methods
};

PyMODINIT_FUNC
PyInit__gpgme(void) {
    PyObject *m = PyModule_Create(&moduledef);
    if (m == NULL)
        return NULL;


    KeyringType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&KeyringType) < 0)
        return NULL;
    Py_INCREF(&KeyringType);
    PyModule_AddObject(m, "Keyring", (PyObject*)(&KeyringType));

    return m;
}
