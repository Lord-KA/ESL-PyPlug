#define PY_SSIZE_T_CLEAN
#include <python3.10/Python.h>
#include <python3.10/modsupport.h>
#include <python3.10/longobject.h>
#include <python3.10/structmember.h>

#include "tools.hpp"
#include <iostream>

static PyObject*
myadd(PyObject *self, PyObject *args)
{
    long a = 0, b = 0;
    if (!PyArg_ParseTuple(args, "ll", &a, &b)) {
        std::cerr << "Bad arguments!\n";
        return NULL;
    }
    long sum = a + b;
    //std::cerr << "a = " << a << "; b = " << b << "; sum = " << sum << std::endl;
    return PyLong_FromLong(sum);
}

static PyMethodDef AddMethods[] = {
    {"myadd", myadd, METH_VARARGS, "add some numbers"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef addmodule = {
    PyModuleDef_HEAD_INIT,
    "myadd",
    NULL,
    -1,
    AddMethods
};

PyMODINIT_FUNC
PyInit_myadd()
{
    return PyModule_Create(&addmodule);
}

class Image : public booba::Image {
    virtual size_t getH() override
    {
        std::cerr << "getH() was called!\n";
        return -1;
    }

    virtual size_t getW() override
    {
        std::cerr << "getW() was called!\n";
        return -1;
    }

    virtual uint32_t getPixel(size_t x, size_t y) override
    {
        std::cerr << "getPixel() was called!\n";
        return -1;
    }

    virtual void setPixel(size_t x, size_t y, uint32_t color) override
    {
        std::cerr << "setPixel() was called!\n";
    }
};

static PyTypeObject PyImage = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "PyImage.PyImage",
    .tp_basicsize = sizeof(Image),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Python booba::Image interface"),
    .tp_new = PyType_GenericNew,
};

static PyMethodDef Custom_methods[] = {
    {"getH", T_OBJECT_EX, offsetof()}
}

int
main()
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;

    const char moduleName[] = "myadd";

    PyImport_AppendInittab(moduleName, &PyInit_myadd);

    Py_Initialize();
    pName = PyUnicode_DecodeFSDefault("./one.py");
    /* Error checking of pName left out */

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule == NULL) {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", moduleName);
        return 1;
    }

    const char funcName[] = "add";
    pFunc = PyObject_GetAttrString(pModule, funcName);
    /* pFunc is a new reference */

    if (pFunc == NULL or not PyCallable_Check(pFunc)) {
        if (PyErr_Occurred())
            PyErr_Print();
        fprintf(stderr, "Cannot find function \"%s\"\n", funcName);
    }
    pArgs = PyTuple_New(1);
    pValue = PyLong_FromLong(179L);
    /* pValue reference stolen here: */
    PyTuple_SetItem(pArgs, 0, pValue);

    pValue = PyObject_CallObject(pFunc, pArgs);
    Py_DECREF(pArgs);
    if (pValue != NULL) {
        printf("Result of call: %ld\n", PyLong_AsLong(pValue));
        Py_DECREF(pValue);
    }
    else {
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
        PyErr_Print();
        fprintf(stderr,"Call failed\n");
        return 1;
    }

    Py_XDECREF(pFunc);
    Py_DECREF(pModule);

    if (Py_FinalizeEx() < 0) {
        return 120;
    }
    return 0;
}

/*
int main()
{
    PyObject *pInt;

    Py_Initialize();

    PyRun_SimpleString("print('Hello World!')");

    Py_Finalize();
}
*/
