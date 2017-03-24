#ifndef _SCRIPT_MANAGER_H_
#define _SCRIPT_MANAGER_H_

#include <stdafx.h>
#include <Singleton.h>

/// Convenience macro to make accessing the singleton a bit less cumbersome
#define SCRIPT_MANAGER ssurge::ScriptManager::getSingletonPtr()

namespace ssurge
{
	/// The Script Manager is responsible for managing the embedded python interpreter.
	/// It also contains functionality to parse python scripts and run them, using the
	/// "ssurge" python/C module that is defined in ScriptTopLevel.h/.cpp.
	class ScriptManager : public Singleton<ScriptManager>
	{
	/***** ATTRIBUTES *****/
	public:
		std::map<std::string, PyObject*> scriptMap;
		PyObject* ssurgeModule;

	protected:

	/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:
		/// One and only (singleton) constructor
		ScriptManager();

		/// Destructor
		virtual ~ScriptManager();
		
	/***** METHODS *****/
	public:
		/// Loads a script from disk into (python) memory.  Fname must include path (for now).  Returns 0 on success
		int loadScript(std::string fname);


		/// Retrieves a formatted error string showing any information obtainable if something goes wrong in Python.
		std::string getErrorString();

	};

}


#endif