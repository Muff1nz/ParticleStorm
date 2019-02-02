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