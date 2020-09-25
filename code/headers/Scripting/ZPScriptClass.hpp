#ifndef zpscript_class
#define zpscript_class

#include <angelscript/angelscript.h>
#include <string>

class ZPScriptClass {
private:
    void* script;
public:
    void* getScriptObject();
    

    ZPScriptClass(void* script);
};

asITypeInfo* getZSPScript(void* mgr);
void bindAgClass(std::string script_text, void* mgr);
void call_func(asIScriptGeneric* gen);

#endif