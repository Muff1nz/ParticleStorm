//######################################################################################
//This class was generated by a python script called: ParticleStorm_StatsClass_Generator
//This is done for performance reasons.
//Could write this class manually using dictionaries to make the data more managable.
//But that would impact performance
//######################################################################################



#pragma once
#include <atomic>
#include <string>


class Stats {
public:

	//Total counts for last second
	std::atomic_int physicsUpdateTotalLastSecond = 0;
	std::atomic_int renderUpdateTotalLastSecond = 0;
	std::atomic_int explosionTotalLastSecond = 0;
	std::atomic_int quadTreeSwapTotalLastSecond = 0;
	std::atomic_int quadTreeOverflowTotalLastSecond = 0;
	std::atomic_int quadTreeLeafTotalLastSecond = 0;
	std::atomic_int particleCollisionTotalLastSecond = 0;
	std::atomic_int physicsDeltaTimeTotalLastSecond = 0;
	std::atomic_int particlesRenderedTotalLastSecond = 0;


	void CompleteLastSecond() {
		physicsUpdateLastSecondTotalShadow = physicsUpdateTotalLastSecond;
		renderUpdateLastSecondTotalShadow = renderUpdateTotalLastSecond;
		explosionLastSecondTotalShadow = explosionTotalLastSecond;
		quadTreeSwapLastSecondTotalShadow = quadTreeSwapTotalLastSecond;
		quadTreeOverflowLastSecondTotalShadow = quadTreeOverflowTotalLastSecond;
		quadTreeLeafLastSecondTotalShadow = quadTreeLeafTotalLastSecond;
		particleCollisionLastSecondTotalShadow = particleCollisionTotalLastSecond;
		physicsDeltaTimeLastSecondTotalShadow = physicsDeltaTimeTotalLastSecond;
		particlesRenderedLastSecondTotalShadow = particlesRenderedTotalLastSecond;

		physicsUpdateTotalLastSecond = 0;
		renderUpdateTotalLastSecond = 0;
		explosionTotalLastSecond = 0;
		quadTreeSwapTotalLastSecond = 0;
		quadTreeOverflowTotalLastSecond = 0;
		quadTreeLeafTotalLastSecond = 0;
		particleCollisionTotalLastSecond = 0;
		physicsDeltaTimeTotalLastSecond = 0;
		particlesRenderedTotalLastSecond = 0;

		physicsUpdateTotal += physicsUpdateLastSecondTotalShadow;
		renderUpdateTotal += renderUpdateLastSecondTotalShadow;
		explosionTotal += explosionLastSecondTotalShadow;
		quadTreeSwapTotal += quadTreeSwapLastSecondTotalShadow;
		quadTreeOverflowTotal += quadTreeOverflowLastSecondTotalShadow;
		quadTreeLeafTotal += quadTreeLeafLastSecondTotalShadow;
		particleCollisionTotal += particleCollisionLastSecondTotalShadow;
		physicsDeltaTimeTotal += physicsDeltaTimeLastSecondTotalShadow;
		particlesRenderedTotal += particlesRenderedLastSecondTotalShadow;

		quadTreeSwapAverageLastSecond = quadTreeSwapLastSecondTotalShadow;
		quadTreeOverflowAverageLastSecond = quadTreeOverflowLastSecondTotalShadow;
		quadTreeLeafAverageLastSecond = quadTreeLeafLastSecondTotalShadow;
		particleCollisionAverageLastSecond = particleCollisionLastSecondTotalShadow;
		physicsDeltaTimeAverageLastSecond = physicsDeltaTimeLastSecondTotalShadow;
		particlesRenderedAverageLastSecond = particlesRenderedLastSecondTotalShadow;

		quadTreeSwapAverageLastSecond /= float(physicsUpdateLastSecondTotalShadow);
		quadTreeOverflowAverageLastSecond /= float(physicsUpdateLastSecondTotalShadow);
		quadTreeLeafAverageLastSecond /= float(physicsUpdateLastSecondTotalShadow);
		particleCollisionAverageLastSecond /= float(physicsUpdateLastSecondTotalShadow);
		physicsDeltaTimeAverageLastSecond /= float(physicsUpdateLastSecondTotalShadow);
		particlesRenderedAverageLastSecond /= float(renderUpdateLastSecondTotalShadow);
	}


	void CompleteSession() {
		quadTreeSwapAverage = quadTreeSwapTotal;
		quadTreeOverflowAverage = quadTreeOverflowTotal;
		quadTreeLeafAverage = quadTreeLeafTotal;
		particleCollisionAverage = particleCollisionTotal;
		physicsDeltaTimeAverage = physicsDeltaTimeTotal;
		particlesRenderedAverage = particlesRenderedTotal;

		quadTreeSwapAverage /= float(physicsUpdateTotal);
		quadTreeOverflowAverage /= float(physicsUpdateTotal);
		quadTreeLeafAverage /= float(physicsUpdateTotal);
		particleCollisionAverage /= float(physicsUpdateTotal);
		physicsDeltaTimeAverage /= float(physicsUpdateTotal);
		particlesRenderedAverage /= float(renderUpdateTotal);
	}


