#pragma once

#include <angelscript/angelscript.h>
#include "stdio.h"
#include <string>

BEGIN_AS_NAMESPACE

class CScriptFile
{
public:
	CScriptFile();

	void AddRef() const;
	void Release() const;

	// TODO: Implement the "r+", "w+" and "a+" modes
	// mode = "r" -> open the file for reading
	//        "w" -> open the file for writing (overwrites existing file)
	//        "a" -> open the file for appending
	int  Open(const std::string& filename, const std::string& mode);
	int  Close();
	int  GetSize() const;
	bool IsEOF() const;

	// Reading
	std::string ReadString(unsigned int length);
	std::string ReadLine();
	asINT64     ReadInt(asUINT bytes);
	asQWORD     ReadUInt(asUINT bytes);
	float       ReadFloat();
	double      ReadDouble();

	// Writing
	int WriteString(const std::string& str);
	int WriteInt(asINT64 v, asUINT bytes);
	int WriteUInt(asQWORD v, asUINT bytes);
	int WriteFloat(float v);
	int WriteDouble(double v);

	// Cursor
	int GetPos() const;
	int SetPos(int pos);
	int MovePos(int delta);

	// Big-endian = most significant byte first
	bool mostSignificantByteFirst;

protected:
	~CScriptFile();

	mutable int refCount;
	FILE* file;
};

// This function will determine the configuration of the engine
// and use one of the two functions below to register the file type
void RegisterScriptFile(asIScriptEngine* engine);


END_AS_NAMESPACE