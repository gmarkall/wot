#define PY_SSIZE_T_CLEAN

#include <Python.h>

static PyMethodDef ext_methods[] = {
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

    return m;
}
