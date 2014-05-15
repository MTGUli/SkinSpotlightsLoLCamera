#include "Stopwatch.h"

StopWatch::StopWatch( LuaPlus::LuaState *pState )
{
	LuaPlus::LuaObject globals = pState->GetGlobals();
	LuaPlus::LuaObject registry = pState->GetRegistry();

	// Create our metatable if not already set
	LuaPlus::LuaObject metaTable = registry["stopWatchMeta"];
	if( metaTable.IsNil() )
	{
		metaTable = registry.CreateTable( "stopWatchMeta" );
		metaTable.RegisterObjectFunctor("__index", &StopWatch::Index);
		//metaTable.RegisterObjectFunctor("__eq", &ObjectMgr::Equals);	
	}
	
	// Create our object table if not already set
	LuaPlus::LuaObject objectTable = registry["stopWatch"];
	if( objectTable.IsNil() )
	{
		objectTable = registry.CreateTable( "stopWatch" );
		objectTable.RegisterObjectFunctor("Start", &StopWatch::Start);
		objectTable.RegisterObjectFunctor("Stop", &StopWatch::Stop);
	}
	
	// Clone our table and set the __object pointer to the current object
	LuaPlus::LuaObject object = objectTable.Clone();
	object.SetLightUserData("__object", this );

	// Don't forget to set our metatable
	object.SetMetaTable(metaTable);

	// Make sure that you give your cloned object a name in our global space
	globals.SetObject("stopWatch",object);
}

int StopWatch::Index(LuaPlus::LuaState *pState )
{
	// Table, Key
	int top = pState->GetTop();
	if( top == 2 && pState->IsTable(1) && pState->IsString(2) )
	{
		// t[key]
		std::string key( pState->ToString(2) );
		if( key == "tick" ){
			if(bEnabled){
				deltaTimeMS = (newTime - old);
				totalTick += deltaTimeMS;
				old = newTime;
				newTime = timeGetTime();
				pState->PushInteger(deltaTimeMS);
			}else
				pState->PushInteger(0);
		}else if(key == "enabled")
			pState->PushBoolean(bEnabled);
		else if(key == "totalTick")
			pState->PushInteger(totalTick);
	}
	// return the count of our pushed values
	return pState->GetTop() - top;
}

int StopWatch::Start(LuaPlus::LuaState *pState){
	totalTick = 0;
	newTime = timeGetTime();
	old = newTime;
	bEnabled = true;
	return 1;
}

int StopWatch::Stop(LuaPlus::LuaState *pState){
	bEnabled = false;
	return 1;
}