	std::string LastSecondToStringConsole() const {
		std::string str;
		str += "===============================================================\n";
		str += "PHYSICS_UPDATE_LAST_SECOND_TOTAL: " + std::to_string(physicsUpdateLastSecondTotalShadow) + "\n";
		str += "RENDER_UPDATE_LAST_SECOND_TOTAL: " + std::to_string(renderUpdateLastSecondTotalShadow) + "\n";
		str += "EXPLOSION_LAST_SECOND_TOTAL: " + std::to_string(explosionLastSecondTotalShadow) + "\n";
		str += "===============================================================\n";
		str += "QUAD_TREE_SWAP_AVERAGE_LAST_SECOND: " + std::to_string(quadTreeSwapAverageLastSecond) + "\n";
		str += "QUAD_TREE_OVERFLOW_AVERAGE_LAST_SECOND: " + std::to_string(quadTreeOverflowAverageLastSecond) + "\n";
		str += "QUAD_TREE_LEAF_AVERAGE_LAST_SECOND: " + std::to_string(quadTreeLeafAverageLastSecond) + "\n";
		str += "PARTICLE_COLLISION_AVERAGE_LAST_SECOND: " + std::to_string(particleCollisionAverageLastSecond) + "\n";
		str += "PHYSICS_DELTA_TIME_AVERAGE_LAST_SECOND: " + std::to_string(physicsDeltaTimeAverageLastSecond) + "\n";
		str += "PARTICLES_RENDERED_AVERAGE_LAST_SECOND: " + std::to_string(particlesRenderedAverageLastSecond) + "\n";
		str += "===============================================================\n";
		return str;
	}


	std::string LastSecondToString() const {
		std::string str;
		str += "PHYSICS_UPDATE_LAST_SECOND_TOTAL: " + std::to_string(physicsUpdateLastSecondTotalShadow) + "\n";
		str += "RENDER_UPDATE_LAST_SECOND_TOTAL: " + std::to_string(renderUpdateLastSecondTotalShadow) + "\n";
		str += "EXPLOSION_LAST_SECOND_TOTAL: " + std::to_string(explosionLastSecondTotalShadow) + "\n";
		str += "QUAD_TREE_SWAP_LAST_SECOND_TOTAL: " + std::to_string(quadTreeSwapLastSecondTotalShadow) + "\n";
		str += "QUAD_TREE_OVERFLOW_LAST_SECOND_TOTAL: " + std::to_string(quadTreeOverflowLastSecondTotalShadow) + "\n";
		str += "QUAD_TREE_LEAF_LAST_SECOND_TOTAL: " + std::to_string(quadTreeLeafLastSecondTotalShadow) + "\n";
		str += "PARTICLE_COLLISION_LAST_SECOND_TOTAL: " + std::to_string(particleCollisionLastSecondTotalShadow) + "\n";
		str += "PHYSICS_DELTA_TIME_LAST_SECOND_TOTAL: " + std::to_string(physicsDeltaTimeLastSecondTotalShadow) + "\n";
		str += "PARTICLES_RENDERED_LAST_SECOND_TOTAL: " + std::to_string(particlesRenderedLastSecondTotalShadow) + "\n";
		str += "QUAD_TREE_SWAP_AVERAGE_LAST_SECOND: " + std::to_string(quadTreeSwapAverageLastSecond) + "\n";
		str += "QUAD_TREE_OVERFLOW_AVERAGE_LAST_SECOND: " + std::to_string(quadTreeOverflowAverageLastSecond) + "\n";
		str += "QUAD_TREE_LEAF_AVERAGE_LAST_SECOND: " + std::to_string(quadTreeLeafAverageLastSecond) + "\n";
		str += "PARTICLE_COLLISION_AVERAGE_LAST_SECOND: " + std::to_string(particleCollisionAverageLastSecond) + "\n";
		str += "PHYSICS_DELTA_TIME_AVERAGE_LAST_SECOND: " + std::to_string(physicsDeltaTimeAverageLastSecond) + "\n";
		str += "PARTICLES_RENDERED_AVERAGE_LAST_SECOND: " + std::to_string(particlesRenderedAverageLastSecond) + "\n";
		return str;
	}


