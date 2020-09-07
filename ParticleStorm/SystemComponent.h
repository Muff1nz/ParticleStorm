#pragma once

enum SystemComponent {
	SYSTEM_None = 0,
	SYSTEM_SessionManager = 1 << 1,
	SYSTEM_PhysicsEngine = 1 << 2,
	SYSTEM_RenderEngine = 1 << 3,
	SYSTEM_EventEngine = 1 << 4,
	SYSTEM_EntityEngine = 1 << 5,
	SYSTEM_CoreEngine = 1 << 6
};
