#include <stdafx.h>
#include <Tuple.h>

ssurge::Tuple::Tuple(PyObject * args)
{
	if (!PyTuple_Check(args) || PyTuple_Size(args) == 0)
	{
		LOG_MANAGER->log("Tuple construction from PyObject argument failed.");
		return;
	}
	unsigned int s = PyTuple_Size(args);
	for (unsigned int i = 0; i != s; ++i)
	{
		BasicItem * item = NULL;
		PyObject * a = PyTuple_GetItem(args, i);
		if (PyLong_Check(a))
		{
			item = new Item<int>(static_cast<int>(PyLong_AsLong(a)));
			mCode.push_back('i');
		}
		else if (PyFloat_Check(a))
		{
			item = new Item<float>(static_cast<float>(PyFloat_AsDouble(a)));
			mCode.push_back('f');
		}
		else if (PyUnicode_Check(a))
		{
			item = new Item<std::string>(std::string(PyUnicode_AsUTF8(a)));
			mCode.push_back('s');
		}
		else if (PyBool_Check(a))
		{
			item = new Item<bool>((static_cast<int>(PyLong_AsLong(a)) != 0));
			mCode.push_back('b');
		}
		else
		{
			LOG_MANAGER->log("Tuple constructor failed to interpret an argument from a PyObject.");
		}
		if (item != NULL)
		{
			mItems.push_back(item);
		}
	}
}

ssurge::Tuple::Tuple(std::string code, ...) :
	mCode(code)
{
	va_list list;
	va_start(list, code);
	for (int i = 0; i != mCode.size(); ++i)
	{
		BasicItem * item = NULL;
		switch (mCode[i])
		{
		case 'b':
			item = new Item<bool>(va_arg(list, bool));
			break;
		case 'i':
			item = new Item<int>(va_arg(list, int));
			break;
		case 'f':
			item = new Item<float>(static_cast<float>(va_arg(list, double)));
			break;
		case 'd':
			item = new Item<double>(va_arg(list, double));
			break;
		case 'c':
			item = new Item<char>(static_cast<char>(va_arg(list, int)));
			break;
		case 'l':
			item = new Item<long>(va_arg(list, long));
			break;
		case 'u':
			item = new Item<unsigned int>(static_cast<unsigned int>(va_arg(list, int)));
			break;
		case 's':
			item = new Item<std::string>(std::string(va_arg(list, char*)));
			break;
		default:
			LOG_MANAGER->log("Tuple with code \""+mCode+"\" could not interpret argument at index "+std::to_string(i)+" with code '"+mCode.substr(i, 1)+"', argument value: "+std::to_string(va_arg(list, int)), LL_ERROR);
			if (i == mItems.size() - 1)
			{
				mCode = mCode.substr(0, i);
				break;
			}
			mCode = mCode.substr(0, i) + mCode.substr(i + 1);
			--i;
			continue;
		}
		if (item != NULL)
		{
			mItems.push_back(item);
		}
	}
	va_end(list);
}

