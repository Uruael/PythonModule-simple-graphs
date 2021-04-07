#include <Python.h>
#include <string.h>
#include <bitset>


typedef struct {
    PyObject_HEAD
    long long vertices;
    long long g[64];
} AdjacencyMatrix;

static PyObject* create_path(AdjacencyMatrix* self, PyObject* args);

static PyObject* AdjacencyMatrix__new__(PyTypeObject* type, PyObject *args){

    return type->tp_alloc(type,0);

}

static void AdjacencyMatrix__del__(AdjacencyMatrix* self){
    Py_TYPE(self)->tp_free((PyObject*)self);
}

int g6Matrix(char* g6, AdjacencyMatrix* self){

    int size=g6[0]-63;
    std::bitset<(2016)> g6bit;
/*
    for(int i=0;i<size;i++){
        self->g[i] = self->g[i] | (unsigned long long)1<<(63-i);   //set vertexes old
    }
*/
    for(int i=0;i<size;i++){
        self->vertices = self->vertices | (unsigned long long)1<<(63-i);   //set vertexes
    }
  

    for(unsigned int i=1;i<strlen(g6);i++){ //set bitset
        g6bit |= (g6[i]-63);
        g6bit <<= 6;

    }

    g6bit <<= (2016-(6*strlen(g6)));

    unsigned int vertexsize = 1;

    for(unsigned int i=1;i<64;i++){ //create edges in lower triangle
        for(unsigned int j=0;j<vertexsize;j++){
            self->g[i] = self->g[i] | (((long long)g6bit[2015])<<(63-j)); //right edge
            g6bit<<=1; //fix bitset
        }
        vertexsize++; //row grows
    }

    unsigned long long temp;

    for(unsigned int i=0;i<63;i++){ //create edges in top triangle
        for(unsigned int j=i+1;j<64;j++){
            temp = ((self->g[j] & (unsigned long long)1<<(63-i)));
            temp = temp>>(j-i);
            self->g[i] |= temp;
        }
    }


    return 0;

}

static int AdjacencyMatrix__init__(AdjacencyMatrix* self, PyObject* args){

    char* g6=NULL;
  

    for(int i=0;i<64;i++){
        self->g[i]=0;
        self->vertices=0;
    }

    if(!PyArg_ParseTuple(args, "|s", &g6))
        return -1;

    if(g6!=NULL){
        g6Matrix(g6, self);
    }

    return 0;
}



int CexampleNumber(int num){
    return num;
}

int Cnumber_of_vertices(AdjacencyMatrix* self){
    int result=0;
    for(int i=0;i<64;i++){
        if(self->vertices & (unsigned long long)1<<(63-i))
            result++;
    }
    return result;
}

int Cvertex_degree(int num, AdjacencyMatrix* self){
    
    int result=0;
    unsigned long long vertex=self->g[num];

    for(int i=0;i<64;i++){
        result+=(vertex & 1);
        vertex >>= 1;
    }

    return result;    

}

int countSetBits(unsigned long long n){
    int result = 0;

    for(int i=0;i<64;i++){
        result+=(n & 1);
        n >>= 1;
    }

    return result;
}

static PyObject* exampleNumber(PyObject* self, PyObject* args){
    int num, sts;

    if(!PyArg_ParseTuple(args, "i", &num))
        return NULL;

    sts = CexampleNumber(num);
    return PyLong_FromLong(sts);
}


static PyObject* number_of_vertices(AdjacencyMatrix* self){
    int sts;
    sts = Cnumber_of_vertices(self);
    return PyLong_FromLong(sts);
}

static PyObject* vertices(AdjacencyMatrix* self){

    PyObject *pyset = PySet_New(NULL);

    for(int i=0;i<64;i++){
        if(self->vertices & (unsigned long long)1<<(63-i)){
            PyObject *ver = PyLong_FromLong(i);
            PySet_Add(pyset,ver);
            Py_DECREF(ver);
        }
    }

    return pyset;
}

static PyObject* vertex_degree(AdjacencyMatrix* self, PyObject* args){

    int num, sts;

    if(!PyArg_ParseTuple(args, "i", &num))
        return NULL;

    sts = Cvertex_degree(num, self);
    return PyLong_FromLong(sts);
}

static PyObject* vertex_neighbors(AdjacencyMatrix* self, PyObject* args){

    int num;
    PyObject *pyset = PySet_New(NULL);

    if(!PyArg_ParseTuple(args, "i", &num))
        return NULL;

    for(int i=0;i<64;i++){
            if(self->g[num] & ((long long)1<<(63-i))){
                PyObject *ver = PyLong_FromLong(i);
                PySet_Add(pyset,ver);
                Py_DECREF(ver);
            }
                
    }
    
    return pyset;
}

static PyObject* add_vertex(AdjacencyMatrix* self, PyObject* args){

    int num;

    if(!PyArg_ParseTuple(args, "i", &num))
        return NULL;

    self->vertices = self->vertices | (long long)1<<(63-num);

    return Py_BuildValue("");
    
}

static PyObject* delete_vertex(AdjacencyMatrix* self, PyObject* args){

    int num;

    if(!PyArg_ParseTuple(args, "i", &num))
        return NULL;

    long long position = ~((long long)1<<(63-num));

    for(int i=0;i<64;i++){
            self->g[i] = self->g[i] & position;
    }

    self->g[num] = 0;
    self->vertices = self->vertices & position;

    return Py_BuildValue("");
    
}

