#include <vector>

class ParticleBuffer {
public:
	void Add(int particle) {
		if (currentIndex < buffer.size()) {
			buffer[currentIndex] = particle;
		} else {
			buffer.push_back(particle);
		}
		++currentIndex;
	}

	int Size() const {
		return currentIndex;
	}

	void Clear() {
		currentIndex = 0;
	}

	int operator[](int index) {
		return buffer[index];
	}

private:
	std::vector<int> buffer;
	int currentIndex = 0;
};