#include "orbit_mpi.hh"
#include "pyORBIT_Object.hh"
#include "wrap_spacecharge.hh"

#include "wrap_utils.hh"
#include "DipoleFieldSource.hh"

#include <iostream>

using namespace OrbitUtils;
using namespace wrap_orbit_utils;

namespace wrap_dipole_field_source{

  void error(const char* msg){ ORBIT_MPI_Finalize(msg); }

#ifdef __cplusplus
extern "C" {
#endif
	//---------------------------------------------------------
	//Python DipoleFieldSource class definition
	//---------------------------------------------------------

	//constructor for python class wrapping DipoleFieldSource instance
	//It never will be called directly
	static PyObject* DipoleFieldSource_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
	{
		pyORBIT_Object* self;
		self = (pyORBIT_Object *) type->tp_alloc(type, 0);
		self->cpp_obj = NULL;
		return (PyObject *) self;
	}

  //initializator for python  DipoleFieldSource class
  //this is implementation of the __init__ method
  static int DipoleFieldSource_init(pyORBIT_Object *self, PyObject *args, PyObject *kwds){
		self->cpp_obj = new DipoleFieldSource();
		((DipoleFieldSource*) self->cpp_obj)->setPyWrapper((PyObject*) self);
    return 0;
  }

  /** Sets / Returns the sizes of dipole in X,Y,Z directions in [m]*/
  static PyObject* DipoleFieldSource_sizesXYZ(PyObject *self, PyObject *args){
	  DipoleFieldSource* cpp_fieldSource = (DipoleFieldSource*)((pyORBIT_Object*) self)->cpp_obj;
	  int nArgs = PyTuple_Size(args);
	  double sizeX,sizeY,sizeZ;  
	  if(nArgs == 3){
	  	if(!PyArg_ParseTuple(args,"ddd:sizesXYZ",&sizeX,&sizeY,&sizeZ)){
	  		error("DipoleFieldSource.sizesXYZ(sizeX,sizeY,sizeZ) - parameters are needed.");
	  	}
	  	cpp_fieldSource->setSizes(sizeX,sizeY,sizeZ);
	  }
	  cpp_fieldSource->getSizes(sizeX,sizeY,sizeZ);
	  return Py_BuildValue("(ddd)",sizeX,sizeY,sizeZ);
  }	 
  
  /** Sets / Returns the fields of dipole in X,Y,Z directions in [T]*/
  static PyObject* DipoleFieldSource_fieldsXYZ(PyObject *self, PyObject *args){
	  DipoleFieldSource* cpp_fieldSource = (DipoleFieldSource*)((pyORBIT_Object*) self)->cpp_obj;
	  int nArgs = PyTuple_Size(args);
	  double fieldX,fieldY,fieldZ;  
	  if(nArgs == 3){
	  	if(!PyArg_ParseTuple(args,"ddd:fieldsXYZ",&fieldX,&fieldY,&fieldZ)){
	  		error("DipoleFieldSource.fieldsXYZ(fieldX,fieldY,fieldZ) - parameters are needed.");
	  	}
	  	cpp_fieldSource->setFields(fieldX,fieldY,fieldZ);
	  }
	  cpp_fieldSource->getFields(fieldX,fieldY,fieldZ);
	  return Py_BuildValue("(ddd)",fieldX,fieldY,fieldZ);
  }	 
  
  /** Sets or returns X,Y,Z axis symmetries */
  static PyObject* DipoleFieldSource_getFields(PyObject *self, PyObject *args){
  	DipoleFieldSource* cpp_fieldSource = (DipoleFieldSource*)((pyORBIT_Object*) self)->cpp_obj;
  	double x,y,z;
  	if(!PyArg_ParseTuple(args,"ddd:getFields",&x,&y,&z)){
  		ORBIT_MPI_Finalize("DipoleFieldSource.getFields(x,y,z) - params needed.");
  	}
  	double fe_x; double fe_y; double fe_z;
  	double fm_x; double fm_y; double fm_z;
  	double t = 0.;
  	cpp_fieldSource->getElectricMagneticField(x,y,z,t,fe_x,fe_y,fe_z,fm_x,fm_y,fm_z);
  	return Py_BuildValue("(dddddd)",fe_x,fe_y,fe_z,fm_x,fm_y,fm_z);
  }
  
  /** Sets / Returns the coordinates transformation matrix 4x4 from external to inner system */
  static PyObject* DipoleFieldSource_transormfMatrix(PyObject *self, PyObject *args){
	  DipoleFieldSource* cpp_fieldSource = (DipoleFieldSource*)((pyORBIT_Object*) self)->cpp_obj;
	  int nArgs = PyTuple_Size(args);
	  PyObject* pyMatrix;
	  Matrix* cpp_matrix;
	  if(nArgs == 1){
	  	if(!PyArg_ParseTuple(args,"O:transormfMatrix",&pyMatrix)){
	  		error("DipoleFieldSource.transormfMatrix(Matrix) - parameter is needed.");
	  	}
	  	PyObject* pyORBIT_Matrix_Type = getOrbitUtilsType("Matrix");
	  	if(!PyObject_IsInstance(pyMatrix,pyORBIT_Matrix_Type)){
	  		error("DipoleFieldSource.transormfMatrix(Matrix) - parameter is not Matrix.");
	  	}
	  	cpp_matrix = (Matrix*) ((pyORBIT_Object*) pyMatrix)->cpp_obj;
	  	if(cpp_matrix->rows() != 4 || cpp_matrix->columns() != 4){
	  		error("DipoleFieldSource.transormfMatrix(Matrix) - Matrix is not 4x4.");
	  	}
	  	// the Py_INCREF(pyMatrix) call will be performed inside setCoordsTransformMatrix(...) method 	  	
	  	cpp_fieldSource->setCoordsTransformMatrix(cpp_matrix);
	  	Py_INCREF(Py_None);
	  	return Py_None;			  	
	  }
	  cpp_matrix = cpp_fieldSource->getCoordsTransformMatrix();
	  pyMatrix = (PyObject*) ((pyORBIT_Object*) cpp_matrix->getPyWrapper());
	  if(pyMatrix == NULL){
	  	error("DipoleFieldSource.transormfMatrix() - cannot return Matrix 4x4. You have to assign it first.");
	  }
	  Py_INCREF(pyMatrix);
	  return pyMatrix;
  }  

  //-----------------------------------------------------
  //destructor for python DipoleFieldSource class (__del__ method).
  //-----------------------------------------------------
  static void DipoleFieldSource_del(pyORBIT_Object* self){
		delete ((DipoleFieldSource*)self->cpp_obj);
		self->ob_type->tp_free((PyObject*)self);
  }
	
	// defenition of the methods of the python DipoleFieldSource wrapper class
	// they will be vailable from python level
  static PyMethodDef DipoleFieldSourceClassMethods[] = {
    { "sizesXYZ",  DipoleFieldSource_sizesXYZ  ,METH_VARARGS, "Sets or returns sizes of dipole [m]"},
    { "fieldsXYZ", DipoleFieldSource_fieldsXYZ ,METH_VARARGS, "Sets or returns fields of dipole in [T]"},   
    { "getFields", DipoleFieldSource_getFields ,METH_VARARGS, "Returns E and B fields (Ex,Ey,Ez,Bx,By,Bz) for (x,y,z) point"},
    { "transormfMatrix", DipoleFieldSource_transormfMatrix,METH_VARARGS, "Sets or returns the coordinates transformation matrix 4x4 from external to inner system"},
    {NULL}
  };

	// defenition of the memebers of the python DipoleFieldSource wrapper class
	// they will be vailable from python level
	static PyMemberDef DipoleFieldSourceClassMembers [] = {
		{NULL}
	};

	//new python DipoleFieldSource wrapper type definition
	static PyTypeObject pyORBIT_DipoleFieldSource_Type = {
		PyObject_HEAD_INIT(NULL)
		0, /*ob_size*/
		"DipoleFieldSource", /*tp_name*/
		sizeof(pyORBIT_Object), /*tp_basicsize*/
		0, /*tp_itemsize*/
		(destructor) DipoleFieldSource_del , /*tp_dealloc*/
		0, /*tp_print*/
		0, /*tp_getattr*/
		0, /*tp_setattr*/
		0, /*tp_compare*/
		0, /*tp_repr*/
		0, /*tp_as_number*/
		0, /*tp_as_sequence*/
		0, /*tp_as_mapping*/
		0, /*tp_hash */
		0, /*tp_call*/
		0, /*tp_str*/
		0, /*tp_getattro*/
		0, /*tp_setattro*/
		0, /*tp_as_buffer*/
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
		"The DipoleFieldSource python wrapper", /* tp_doc */
		0, /* tp_traverse */
		0, /* tp_clear */
		0, /* tp_richcompare */
		0, /* tp_weaklistoffset */
		0, /* tp_iter */
		0, /* tp_iternext */
		DipoleFieldSourceClassMethods, /* tp_methods */
		DipoleFieldSourceClassMembers, /* tp_members */
		0, /* tp_getset */
		0, /* tp_base */
		0, /* tp_dict */
		0, /* tp_descr_get */
		0, /* tp_descr_set */
		0, /* tp_dictoffset */
		(initproc) DipoleFieldSource_init, /* tp_init */
		0, /* tp_alloc */
		DipoleFieldSource_new, /* tp_new */
	};

	//--------------------------------------------------
	//Initialization function of the pyDipoleFieldSource class
	//It will be called from wrap_field_sources_module
	//--------------------------------------------------
  void initDipoleFieldSource(PyObject* module){
		if (PyType_Ready(&pyORBIT_DipoleFieldSource_Type) < 0) return;
		Py_INCREF(&pyORBIT_DipoleFieldSource_Type);
		PyModule_AddObject(module, const_cast<char*>("DipoleFieldSource"), (PyObject *)&pyORBIT_DipoleFieldSource_Type);
	}

#ifdef __cplusplus
}
#endif

//end of namespace wrap_dipole_field_source
}