	std::string CompleteSessionToStringConsole() const {
		std::string str;
		str += "===============================================================\n";
		str += "PHYSICS_UPDATE_TOTAL: " + std::to_string(physicsUpdateTotal) + "\n";
		str += "RENDER_UPDATE_TOTAL: " + std::to_string(renderUpdateTotal) + "\n";
		str += "EXPLOSION_TOTAL: " + std::to_string(explosionTotal) + "\n";
		str += "QUAD_TREE_SWAP_TOTAL: " + std::to_string(quadTreeSwapTotal) + "\n";
		str += "QUAD_TREE_OVERFLOW_TOTAL: " + std::to_string(quadTreeOverflowTotal) + "\n";
		str += "QUAD_TREE_LEAF_TOTAL: " + std::to_string(quadTreeLeafTotal) + "\n";
		str += "PARTICLE_COLLISION_TOTAL: " + std::to_string(particleCollisionTotal) + "\n";
		str += "PHYSICS_DELTA_TIME_TOTAL: " + std::to_string(physicsDeltaTimeTotal) + "\n";
		str += "PARTICLES_RENDERED_TOTAL: " + std::to_string(particlesRenderedTotal) + "\n";
		str += "===============================================================\n";
		str += "QUAD_TREE_SWAP_AVERAGE: " + std::to_string(quadTreeSwapAverage) + "\n";
		str += "QUAD_TREE_OVERFLOW_AVERAGE: " + std::to_string(quadTreeOverflowAverage) + "\n";
		str += "QUAD_TREE_LEAF_AVERAGE: " + std::to_string(quadTreeLeafAverage) + "\n";
		str += "PARTICLE_COLLISION_AVERAGE: " + std::to_string(particleCollisionAverage) + "\n";
		str += "PHYSICS_DELTA_TIME_AVERAGE: " + std::to_string(physicsDeltaTimeAverage) + "\n";
		str += "PARTICLES_RENDERED_AVERAGE: " + std::to_string(particlesRenderedAverage) + "\n";
		str += "===============================================================\n";
		return str;
	}


	std::string CompleteSessionToString() const {
		std::string str;
		str += "PHYSICS_UPDATE_TOTAL: " + std::to_string(physicsUpdateTotal) + "\n";
		str += "RENDER_UPDATE_TOTAL: " + std::to_string(renderUpdateTotal) + "\n";
		str += "EXPLOSION_TOTAL: " + std::to_string(explosionTotal) + "\n";
		str += "QUAD_TREE_SWAP_TOTAL: " + std::to_string(quadTreeSwapTotal) + "\n";
		str += "QUAD_TREE_OVERFLOW_TOTAL: " + std::to_string(quadTreeOverflowTotal) + "\n";
		str += "QUAD_TREE_LEAF_TOTAL: " + std::to_string(quadTreeLeafTotal) + "\n";
		str += "PARTICLE_COLLISION_TOTAL: " + std::to_string(particleCollisionTotal) + "\n";
		str += "PHYSICS_DELTA_TIME_TOTAL: " + std::to_string(physicsDeltaTimeTotal) + "\n";
		str += "PARTICLES_RENDERED_TOTAL: " + std::to_string(particlesRenderedTotal) + "\n";
		str += "QUAD_TREE_SWAP_AVERAGE: " + std::to_string(quadTreeSwapAverage) + "\n";
		str += "QUAD_TREE_OVERFLOW_AVERAGE: " + std::to_string(quadTreeOverflowAverage) + "\n";
		str += "QUAD_TREE_LEAF_AVERAGE: " + std::to_string(quadTreeLeafAverage) + "\n";
		str += "PARTICLE_COLLISION_AVERAGE: " + std::to_string(particleCollisionAverage) + "\n";
		str += "PHYSICS_DELTA_TIME_AVERAGE: " + std::to_string(physicsDeltaTimeAverage) + "\n";
		str += "PARTICLES_RENDERED_AVERAGE: " + std::to_string(particlesRenderedAverage) + "\n";
		return str;
	}

private:

	//Total counts for entire session (from start to program closing)
	int physicsUpdateTotal = 0;
	int renderUpdateTotal = 0;
	int explosionTotal = 0;
	int quadTreeSwapTotal = 0;
	int quadTreeOverflowTotal = 0;
	int quadTreeLeafTotal = 0;
	int particleCollisionTotal = 0;
	int physicsDeltaTimeTotal = 0;
	int particlesRenderedTotal = 0;

	//Average per second for entire session (from start to program closing)
	float physicsUpdateAverage = 0;
	float renderUpdateAverage = 0;
	float explosionAverage = 0;
	float quadTreeSwapAverage = 0;
	float quadTreeOverflowAverage = 0;
	float quadTreeLeafAverage = 0;
	float particleCollisionAverage = 0;
	float physicsDeltaTimeAverage = 0;
	float particlesRenderedAverage = 0;

	//Average counts for last second
	float quadTreeSwapAverageLastSecond = 0;
	float quadTreeOverflowAverageLastSecond = 0;
	float quadTreeLeafAverageLastSecond = 0;
	float particleCollisionAverageLastSecond = 0;
	float physicsDeltaTimeAverageLastSecond = 0;
	float particlesRenderedAverageLastSecond = 0;

	//Total counts for last second(Shadow)
	int physicsUpdateLastSecondTotalShadow = 0;
	int renderUpdateLastSecondTotalShadow = 0;
	int explosionLastSecondTotalShadow = 0;
	int quadTreeSwapLastSecondTotalShadow = 0;
	int quadTreeOverflowLastSecondTotalShadow = 0;
	int quadTreeLeafLastSecondTotalShadow = 0;
	int particleCollisionLastSecondTotalShadow = 0;
	int physicsDeltaTimeLastSecondTotalShadow = 0;
	int particlesRenderedLastSecondTotalShadow = 0;
};
