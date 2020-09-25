#include "../../headers/Scripting/ZPScriptClass.hpp"
#include "../../headers/Scripting/AngelScript.hpp"

void call_func(asIScriptGeneric* gen) {
	asIScriptFunction* func = gen->GetFunction();
	const char* decl = func->GetDeclaration(false, false, false);
	ZPScriptClass* self = reinterpret_cast<ZPScriptClass*>(gen->GetObject());

	Engine::AGScript* _script = static_cast<Engine::AGScript*>(self->getScriptObject());
	_script->getFuncOnMainClass(decl);
}

asITypeInfo* getZSPScript(void* mgr) {
	Engine::AGScriptMgr* _mgr = static_cast<Engine::AGScriptMgr*>(mgr);
	asITypeInfo* type = 0;
	asIScriptModule* mod = _mgr->GetModule("ss", asGM_ONLY_IF_EXISTS);
	int tc = mod->GetObjectTypeCount();
	for (int n = 0; n < tc; n++)
	{
		bool found = false;
		type = mod->GetObjectTypeByIndex(n);
		int ic = type->GetInterfaceCount();
		for (int i = 0; i < ic; i++)
		{
			if (strcmp(type->GetInterface(i)->GetName(), "ZPScript") == 0)
			{
				return type;
			}
		}
	}
	return type;
}

void bindAgClass(std::string script_text, void* mgr) {
	Engine::AGScriptMgr* _mgr = static_cast<Engine::AGScriptMgr*>(mgr);

	CScriptBuilder builder;
	int result = 0;
	builder.StartNewModule(_mgr->getAgScriptEngine(), "ss");
	result = builder.AddSectionFromMemory("sect", script_text.c_str(), script_text.size());
	result = builder.BuildModule();


	
	

	asITypeInfo* class_type = getZSPScript(mgr);

	std::string _d = class_type->GetName();
	_mgr->RegisterObjectType(_d.c_str(), 0, asOBJ_REF | asOBJ_NOCOUNT);
	for (unsigned int f_i = 0; f_i < class_type->GetMethodCount(); f_i++) {
		asIScriptFunction* func = class_type->GetMethodByIndex(f_i);
		const char* decl = func->GetDeclaration(false, false, false);

		int res = _mgr->RegisterObjectMethod(_d.c_str(), decl, asFUNCTION(call_func), asCALL_GENERIC);
		//assert(res >= 0);
	}
}

void* ZPScriptClass::getScriptObject() {
	return script;
}

ZPScriptClass::ZPScriptClass(void* script) {
	this->script = script;
}