ssurge::Tuple::Tuple(Tuple * tuple, std::string code, ...) :
	mCode(code)
{
	va_list list;
	va_start(list, code);
	for (int i = 0; i != mCode.size(); ++i)
	{
		BasicItem * item = NULL;
		switch (mCode[i])
		{
		case 'b':
			item = new Item<bool>(va_arg(list, bool));
			break;
		case 'i':
			item = new Item<int>(va_arg(list, int));
			break;
		case 'f':
			item = new Item<float>(static_cast<float>(va_arg(list, double)));
			break;
		case 'd':
			item = new Item<double>(va_arg(list, double));
			break;
		case 'c':
			item = new Item<char>(static_cast<char>(va_arg(list, int)));
			break;
		case 'l':
			item = new Item<long>(va_arg(list, long));
			break;
		case 'u':
			item = new Item<unsigned int>(static_cast<unsigned int>(va_arg(list, int)));
			break;
		case 's':
			item = new Item<std::string>(std::string(va_arg(list, char*)));
			break;
		default:
			LOG_MANAGER->log("Tuple with code \"" + mCode + "\" could not interpret argument at index " + std::to_string(i) + " with code '" + mCode.substr(i, 1) + "', argument value: " + std::to_string(va_arg(list, int)), LL_ERROR);
			if (i == mItems.size() - 1)
			{
				mCode = mCode.substr(0, i);
				break;
			}
			mCode = mCode.substr(0, i) + mCode.substr(i + 1);
			--i;
			continue;
		}
		if (item != NULL)
		{
			mItems.push_back(item);
		}
	}
	if (tuple)
	{
		code = tuple->getCode();
		for (int i = 0; i != tuple->getSize(); ++i)
		{
			BasicItem * item = NULL;
			switch (code[i])
			{
			case 'b':
				item = new Item<bool>(tuple->getItemAsBool(i));
				break;
			case 'i':
				item = new Item<int>(tuple->getItemAsInt(i));
				break;
			case 'f':
				item = new Item<float>(tuple->getItemAsFloat(i));
				break;
			case 'd':
				item = new Item<double>(tuple->getItemAsDouble(i));
				break;
			case 'c':
				item = new Item<char>(tuple->getItemAsChar(i));
				break;
			case 'l':
				item = new Item<long>(tuple->getItemAsLong(i));
				break;
			case 'u':
				item = new Item<unsigned int>(tuple->getItemAsUnsignedInt(i));
				break;
			case 's':
				item = new Item<std::string>(tuple->getItemAsString(i));
				break;
			default:
				LOG_MANAGER->log("Tuple with code \"" + mCode + "\" could not interpret argument at index " + std::to_string(i) + " with code '" + mCode.substr(i, 1) + "', argument value: " + std::to_string(va_arg(list, int)), LL_ERROR);
				if (i == mItems.size() - 1)
				{
					code = code.substr(0, i);
					break;
				}
				code = code.substr(0, i) + code.substr(i + 1);
				--i;
				continue;
			}
			if (item != NULL)
			{
				mItems.push_back(item);
				mCode.push_back(code[i]);
			}
		}
	}
	va_end(list);
}

ssurge::Tuple::~Tuple()
{
	for (int i = 0; i != mItems.size(); ++i)
	{
		delete mItems[i];
	}
	mItems.clear();
}

void ssurge::Tuple::log()
{
	std::string message = "Code: " + mCode + ", Size: " + std::to_string(mCode.size());
	for (int i = 0; i != mCode.size(); ++i)
	{
		message += ", Item " + std::to_string(i) + ": <";
		switch (mCode[i])
		{
		case 'b':
			if (static_cast<Item<bool>*>(mItems[i])->mItem)
			{
				message += "TRUE";
			}
			else
			{
				message += "FALSE";
			}
			break;
		case 'i':
			message += std::to_string(static_cast<Item<int>*>(mItems[i])->mItem);
			break;
		case 'f':
			message += std::to_string(static_cast<Item<float>*>(mItems[i])->mItem);
			break;
		case 'd':
			message += std::to_string(static_cast<Item<double>*>(mItems[i])->mItem);
			break;
		case 'c':
			message += std::to_string(static_cast<Item<char>*>(mItems[i])->mItem);
			break;
		case 'l':
			message += std::to_string(static_cast<Item<long>*>(mItems[i])->mItem);
			break;
		case 'u':
			message += std::to_string(static_cast<Item<unsigned int>*>(mItems[i])->mItem);
			break;
		case 's':
			message += "\""+std::string(static_cast<Item<std::string>*>(mItems[i])->mItem)+"\"";
			break;
		default:
			LOG_MANAGER->log("Tuple with code \"" + mCode + "\" encountered an internal error while logging itself at item index " + std::to_string(i) + " with code '" + std::to_string(mCode[i]) + "'", LL_ERROR);
			if (i == mItems.size() - 1)
			{
				mCode = mCode.substr(0, i);
				message += ">";
				break;
			}
			mCode = mCode.substr(0, i) + mCode.substr(i + 1);
			--i;
			continue;
		}
		message += ">";
	}
	LOG_MANAGER->log(message, LL_NORMAL);
}

