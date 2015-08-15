#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <structmember.h>
#include <gpgme.h>

static const char* gpgme_version;

typedef struct KeyringObject {
    PyObject_HEAD
    char* keyring_path;
    Py_ssize_t keyring_path_len;
} KeyringObject;

static int
Keyring_traverse(KeyringObject* self, visitproc visit, void* arg) {
    return 0;
}

static void
Keyring_dealloc(KeyringObject* self) {
    free(self->keyring_path);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static int
Keyring_init(KeyringObject* self, PyObject* args, PyObject* kwds) {
    PyObject *bytes;
    char* keyring_path;
    Py_ssize_t len;

    if (!PyArg_ParseTuple(args, "O&", PyUnicode_FSConverter, &bytes))
        return NULL;

    PyBytes_AsStringAndSize(bytes, &keyring_path, &len);
    len += 1; // NUL terminator not counted by PyBytes_AsStringAndSize
    self->keyring_path = (char*)malloc(len);
    memcpy(self->keyring_path, keyring_path, len + 1);
    self->keyring_path_len = len;
    Py_DECREF(bytes);

    return 0;
}

static PyMethodDef Keyring_methods[] = {
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
get_gpgme_version(PyObject* self, PyObject* args) {
    return PyUnicode_FromString(gpgme_version);
}

static PyMethodDef ext_methods[] = {
    { "get_gpgme_version", (PyCFunction)get_gpgme_version, METH_NOARGS, NULL },
    { NULL }
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT, "_gpgme", "No docs", -1, ext_methods
};

int
gpgme_setup(void) {
    gpgme_version = gpgme_check_version(NULL);
    return 0;
}

PyMODINIT_FUNC
PyInit__gpgme(void) {
    if (gpgme_setup() != 0)
        return NULL;

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
