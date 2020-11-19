#include "../../../headers/Scripting/Types/ASFile.hpp"
#include <new>
#include <assert.h>
#include <string>
#include <string.h>
#include <stdio.h>

using namespace std;

BEGIN_AS_NAMESPACE

CScriptFile* ScriptFile_Factory()
{
	return new CScriptFile();
}

void RegisterScriptFile(asIScriptEngine* engine)
{
	int r;

	r = engine->RegisterObjectType("file", 0, asOBJ_REF); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("file", asBEHAVE_FACTORY, "file @f()", asFUNCTION(ScriptFile_Factory), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("file", asBEHAVE_ADDREF, "void f()", asMETHOD(CScriptFile, AddRef), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("file", asBEHAVE_RELEASE, "void f()", asMETHOD(CScriptFile, Release), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("file", "int open(const string &in, const string &in)", asMETHOD(CScriptFile, Open), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int close()", asMETHOD(CScriptFile, Close), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int getSize() const", asMETHOD(CScriptFile, GetSize), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "bool isEndOfFile() const", asMETHOD(CScriptFile, IsEOF), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "string readString(uint)", asMETHOD(CScriptFile, ReadString), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "string readLine()", asMETHOD(CScriptFile, ReadLine), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int64 readInt(uint)", asMETHOD(CScriptFile, ReadInt), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "uint64 readUInt(uint)", asMETHOD(CScriptFile, ReadUInt), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "float readFloat()", asMETHOD(CScriptFile, ReadFloat), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "double readDouble()", asMETHOD(CScriptFile, ReadDouble), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("file", "int writeString(const string &in)", asMETHOD(CScriptFile, WriteString), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int writeInt(int64, uint)", asMETHOD(CScriptFile, WriteInt), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int writeUInt(uint64, uint)", asMETHOD(CScriptFile, WriteUInt), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int writeFloat(float)", asMETHOD(CScriptFile, WriteFloat), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int writeDouble(double)", asMETHOD(CScriptFile, WriteDouble), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("file", "int getPos() const", asMETHOD(CScriptFile, GetPos), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int setPos(int)", asMETHOD(CScriptFile, SetPos), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int movePos(int)", asMETHOD(CScriptFile, MovePos), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectProperty("file", "bool mostSignificantByteFirst", asOFFSET(CScriptFile, mostSignificantByteFirst)); assert(r >= 0);
}

CScriptFile::CScriptFile()
{
	refCount = 1;
	file = 0;
	mostSignificantByteFirst = false;
}

CScriptFile::~CScriptFile()
{
	Close();
}

void CScriptFile::AddRef() const
{
	asAtomicInc(refCount);
}

void CScriptFile::Release() const
{
	if (asAtomicDec(refCount) == 0)
		delete this;
}

int CScriptFile::Open(const std::string& filename, const std::string& mode)
{
	// Close the previously opened file handle
	if (file)
		Close();

	std::string myFilename = filename;

	// Validate the mode
	string m;
	if (mode != "r" && mode != "w" && mode != "a")

		return -1;
	else
		m = mode;

	// By default windows translates "\r\n" to "\n", but we want to read the file as-is.
	m += "b";

	// Open the file
	// MSVC 8.0 / 2005 introduced new functions 
	// Marmalade doesn't use these, even though it uses the MSVC compiler
	fopen_s(&file, myFilename.c_str(), m.c_str());

	if (file == 0)
		return -1;

	return 0;
}

int CScriptFile::Close()
{
	if (file == 0)
		return -1;

	fclose(file);
	file = 0;

	return 0;
}

int CScriptFile::GetSize() const
{
	if (file == 0)
		return -1;

	int pos = ftell(file);
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, pos, SEEK_SET);

	return size;
}

int CScriptFile::GetPos() const
{
	if (file == 0)
		return -1;

	return ftell(file);
}

int CScriptFile::SetPos(int pos)
{
	if (file == 0)
		return -1;

	int r = fseek(file, pos, SEEK_SET);

	// Return -1 on error
	return r ? -1 : 0;
}

int CScriptFile::MovePos(int delta)
{
	if (file == 0)
		return -1;

	int r = fseek(file, delta, SEEK_CUR);

	// Return -1 on error
	return r ? -1 : 0;
}

string CScriptFile::ReadString(unsigned int length)
{
	if (file == 0)
		return "";

	// Read the string
	string str;
	str.resize(length);
	int size = (int)fread(&str[0], 1, length, file);
	str.resize(size);

	return str;
}

string CScriptFile::ReadLine()
{
	if (file == 0)
		return "";

	// Read until the first new-line character
	string str;
	char buf[256];

	do
	{
		// Get the current position so we can determine how many characters were read
		int start = ftell(file);

		// Set the last byte to something different that 0, so that we can check if the buffer was filled up
		buf[255] = 1;

		// Read the line (or first 255 characters, which ever comes first)
		char* r = fgets(buf, 256, file);
		if (r == 0) break;

		// Get the position after the read
		int end = ftell(file);

		// Add the read characters to the output buffer
		str.append(buf, end - start);
	} while (!feof(file) && buf[255] == 0 && buf[254] != '\n');

	return str;
}

asINT64 CScriptFile::ReadInt(asUINT bytes)
{
	if (file == 0)
		return 0;

	if (bytes > 8) bytes = 8;
	if (bytes == 0) return 0;

	unsigned char buf[8];
	size_t r = fread(buf, bytes, 1, file);
	if (r == 0) return 0;

	asINT64 val = 0;
	if (mostSignificantByteFirst)
	{
		unsigned int n = 0;
		for (; n < bytes; n++)
			val |= asQWORD(buf[n]) << ((bytes - n - 1) * 8);

		// Check the most significant byte to determine if the rest 
		// of the qword must be filled to give a negative value
		if (buf[0] & 0x80)
			for (; n < 8; n++)
				val |= asQWORD(0xFF) << (n * 8);
	}
	else
	{
		unsigned int n = 0;
		for (; n < bytes; n++)
			val |= asQWORD(buf[n]) << (n * 8);

		// Check the most significant byte to determine if the rest 
		// of the qword must be filled to give a negative value
		if (buf[bytes - 1] & 0x80)
			for (; n < 8; n++)
				val |= asQWORD(0xFF) << (n * 8);
	}

	return val;
}

asQWORD CScriptFile::ReadUInt(asUINT bytes)
{
	if (file == 0)
		return 0;

	if (bytes > 8) bytes = 8;
	if (bytes == 0) return 0;

	unsigned char buf[8];
	size_t r = fread(buf, bytes, 1, file);
	if (r == 0) return 0;

	asQWORD val = 0;
	if (mostSignificantByteFirst)
	{
		unsigned int n = 0;
		for (; n < bytes; n++)
			val |= asQWORD(buf[n]) << ((bytes - n - 1) * 8);
	}
	else
	{
		unsigned int n = 0;
		for (; n < bytes; n++)
			val |= asQWORD(buf[n]) << (n * 8);
	}

	return val;
}

float CScriptFile::ReadFloat()
{
	if (file == 0)
		return 0;

	unsigned char buf[4];
	size_t r = fread(buf, 4, 1, file);
	if (r == 0) return 0;

	asUINT val = 0;
	if (mostSignificantByteFirst)
	{
		unsigned int n = 0;
		for (; n < 4; n++)
			val |= asUINT(buf[n]) << ((3 - n) * 8);
	}
	else
	{
		unsigned int n = 0;
		for (; n < 4; n++)
			val |= asUINT(buf[n]) << (n * 8);
	}

	return *reinterpret_cast<float*>(&val);
}

double CScriptFile::ReadDouble()
{
	if (file == 0)
		return 0;

	unsigned char buf[8];
	size_t r = fread(buf, 8, 1, file);
	if (r == 0) return 0;

	asQWORD val = 0;
	if (mostSignificantByteFirst)
	{
		unsigned int n = 0;
		for (; n < 8; n++)
			val |= asQWORD(buf[n]) << ((7 - n) * 8);
	}
	else
	{
		unsigned int n = 0;
		for (; n < 8; n++)
			val |= asQWORD(buf[n]) << (n * 8);
	}

	return *reinterpret_cast<double*>(&val);
}

bool CScriptFile::IsEOF() const
{
	if (file == 0)
		return true;

	return feof(file) ? true : false;
}

int CScriptFile::WriteString(const std::string& str)
{
	if (file == 0)
		return -1;

	// Write the entire string
	size_t r = fwrite(&str[0], 1, str.length(), file);

	return int(r);
}

int CScriptFile::WriteInt(asINT64 val, asUINT bytes)
{
	if (file == 0)
		return 0;

	unsigned char buf[8];
	if (mostSignificantByteFirst)
	{
		for (unsigned int n = 0; n < bytes; n++)
			buf[n] = (val >> ((bytes - n - 1) * 8)) & 0xFF;
	}
	else
	{
		for (unsigned int n = 0; n < bytes; n++)
			buf[n] = (val >> (n * 8)) & 0xFF;
	}

	size_t r = fwrite(&buf, bytes, 1, file);
	return int(r);
}

int CScriptFile::WriteUInt(asQWORD val, asUINT bytes)
{
	if (file == 0)
		return 0;

	unsigned char buf[8];
	if (mostSignificantByteFirst)
	{
		for (unsigned int n = 0; n < bytes; n++)
			buf[n] = (val >> ((bytes - n - 1) * 8)) & 0xFF;
	}
	else
	{
		for (unsigned int n = 0; n < bytes; n++)
			buf[n] = (val >> (n * 8)) & 0xFF;
	}

	size_t r = fwrite(&buf, bytes, 1, file);
	return int(r);
}

int CScriptFile::WriteFloat(float f)
{
	if (file == 0)
		return 0;

	unsigned char buf[4];
	asUINT val = *reinterpret_cast<asUINT*>(&f);
	if (mostSignificantByteFirst)
	{
		for (unsigned int n = 0; n < 4; n++)
			buf[n] = (val >> ((3 - n) * 4)) & 0xFF;
	}
	else
	{
		for (unsigned int n = 0; n < 4; n++)
			buf[n] = (val >> (n * 8)) & 0xFF;
	}

	size_t r = fwrite(&buf, 4, 1, file);
	return int(r);
}

int CScriptFile::WriteDouble(double d)
{
	if (file == 0)
		return 0;

	unsigned char buf[8];
	asQWORD val = *reinterpret_cast<asQWORD*>(&d);
	if (mostSignificantByteFirst)
	{
		for (unsigned int n = 0; n < 8; n++)
			buf[n] = (val >> ((7 - n) * 8)) & 0xFF;
	}
	else
	{
		for (unsigned int n = 0; n < 8; n++)
			buf[n] = (val >> (n * 8)) & 0xFF;
	}

	size_t r = fwrite(&buf, 8, 1, file);
	return int(r);
}


END_AS_NAMESPACE