PyObject* ssurge::Tuple::getPyTuple(bool doRegister)
{
	PyObject * tuple = PyTuple_New(mCode.size());
	for (int i = 0; i != mCode.size(); ++i)
	{
		PyObject * item = NULL;
		switch (mCode[i])
		{
		case 'b':
			item = PyBool_FromLong(static_cast<Item<bool>*>(mItems[i])->mItem?1:0);
			break;
		case 'i':
			item = PyLong_FromLong(static_cast<Item<int>*>(mItems[i])->mItem);
			break;
		case 'f':
			item = PyFloat_FromDouble(static_cast<Item<float>*>(mItems[i])->mItem);
			break;
		case 'd':
			item = PyFloat_FromDouble(static_cast<Item<double>*>(mItems[i])->mItem);
			break;
		case 'c':
			item = PyLong_FromLong(static_cast<Item<char>*>(mItems[i])->mItem);
			break;
		case 'l':
			item = PyLong_FromLong(static_cast<Item<long>*>(mItems[i])->mItem);
			break;
		case 'u':
			item = PyLong_FromUnsignedLong(static_cast<Item<unsigned int>*>(mItems[i])->mItem);
			break;
		case 's':
			item = PyUnicode_FromString(static_cast<Item<std::string>*>(mItems[i])->mItem.c_str());
			break;
		default:
			LOG_MANAGER->log("Tuple with code \"" + mCode + "\" encountered an internal error while building a PyTuple at item index "+std::to_string(i)+" with code '"+std::to_string(mCode[i])+"'", LL_ERROR);
			if (i == mItems.size() - 1)
			{
				mCode = mCode.substr(0, i);
				break;
			}
			mCode = mCode.substr(0, i) + mCode.substr(i + 1);
			--i;
			continue;
		}
		if (item != NULL)
		{
			PyTuple_SetItem(tuple, i, item);
		}
	}
	if (doRegister)
	{
		Py_INCREF(tuple);
	}
	return tuple;
}

bool ssurge::Tuple::getItemAsBool(unsigned int index)
{
	bool error = false;
	char code = 'b';
	if (index >= mItems.size())
	{
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" was accessed outside of its capacity of " + std::to_string(mItems.size()) + " at index " + std::to_string(index), LL_ERROR);
		return error;
	}
	switch (mCode[index])
	{
	case 'b':
		return static_cast<bool>(static_cast<Item<bool>*>(mItems[index])->mItem);
	case 'i':
		return static_cast<bool>(static_cast<Item<int>*>(mItems[index])->mItem);
	case 'f':
		return static_cast<bool>(static_cast<Item<float>*>(mItems[index])->mItem);
	case 'd':
		return static_cast<bool>(static_cast<Item<double>*>(mItems[index])->mItem);
	case 'c':
		return static_cast<bool>(static_cast<Item<char>*>(mItems[index])->mItem);
	case 'l':
		return static_cast<bool>(static_cast<Item<long>*>(mItems[index])->mItem);
	case 'u':
		return static_cast<bool>(static_cast<Item<unsigned int>*>(mItems[index])->mItem);
	case 's':
		return static_cast<bool>(atoi(static_cast<Item<std::string>*>(mItems[index])->mItem.c_str()));
	default:
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" encountered an internal error while casting an item at index " + std::to_string(index) + " with code '" + std::to_string(mCode[index]) + "' to code '" + std::to_string(code) + "'", LL_ERROR);
		if (index == mItems.size() - 1)
		{
			mCode = mCode.substr(0, index);
			break;
		}
		mCode = mCode.substr(0, index) + mCode.substr(index + 1);
		break;
	}
	return error;
}

