//TODO: The below peieces of code are attempts at fixing the particle soup bug
//TODO:		They are methods of doing particle displacement after collisions
//TODO: Method 1
//float overlap = particleRadius * 2 - distance;
//if (alreadyMoved[particle] == alreadyMoved[i]) {
//	alreadyMoved[particle] = true; alreadyMoved[i] = true;
//	particlePos[particle] += -(positionDelta / distance) * (overlap / 2);
//	particlePos[i] += (positionDelta / distance) * (overlap / 2);
//} else if (!alreadyMoved[particle]) {
//	alreadyMoved[particle] = true;
//	particlePos[particle] += -(positionDelta / distance) * overlap;
//} else {
//	alreadyMoved[i] = true;
//	particlePos[i] += (positionDelta / distance) * overlap;
//}

//TODO: Method 2
//glm::vec2 top;
//glm::vec2 bottom;
//int topIndex;

//if (particlePos[particle].y > particlePos[i].y) {
//	topIndex = particle;
//	top = particlePos[particle];
//	bottom = particlePos[i];
//} else {
//	topIndex = i;
//	top = particlePos[i];
//	bottom = particlePos[particle];
//}

//float a = 1;
//float b = -2 * bottom.y;
//float c = pow(bottom.y, 2) - 4 * pow(particleRadius, 2) + pow(bottom.x - top.x, 2);
//particlePos[topIndex].y = QuadraticEquation(a, b, c);


//TODO: Method 3
////			This did solve the "Particle soup" problem, but made the particles unstable.
//			float overlap = particleRadius * 2 - distance;
//			if (particlePos[particle].y > particlePos[i].y)
//				particlePos[particle] += -(positionDelta / distance) * overlap;
//			else
//				particlePos[i] += positionDelta / distance * overlap;



//TODO: Fancy move against velocity displacement.
//glm::vec2 PhysicsEngine::DisplaceParticle(glm::vec2 particle1, glm::vec2 particle1Vel, glm::vec2 particle2) const {
//	//Equation for doing displacement along the highest trajectory (scenario when particle1 comes from above relative to particle2)
//	//Arrived at with pen and paper.
//	const glm::vec2 seperation = particle1 - particle2;
//	const float a = particle1Vel.x * particle1Vel.x + particle1Vel.y * particle1Vel.y;
//	const float b = 2 * particle1Vel.x * seperation.x + 2 * particle1Vel.y * seperation.y;
//	const float c = seperation.x * seperation.x + seperation.y * seperation.y - 4 * environment->particleRadius * environment->particleRadius;
//
//	const float thingie = sqrt(b * b - 4 * a * c);
//	auto t1 = (-b + thingie) / (2 * a);
//	auto t2 = (-b - thingie) / (2 * a);
//	auto p1 = particle1 + t1 * particle1Vel;
//	auto p2 = particle1 + t2 * particle1Vel;
//	return p1.y >= p2.y ? p1 : p2;
//}