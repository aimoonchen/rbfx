#pragma once

#include "../Core/Context.h"
#include "../Core/Object.h"
extern "C" {
    typedef struct PyModuleDef PyModuleDef;
    typedef struct PyModuleDef_Slot PyModuleDef_Slot;
    typedef struct PyMethodDef PyMethodDef;
    typedef struct PyGetSetDef PyGetSetDef;
    typedef struct PyMemberDef PyMemberDef;

    typedef struct _object PyObject;
    typedef struct _longobject PyLongObject;
    typedef struct _typeobject PyTypeObject;
    typedef struct PyCodeObject PyCodeObject;
    typedef struct _frame PyFrameObject;

    typedef struct _ts PyThreadState;
    typedef struct _is PyInterpreterState;
}
namespace Urho3D
{

class Scene;

/// Python script subsystem.
class URHO3D_API PythonScript : public Object
{
    URHO3D_OBJECT(PythonScript, Object);

public:
    /// Construct.
    explicit PythonScript(Context* context);
    /// Destruct.
    ~PythonScript() override;
    bool Initialize(const std::string& pythonHome = "");
    bool AddModulePath(const std::string& path);
    PyObject* CallFunction(const std::string& moduleName, const std::string& funcName, PyObject* args = nullptr);
    PyObject* RunSimpleString(const std::string& code);
    void Finalize();
    /// Execute script file. Return true if successful.
    bool ExecuteFile(const ea::string& fileName, bool fromui = false);
    /// Execute script string. Return true if successful.
    bool ExecuteString(const ea::string& string);
    /// Load script file on file system (i.e. not from resource cache). Return true if successful.
    bool LoadRawFile(const ea::string& fileName);
    /// Load and execute script file on file system (i.e. not from resource cache). Return true if successful.
    bool ExecuteRawFile(const ea::string& fileName);
private:
    bool initialized_{ false };
    PyThreadState* main_thread_state_{ nullptr };
};

/// Register Python script library objects.
void URHO3D_API RegisterPythonScriptLibrary(Context* context);

bool URHO3D_API RunPython(Context* context, const ea::string& scriptFileName);

}

// nb::object call_python(nb::object py_func, nb::object arg)
// {
//     PyObject* result = PyObject_CallFunctionObjArgs(py_func.ptr(), arg.ptr(), nullptr);
//     if (!result) {
//         PyObject *ptype, *pvalue, *ptraceback;
//         PyErr_Fetch(&ptype, &pvalue, &ptraceback);
//
//         const char* err_msg = pvalue ? PyUnicode_AsUTF8(pvalue) : "Unknown error";
//         URHO3D_LOGERRORF("%s", err_msg);
//
//         Py_XDECREF(ptype);
//         Py_XDECREF(pvalue);
//         Py_XDECREF(ptraceback);
//         return nb::none();
//     }
//     return nb::steal(result);
// }

// #define CALL_PYTHON(func, ...) \
//     if (PyObject* result = func(__VA_ARGS__); !result) { \
//         PyObject *ptype, *pvalue, *ptraceback; \
//         PyErr_Fetch(&ptype, &pvalue, &ptraceback); \
//         const char* err_msg = pvalue ? PyUnicode_AsUTF8(pvalue) : "Unknown error"; \
//         URHO3D_LOGERRORF("%s", err_msg); \
//         Py_XDECREF(ptype); \
//         Py_XDECREF(pvalue); \
//         Py_XDECREF(ptraceback); \
//     }

#define CALL_PYTHON(func, ...) \
    nb::object result; \
    try { \
        result = func(__VA_ARGS__); \
    } catch (const nb::python_error& e) { \
        URHO3D_LOGERRORF("%s", e.what()); \
    }