int ssurge::Tuple::getItemAsInt(unsigned int index)
{
	int error = 0;
	char code = 'i';
	if (index >= mItems.size())
	{
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" was accessed outside of its capacity of " + std::to_string(mItems.size()) + " at index " + std::to_string(index), LL_ERROR);
		return error;
	}
	switch (mCode[index])
	{
	case 'b':
		return static_cast<int>(static_cast<Item<bool>*>(mItems[index])->mItem);
	case 'i':
		return static_cast<int>(static_cast<Item<int>*>(mItems[index])->mItem);
	case 'f':
		return static_cast<int>(static_cast<Item<float>*>(mItems[index])->mItem);
	case 'd':
		return static_cast<int>(static_cast<Item<double>*>(mItems[index])->mItem);
	case 'c':
		return static_cast<int>(static_cast<Item<char>*>(mItems[index])->mItem);
	case 'l':
		return static_cast<int>(static_cast<Item<long>*>(mItems[index])->mItem);
	case 'u':
		return static_cast<int>(static_cast<Item<unsigned int>*>(mItems[index])->mItem);
	case 's':
		return static_cast<int>(atoi(static_cast<Item<std::string>*>(mItems[index])->mItem.c_str()));
	default:
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" encountered an internal error while casting an item at index " + std::to_string(index) + " with code '" + std::to_string(mCode[index]) + "' to code '" + std::to_string(code) + "'", LL_ERROR);
		if (index == mItems.size() - 1)
		{
			mCode = mCode.substr(0, index);
			break;
		}
		mCode = mCode.substr(0, index) + mCode.substr(index + 1);
		break;
	}
	return error;
}

float ssurge::Tuple::getItemAsFloat(unsigned int index)
{
	float error = 0.0f;
	char code = 'f';
	if (index >= mItems.size())
	{
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" was accessed outside of its capacity of " + std::to_string(mItems.size()) + " at index " + std::to_string(index), LL_ERROR);
		return error;
	}
	switch (mCode[index])
	{
	case 'b':
		return static_cast<float>(static_cast<Item<bool>*>(mItems[index])->mItem);
	case 'i':
		return static_cast<float>(static_cast<Item<int>*>(mItems[index])->mItem);
	case 'f':
		return static_cast<float>(static_cast<Item<float>*>(mItems[index])->mItem);
	case 'd':
		return static_cast<float>(static_cast<Item<double>*>(mItems[index])->mItem);
	case 'c':
		return static_cast<float>(static_cast<Item<char>*>(mItems[index])->mItem);
	case 'l':
		return static_cast<float>(static_cast<Item<long>*>(mItems[index])->mItem);
	case 'u':
		return static_cast<float>(static_cast<Item<unsigned int>*>(mItems[index])->mItem);
	case 's':
		return static_cast<float>(atof(static_cast<Item<std::string>*>(mItems[index])->mItem.c_str()));
	default:
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" encountered an internal error while casting an item at index " + std::to_string(index) + " with code '" + std::to_string(mCode[index]) + "' to code '" + std::to_string(code) + "'", LL_ERROR);
		if (index == mItems.size() - 1)
		{
			mCode = mCode.substr(0, index);
			break;
		}
		mCode = mCode.substr(0, index) + mCode.substr(index + 1);
		break;
	}
	return error;
}

