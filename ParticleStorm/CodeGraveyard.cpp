//TODO: The below peieces of code are attempts at fixing the particle soup bug
//TODO:		They are methods of doing particle displacement after collisions
//TODO: Method 1
//float overlap = circleRadius * 2 - distance;
//if (alreadyMoved[particle] == alreadyMoved[i]) {
//	alreadyMoved[particle] = true; alreadyMoved[i] = true;
//	circlePos[particle] += -(positionDelta / distance) * (overlap / 2);
//	circlePos[i] += (positionDelta / distance) * (overlap / 2);
//} else if (!alreadyMoved[particle]) {
//	alreadyMoved[particle] = true;
//	circlePos[particle] += -(positionDelta / distance) * overlap;
//} else {
//	alreadyMoved[i] = true;
//	circlePos[i] += (positionDelta / distance) * overlap;
//}

//TODO: Method 2
//glm::vec2 top;
//glm::vec2 bottom;
//int topIndex;

//if (circlePos[particle].y > circlePos[i].y) {
//	topIndex = particle;
//	top = circlePos[particle];
//	bottom = circlePos[i];
//} else {
//	topIndex = i;
//	top = circlePos[i];
//	bottom = circlePos[particle];
//}

//float a = 1;
//float b = -2 * bottom.y;
//float c = pow(bottom.y, 2) - 4 * pow(circleRadius, 2) + pow(bottom.x - top.x, 2);
//circlePos[topIndex].y = QuadraticEquation(a, b, c);


//TODO: Method 3
////			This did solve the "Particle soup" problem, but made the particles unstable.
//			float overlap = circleRadius * 2 - distance;
//			if (circlePos[particle].y > circlePos[i].y)
//				circlePos[particle] += -(positionDelta / distance) * overlap;
//			else
//				circlePos[i] += positionDelta / distance * overlap;