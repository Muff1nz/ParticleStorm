#pragma once

enum SystemComponent {
	SYSTEM_None = 0,
	SYSTEM_SessionManager = 1 << 1,
	SYSTEM_PhysicsEngine = 1 << 2,
	SYSTEM_RenderEngine = 1 << 3
};