static PyObject* number_of_edges(AdjacencyMatrix* self){

    int sts=0;

    for(int i=0;i<64;i++){
        if(self->g[i])
            sts+=countSetBits(self->g[i]);
    }

    sts/=2;

    return PyLong_FromLong(sts);
    
}

static PyObject* edges(AdjacencyMatrix* self){

    PyObject *pyset = PySet_New(NULL);

    for(int i=0;i<64;i++){
        long long vertex = self->g[i];
        for(int j=0;j<i;j++){
            if( vertex & ((unsigned long long)1<<(63-j)) ){
                PyObject *pytuple = PyTuple_New(2);
                PyTuple_SetItem(pytuple, 0, PyLong_FromLong(j));
                PyTuple_SetItem(pytuple, 1, PyLong_FromLong(i));
                PySet_Add(pyset,pytuple);
                Py_DECREF(pytuple);
            }
        }

    }
    
    return pyset;
    
}

static PyObject* is_edge(AdjacencyMatrix* self, PyObject* args){

    int v1,v2;

    if(!PyArg_ParseTuple(args, "ii", &v1, &v2))
        return NULL;


    if(self->g[v1] & ((unsigned long long)1<<(63-v2)))
        return PyLong_FromLong(1);
    else
        return PyLong_FromLong(0);
    
}

static PyObject* add_edge(AdjacencyMatrix* self, PyObject* args){

    int v1,v2;

    if(!PyArg_ParseTuple(args, "ii", &v1, &v2))
        return NULL;


    self->g[v1] = self->g[v1] | ((unsigned long long)1<<(63-v2));
    self->g[v2] = self->g[v2] | ((unsigned long long)1<<(63-v1));

   return Py_BuildValue("");

}

static PyObject* delete_edge(AdjacencyMatrix* self, PyObject* args){

    int v1,v2;

    if(!PyArg_ParseTuple(args, "ii", &v1, &v2))
        return NULL;

    self->g[v1] = self->g[v1] & ~((unsigned long long)1<<(63-v2));
    self->g[v2] = self->g[v2] & ~((unsigned long long)1<<(63-v1));

    return Py_BuildValue("");
}




static PyObject* version(AdjacencyMatrix* self){
    return Py_BuildValue("s", "Version 0.01");
}

static PyMethodDef AdjacencyMatrixMethods[]={
    {"exampleNumber",(PyCFunction)exampleNumber, METH_VARARGS, "Returns your number"},
    {"number_of_vertices",(PyCFunction)number_of_vertices, METH_NOARGS, "Zwraca liczbę wierzchołków grafu"},
    {"vertices",(PyCFunction)vertices, METH_NOARGS, "Zwraca zbiór wierzchołków grafu"},
    {"vertex_degree",(PyCFunction)vertex_degree, METH_VARARGS, ""},
    {"vertex_neighbors",(PyCFunction)vertex_neighbors, METH_VARARGS, ""},
    {"add_vertex",(PyCFunction)add_vertex, METH_VARARGS, ""},
    {"delete_vertex",(PyCFunction)delete_vertex, METH_VARARGS, ""},
    {"number_of_edges",(PyCFunction)number_of_edges, METH_NOARGS, ""},
    {"edges",(PyCFunction)edges, METH_NOARGS, ""},
    {"is_edge",(PyCFunction)is_edge, METH_VARARGS, ""},
    {"add_edge",(PyCFunction)add_edge, METH_VARARGS, ""},
    {"delete_edge",(PyCFunction)delete_edge, METH_VARARGS, ""},
    {"create_path", (PyCFunction)create_path, METH_VARARGS | METH_STATIC, ""},//METH_STATIC 
    {"version", (PyCFunction)version, METH_NOARGS, "returns the version of module"},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject AdjacencyMatrix_Type = {
    PyVarObject_HEAD_INIT( NULL,0 )
    "simple_graphs.AdjacencyMatrix",
    sizeof(AdjacencyMatrix),
    0,
    (destructor)AdjacencyMatrix__del__,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    Py_TPFLAGS_DEFAULT,
    "AdjacencyMatrix",
    0,
    0,
    0,
    0,
    0,
    0,
    AdjacencyMatrixMethods,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (initproc)AdjacencyMatrix__init__,
    0,
    (newfunc)AdjacencyMatrix__new__
};

static PyObject* create_path(AdjacencyMatrix* self, PyObject* args){
    

    int vert;
    if(!PyArg_ParseTuple(args, "i", &vert))
        return NULL;

    PyObject* myarg = Py_BuildValue("(s)", "?");
           
    PyObject *obj = PyObject_CallObject((PyObject *) &AdjacencyMatrix_Type, myarg);

    for(int i=0;i<vert;i++){
        PyObject_CallMethod(obj, "add_vertex", "(i)", i);
        if(i+1<vert)
            PyObject_CallMethod(obj, "add_edge", "(ii)", i, i+1);
    }

    Py_DECREF(myarg);
    return obj;

}

static struct PyModuleDef simple_graphs = {
    PyModuleDef_HEAD_INIT,
    "simple_graphs",
    "simple_graphs Module",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC PyInit_simple_graphs(void){
    if(PyType_Ready(&AdjacencyMatrix_Type)<0) return NULL;

    PyObject* m = PyModule_Create(&simple_graphs);
    if(m==NULL) return NULL;

    Py_INCREF(&AdjacencyMatrix_Type);
    PyModule_AddObject(m,"AdjacencyMatrix",(PyObject*)&AdjacencyMatrix_Type);
    
    return m;
}