double ssurge::Tuple::getItemAsDouble(unsigned int index)
{
	double error = 0.0;
	char code = 'd';
	if (index >= mItems.size())
	{
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" was accessed outside of its capacity of " + std::to_string(mItems.size()) + " at index " + std::to_string(index), LL_ERROR);
		return error;
	}
	switch (mCode[index])
	{
	case 'b':
		return static_cast<double>(static_cast<Item<bool>*>(mItems[index])->mItem);
	case 'i':
		return static_cast<double>(static_cast<Item<int>*>(mItems[index])->mItem);
	case 'f':
		return static_cast<double>(static_cast<Item<float>*>(mItems[index])->mItem);
	case 'd':
		return static_cast<double>(static_cast<Item<double>*>(mItems[index])->mItem);
	case 'c':
		return static_cast<double>(static_cast<Item<char>*>(mItems[index])->mItem);
	case 'l':
		return static_cast<double>(static_cast<Item<long>*>(mItems[index])->mItem);
	case 'u':
		return static_cast<double>(static_cast<Item<unsigned int>*>(mItems[index])->mItem);
	case 's':
		return static_cast<double>(atof(static_cast<Item<std::string>*>(mItems[index])->mItem.c_str()));
	default:
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" encountered an internal error while casting an item at index " + std::to_string(index) + " with code '" + std::to_string(mCode[index]) + "' to code '" + std::to_string(code) + "'", LL_ERROR);
		if (index == mItems.size() - 1)
		{
			mCode = mCode.substr(0, index);
			break;
		}
		mCode = mCode.substr(0, index) + mCode.substr(index + 1);
		break;
	}
	return error;
}

char ssurge::Tuple::getItemAsChar(unsigned int index)
{
	char error = 0;
	char code = 'c';
	if (index >= mItems.size())
	{
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" was accessed outside of its capacity of " + std::to_string(mItems.size()) + " at index " + std::to_string(index), LL_ERROR);
		return error;
	}
	switch (mCode[index])
	{
	case 'b':
		return static_cast<char>(static_cast<Item<bool>*>(mItems[index])->mItem);
	case 'i':
		return static_cast<char>(static_cast<Item<int>*>(mItems[index])->mItem);
	case 'f':
		return static_cast<char>(static_cast<Item<float>*>(mItems[index])->mItem);
	case 'd':
		return static_cast<char>(static_cast<Item<double>*>(mItems[index])->mItem);
	case 'c':
		return static_cast<char>(static_cast<Item<char>*>(mItems[index])->mItem);
	case 'l':
		return static_cast<char>(static_cast<Item<long>*>(mItems[index])->mItem);
	case 'u':
		return static_cast<char>(static_cast<Item<unsigned int>*>(mItems[index])->mItem);
	case 's':
		return static_cast<char>(atoi(static_cast<Item<std::string>*>(mItems[index])->mItem.c_str()));
	default:
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" encountered an internal error while casting an item at index " + std::to_string(index) + " with code '" + std::to_string(mCode[index]) + "' to code '" + std::to_string(code) + "'", LL_ERROR);
		if (index == mItems.size() - 1)
		{
			mCode = mCode.substr(0, index);
			break;
		}
		mCode = mCode.substr(0, index) + mCode.substr(index + 1);
		break;
	}
	return error;
}

long ssurge::Tuple::getItemAsLong(unsigned int index)
{
	long error = 0;
	char code = 'l';
	if (index >= mItems.size())
	{
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" was accessed outside of its capacity of " + std::to_string(mItems.size()) + " at index " + std::to_string(index), LL_ERROR);
		return error;
	}
	switch (mCode[index])
	{
	case 'b':
		return static_cast<long>(static_cast<Item<bool>*>(mItems[index])->mItem);
	case 'i':
		return static_cast<long>(static_cast<Item<int>*>(mItems[index])->mItem);
	case 'f':
		return static_cast<long>(static_cast<Item<float>*>(mItems[index])->mItem);
	case 'd':
		return static_cast<long>(static_cast<Item<double>*>(mItems[index])->mItem);
	case 'c':
		return static_cast<long>(static_cast<Item<char>*>(mItems[index])->mItem);
	case 'l':
		return static_cast<long>(static_cast<Item<long>*>(mItems[index])->mItem);
	case 'u':
		return static_cast<long>(static_cast<Item<unsigned int>*>(mItems[index])->mItem);
	case 's':
		return static_cast<long>(atoi(static_cast<Item<std::string>*>(mItems[index])->mItem.c_str()));
	default:
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" encountered an internal error while casting an item at index " + std::to_string(index) + " with code '" + std::to_string(mCode[index]) + "' to code '" + std::to_string(code) + "'", LL_ERROR);
		if (index == mItems.size() - 1)
		{
			mCode = mCode.substr(0, index);
			break;
		}
		mCode = mCode.substr(0, index) + mCode.substr(index + 1);
		break;
	}
	return error;
}

