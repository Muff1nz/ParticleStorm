#pragma once

enum MessageType {
	//Technical
	MT_Empty,

	//System events
	MT_ShutDown,
	MT_Shutdown_Session,
	MT_Config,

	//User triggered events
	MT_Explosion,
	MT_DebugModeToggle,
	MT_FullScreenToggle,

	//Engine events
	MT_Entity_Submit_Request,
	MT_Entity_Destroy_Request,
	
	MT_Entity_Submitted,
	MT_Entity_Destroyed
};