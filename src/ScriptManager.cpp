#include <stdafx.h>
#include <utility.h>
#include <ScriptManager.h>
#include <ScriptMain.h>
#include <ScriptTopLevel.h>
#include <LogManager.h>


// Template specialization to initialize singleton attribute
template<>
ssurge::ScriptManager * ssurge::Singleton<ssurge::ScriptManager>::msSingleton = NULL;

ssurge::ScriptManager::ScriptManager()
{
	// These were attempts at removing the requirement that we have python 3.5 installed...no success so far
	//::Py_NoSiteFlag = 1;
	//Py_SetProgramName(L"ssurge");
	//Py_SetPythonHome(_wgetcwd(NULL, 1000));
	//Py_SetPath(_wgetcwd(NULL, 1000));

	PyImport_AppendInittab("ssurge", ssurge::script::PyInit_ssurge);
	Py_Initialize();
}


ssurge::ScriptManager::~ScriptManager()
{
	Py_Finalize();
}



int ssurge::ScriptManager::loadScript(std::string fname)
{
	int result = 0;
	FILE * fp = NULL;
	fopen_s(&fp, fname.c_str(), "r");
	if (fp != NULL)
	{
		unsigned int file_len = 0;
		fseek(fp, 0, SEEK_END);
		file_len = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		char * buffer = NULL;
		buffer = (char*)malloc((file_len + 1) * sizeof(char));
		if (buffer)
		{
			// Read file contents
			file_len = fread(buffer, sizeof(char), file_len, fp);
			// Important: set null-char in buffer
			buffer[file_len] = 0;

			// Compile the buffer into python "byte-code"
			//PyObject * code_object = Py_CompileString(buffer, fname.c_str(), Py_file_input);
			// This version is supposed to remove all "asserts".
			PyObject * code_object = Py_CompileStringExFlags(buffer, fname.c_str(), Py_file_input, NULL, 0);
			// Execute the "byte-code".  TO-DO: we'll probably want to selectively run things...
			PyObject * module_object = PyImport_ExecCodeModule(fname.c_str(), code_object);

			if (module_object == NULL)
			{
				std::string errS = getErrorString();
				LOG_MANAGER->log("ERROR\n" + errS, LL_SCRIPT);
				return NULL;
			}
			else
				LOG_MANAGER->log("Successfully ran script '" + fname + "'", LL_SCRIPT);

			//get the module's dictionary and the dictionary's corresponding keys
			PyObject* cDict = PyModule_GetDict(module_object);
			PyObject* keys = PyDict_Keys(cDict);

			//iterate through all the dictionary's keys
			for (int i = 0; i < PyList_Size(keys); i++)
			{
				//check if the corresponds to a class type
				if (PyType_Check(PyDict_GetItem(cDict, PyList_GetItem(keys, i))))
				{
					std::string tempStr(PyUnicode_AsUTF8(PyList_GetItem(keys, i)));

					/*check if the class type has already been put into the scriptMap. 
					If it has, log a warning about overwriting types*/
					if (SCRIPT_MANAGER->scriptMap[tempStr] != NULL)
						LOG_MANAGER->log(tempStr + " is being overwritten by " + fname, LL_WARNING);

					//Add the class type to the scriptMap
					SCRIPT_MANAGER->scriptMap[tempStr] = PyDict_GetItem(cDict, PyList_GetItem(keys, i));
					
					//Add the class type to the ssurge module
					PyObject* ssurgeDict = PyModule_GetDict(SCRIPT_MANAGER->ssurgeModule);
					PyDict_SetItem(ssurgeDict, PyUnicode_FromString(tempStr.c_str()), SCRIPT_MANAGER->scriptMap[tempStr]);
					
					/*
					---CALLING A PYTHON-DEFINED TYPE EXAMPLE---
					PyObject* tups = PyTuple_New(2);
					PyTuple_SetItem(tups, 0, PyUnicode_FromString(tempStr.c_str()));
					PyTuple_SetItem(tups, 1, PyLong_FromLong(34));
					PyObject_CallObject(SCRIPT_MANAGER->scriptMap[tempStr],tups);
					*/
				}
			}
			free(buffer);
		}
		else
			result = 2;

		fclose(fp);
	}
	else
		result = 1;

	return result;
}


std::string ssurge::ScriptManager::getErrorString()
{
	PyObject * ptype, *pvalue;
	PyObject *ptrace;
	std::string s = "";

	PyErr_Fetch(&ptype, &pvalue, &ptrace);

	if (ptype != NULL)
	{
		PyObject * temps = PyObject_Str(ptype);
		char * tempcstr = PyUnicode_AsUTF8(temps);
		s += "ptype = " + std::string(tempcstr) + "\n";
		Py_DECREF(temps);
	}

	if (pvalue != NULL)
	{
		PyObject * temps = PyObject_Str(pvalue);
		char * tempcstr = PyUnicode_AsUTF8(temps);
		s += "pvalue = " + std::string(tempcstr) + "\n";
		Py_DECREF(temps);
	}

	if (ptrace != NULL)
	{
		s += "ptrace =\n";

		// The traceback object is more complex than the others: it's a linked-list
		// of "frames".  Each frame is a context (e.g. A called B, who called C would generate
		// a linked list of A -> B -> C).  Each frame has info about the filename, line#, etc.
		// Reference: http://stackoverflow.com/questions/1796510/accessing-a-python-traceback-from-the-c-api
		PyFrameObject * cur_frame;
		std::stringstream ss;
		PyTracebackObject * pactual_trace = (PyTracebackObject*)ptrace;
		
		while (pactual_trace != NULL)
		{
			cur_frame = pactual_trace->tb_frame;
			unsigned int frame_num = 0;
		
			char * fname = PyUnicode_AsUTF8(cur_frame->f_code->co_filename);
			int line = PyFrame_GetLineNumber(cur_frame);

			ss << "frame" << frame_num << ": " << fname << "@" << line << "\n";
			frame_num++;
			//cur_frame = cur_frame->f_back;
			pactual_trace = pactual_trace->tb_next;
		}
		s += ss.str() + "\n";
	}

	return s;
}