unsigned int ssurge::Tuple::getItemAsUnsignedInt(unsigned int index)
{
	unsigned error = 0;
	char code = 'u';
	if (index >= mItems.size())
	{
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" was accessed outside of its capacity of " + std::to_string(mItems.size()) + " at index " + std::to_string(index), LL_ERROR);
		return error;
	}
	switch (mCode[index])
	{
	case 'b':
		return static_cast<unsigned int>(static_cast<Item<bool>*>(mItems[index])->mItem);
	case 'i':
		return static_cast<unsigned int>(static_cast<Item<int>*>(mItems[index])->mItem);
	case 'f':
		return static_cast<unsigned int>(static_cast<Item<float>*>(mItems[index])->mItem);
	case 'd':
		return static_cast<unsigned int>(static_cast<Item<double>*>(mItems[index])->mItem);
	case 'c':
		return static_cast<unsigned int>(static_cast<Item<char>*>(mItems[index])->mItem);
	case 'l':
		return static_cast<unsigned int>(static_cast<Item<long>*>(mItems[index])->mItem);
	case 'u':
		return static_cast<unsigned int>(static_cast<Item<unsigned int>*>(mItems[index])->mItem);
	case 's':
		return static_cast<unsigned int>(atoi(static_cast<Item<std::string>*>(mItems[index])->mItem.c_str()));
	default:
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" encountered an internal error while casting an item at index " + std::to_string(index) + " with code '" + std::to_string(mCode[index]) + "' to code '" + std::to_string(code) + "'", LL_ERROR);
		if (index == mItems.size() - 1)
		{
			mCode = mCode.substr(0, index);
			break;
		}
		mCode = mCode.substr(0, index) + mCode.substr(index + 1);
		break;
	}
	return error;
}

std::string ssurge::Tuple::getItemAsString(unsigned int index)
{
	std::string error = "";
	char code = 's';
	if (index >= mItems.size())
	{
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" was accessed outside of its capacity of " + std::to_string(mItems.size()) + " at index " + std::to_string(index), LL_ERROR);
		return error;
	}
	switch (mCode[index])
	{
	case 'b':
		return std::to_string(static_cast<Item<bool>*>(mItems[index])->mItem);
	case 'i':
		return std::to_string(static_cast<Item<int>*>(mItems[index])->mItem);
	case 'f':
		return std::to_string(static_cast<Item<float>*>(mItems[index])->mItem);
	case 'd':
		return std::to_string(static_cast<Item<double>*>(mItems[index])->mItem);
	case 'c':
		return std::to_string(static_cast<Item<char>*>(mItems[index])->mItem);
	case 'l':
		return std::to_string(static_cast<Item<long>*>(mItems[index])->mItem);
	case 'u':
		return std::to_string(static_cast<Item<unsigned int>*>(mItems[index])->mItem);
	case 's':
		return static_cast<Item<std::string>*>(mItems[index])->mItem;
	default:
		LOG_MANAGER->log("Tuple with code \"" + mCode + "\" encountered an internal error while casting an item at index " + std::to_string(index) + " with code '" + std::to_string(mCode[index]) + "' to code '" + std::to_string(code) + "'", LL_ERROR);
		if (index == mItems.size() - 1)
		{
			mCode = mCode.substr(0, index);
			break;
		}
		mCode = mCode.substr(0, index) + mCode.substr(index + 1);
		break;
	}
	return error;
}

unsigned int ssurge::Tuple::getSize() const
{
	return mItems.size();
}

const std::string& ssurge::Tuple::getCode() const
{
	return mCode;
}
